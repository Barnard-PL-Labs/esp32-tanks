#include <esp_now.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "config.h"
#include "tank_functions.h"


typedef struct struct_message {
  char text[128];
} struct_message;

struct_message incomingMessage;
struct_message myResponse;

// global variables for json message
String jsondata;
StaticJsonDocument<64> doc;

esp_now_peer_info_t peerInfo;
uint8_t senderMacAddress[6];

// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  // receive json data and deserialize
  char* buff = (char*) incomingData;       
  jsondata = String(buff);                  
  Serial.print("Recieved ");
  Serial.println(jsondata);
  DeserializationError error = deserializeJson(doc, jsondata);

  memcpy(senderMacAddress, mac, 6); 
  
  if (!error) {
    // read arguments and execute command
    const char* dir = doc["direction"]; // must be const char*? Not sure why
    char direction = dir[0];
    int val = doc["value"]; 
    executeCommand(direction, val);
  }
  else {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
}

// Callback function executed when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Callback Delivery Success" : "Callback Delivery Fail");
}

// Formats MAC Address if needed
void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}
 
void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  
  // Initialize Motor pins
  initMotors();
  
  // Set ESP32 as a Wi-Fi Station and print MAC
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback functions
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}

/* Input should be in the form of direction+value. For left and right ('l','r'), the value is the degrees. For
 * forward and backward ('f','b'), the value is the milliseconds it drives for.
 */
void executeCommand(char direction, int value) {
  char responseMessage[128];
  
  if (value == 0) {
    strncpy(responseMessage, "Error: Invalid value", sizeof(responseMessage));
  } else{
    switch(direction) {
      case 'f':
        accelerateForward(value);
        break;
      case 'b':
        accelerateBackward(value);
        break;
      case 'l':
        turnLeft(value);
        break;
      case 'r':
        turnRight(value);
        break;
      default:
        strncpy(responseMessage, "Error: Invalid direction", sizeof(responseMessage));
    }
    snprintf(responseMessage, sizeof(responseMessage), "Command %c%d executed", direction, value);
  }

  // Send response to coordinator
  sendResponse(responseMessage);
}

void sendResponse(char * responseMessage){
    Serial.println(responseMessage);

    memcpy(peerInfo.peer_addr, senderMacAddress, 6); 
    peerInfo.channel = 0;  
    peerInfo.encrypt = false; 
    
    // Attempt to add the peer. 
    esp_err_t addPeerResult = esp_now_add_peer(&peerInfo);
    if (addPeerResult != ESP_OK && addPeerResult != ESP_ERR_ESPNOW_EXIST) {
        Serial.print("Failed to add peer, error: ");
        Serial.println(esp_err_to_name(addPeerResult));
        return; 
    }

    // Prepare response
    strncpy(myResponse.text, responseMessage, sizeof(myResponse.text) - 1);
    myResponse.text[sizeof(myResponse.text) - 1] = '\0';

    // Send Response
    esp_err_t sendResult = esp_now_send(senderMacAddress, (uint8_t *)&myResponse, sizeof(myResponse));

    // Print errors
    if (sendResult != ESP_OK) {
        Serial.print("Send failed, error: ");
        Serial.println(esp_err_to_name(sendResult));
    } 

}

void loop() {
 
}
