#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // The broadcast MAC Address

typedef struct struct_message {
  char text[128];
} struct_message;

struct_message outgoingMessage;

// global variables for json message
String jsondata;
StaticJsonDocument<64> doc; // 64 computed from https://arduinojson.org/v6/assistant/

esp_now_peer_info_t peerInfo;

// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&outgoingMessage, incomingData, sizeof(outgoingMessage));
  Serial.print("Data received: ");
  Serial.println(outgoingMessage.text);
}

// Callback function executed when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);

  // Set ESP32 as a Wi-Fi Station and print MAC
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback functions
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Add broadcast MAC to peers
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Please input a direction (f,b,l,r) and value (degrees for l,r and milliseconds for f,b) with a space in between. E.g. f 1000");
}

void loop() {
   static String inputString = "";
   // read and send string from serial buffer
   if (Serial.available()) {
     inputString = Serial.readString(); 
     inputString.trim();

     // split string based on spaces
     int delimIndex = inputString.indexOf(' ');
     String direction = inputString.substring(0, delimIndex);
     String value = inputString.substring(delimIndex + 1);

     // create json struct and serialize
     jsondata = "";
     doc["direction"] = direction;
     doc["value"] = value;
     serializeJson(doc, jsondata);
     Serial.println(jsondata);
     Serial.println(sizeof(jsondata));

     // send message
     esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) jsondata.c_str(), strlen(jsondata.c_str()));
     if (result != ESP_OK) {
       Serial.print("Send failed, error: ");
       Serial.println(esp_err_to_name(result));
     }
     inputString = ""; 
  }
}
