#include <esp_now.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "tank_functions.h"
#include <BLEDevice.h>

const char* ssid = "";  // Enter the name of your WiFi
const char* password = "";       // Enter WiFi password. Leave empty if none

String serverIP = "";  // Assigned after server connects to tank

WebServer server(80);

void setup() {
  Serial.begin(115200);
  initWiFi(ssid, password);
  
  initBLE(WiFi.macAddress(), WiFi.localIP());

  // Telling the tank how to handle different commands
  server.on("/command", HTTP_POST, handleCommand);  // Handle /command URL with POST request
  // server.on("/blink", HTTP_POST, handleBlink);      // For telling headlights to blink.

  server.begin();
}

void loop() {
  server.handleClient();
}
