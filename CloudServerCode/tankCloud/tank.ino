#include <esp_now.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "tank_functions.h"
#include <HTTPClient.h>


const char* ssid = "";  // Enter the name of your WiFi
const char* password = "";       // Enter WiFi password. Leave empty if none

String serverIP = "http://159.203.73.83:8080";  // The VM's IP address 

WebServer server(80);

void setup() {
  Serial.begin(115200);
  initWiFi(ssid, password);


  server.begin();
}

void loop() {
  checkForCommands();
  delay(1000);
}