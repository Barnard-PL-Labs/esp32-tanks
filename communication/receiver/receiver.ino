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
StaticJsonDocument<96> doc;
StaticJsonDocument<96> responseJsonDocument;

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
        const char* function = doc["function"];
        
        if (strcmp(function, "motor") == 0) {
            // motor function
            const char* dir = doc["direction"];
            char direction = dir[0];
            int val = doc["value"]; 
            executeMotorCommand(direction, val);
        } else if (strcmp(function, "light") == 0) {
            // light function
            int val = doc["value"]; 
            executeLightCommand(val);
        } else if (strcmp(function, "camera") == 0) {
            // camera function
            // TODO: handle camera command
        } else {
            responseJsonDocument["status"] = "error";
            responseJsonDocument["message"] = "Unknown function";
            sendResponse();
        }
    } else {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        responseJsonDocument["status"] = "error";
        responseJsonDocument["message"] = "Invalid JSON";
        sendResponse();
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
  initLight();
  
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
void executeMotorCommand(char direction, int value) {
  char responseMessage[128];
  
  if (value <= 0) {
    responseJsonDocument["status"] = "error";
    responseJsonDocument["message"] = "Motor invalid value";
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
        responseJsonDocument["status"] = "error";
        responseJsonDocument["message"] = "Motor invalid direction";
        sendResponse();
    }
    snprintf(responseMessage, sizeof(responseMessage), "Motor command %c%d executed", direction, value);
    responseJsonDocument["status"] = "success";
    responseJsonDocument["message"] = responseMessage;
  }

  // Send response to coordinator
  sendResponse();
}

void executeLightCommand(int value) {
  char responseMessage[128];
  if (value < 0 || value > 255) {
    responseJsonDocument["status"] = "error";
    responseJsonDocument["message"] = "Light invalid value";
  } else {
    // TODO: implement light control
    adjustLight(value);
    snprintf(responseMessage, sizeof(responseMessage), "Light command %d executed", value);
    responseJsonDocument["status"] = "success";
    responseJsonDocument["message"] = responseMessage;
  }

  // Send response to coordinator
  sendResponse();
}

void sendResponse(){
    char jsonBuffer[96];

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
    serializeJson(responseJsonDocument, jsonBuffer);
    Serial.println(jsonBuffer);

    // Send Response
    esp_err_t sendResult = esp_now_send(senderMacAddress, (uint8_t *)jsonBuffer, strlen(jsonBuffer) + 1);

    // Print errors
    if (sendResult != ESP_OK) {
        Serial.print("Send failed, error: ");
        Serial.println(esp_err_to_name(sendResult));
    } 

}

void loop() {
 
}
