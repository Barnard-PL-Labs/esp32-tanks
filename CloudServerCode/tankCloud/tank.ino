#include <esp_now.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "tank_functions.h"
#include <HTTPClient.h>


const char* ssid = "MyWiFi";  // Enter the name of your WiFi
const char* password = "";       // Enter WiFi password. Leave empty if none

String serverIP = "http://192.168.1.41:8080";  // My current local IP for testing, if I forget to change this I think the 
                                              // virtual machine will have its own

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