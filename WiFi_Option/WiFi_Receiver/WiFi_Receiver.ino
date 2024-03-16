#include <WiFi.h>
#include "config.h"
#include "tank_functions.h"

const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";
const char* serverIP = "192.168.4.1"; // AP's IP

WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  if (client.connect(serverIP, 80)) {
    client.println("GET /check-command HTTP/1.1");
    client.println("Host: " + String(serverIP));
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("Headers received");
        break;
      }
    }

    String command = client.readStringUntil('\n');
    Serial.println("Command: " + command);
    char action = command.charAt(0);
    int spaceIndex = command.indexOf(' ');
    String valueString = command.substring(spaceIndex + 1);
    int value = valueString.toInt();
    if (action == 'f'){
      accelerateForward(value);
    }
    else if (action == 'b'){
      accelerateBackward(value);
    }
    else if (action == 'l'){
      turnLeft(value);
    }
    else{
      turnRight(value);
    }
    client.stop();
  } else {
    Serial.println("Failed to connect to server");
  }

  delay(5000); // Check for new commands every 5 seconds
}
