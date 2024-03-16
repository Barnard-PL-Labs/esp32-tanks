#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

WebServer server(80);

String command = ""; // This will store the most recent command

void handleRoot() {
  String html = "<html><body>"
                "<h1>ESP32 Tank Control</h1>"
                "<form id='tankForm'>"
                "Directions: (f, b, l, r): <input type='text' name='direction' id='direction'><br>"
                "Value: <input type='text' name='value' id='value'><br>"
                "<input type='button' value='Submit' onclick='submitForm()'>"
                "</form>"
                "<script>"
                "function submitForm() {"
                "  var direction = document.getElementById('direction').value;"
                "  var value = document.getElementById('value').value;"
                "  var xhr = new XMLHttpRequest();"
                "  xhr.open('POST', '/command', true);"
                "  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"
                "  xhr.send('direction=' + direction + '&value=' + value);"
                "  xhr.onload = function() {"
                "    alert('Command sent: ' + direction + ' ' + value);"
                "    document.getElementById('tankForm').reset();"
                "  };"
                "}"
                "</script>"
                "</body></html>";
  server.send(200, "text/html", html);
}

void handleCommand() {
  if (server.hasArg("direction") && server.hasArg("value")) {
    command = server.arg("direction") + " " + server.arg("value");
    Serial.println("New command: " + command);
    server.send(200, "text/plain", command);
  } else {
    server.send(200, "text/plain", "Invalid command");
  }
}

void handleCheckCommand() {
  server.send(200, "text/plain", command);
  command = ""; // Clear the command after sending
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/command", HTTP_POST, handleCommand);
  server.on("/check-command", HTTP_GET, handleCheckCommand);
  server.begin();
}

void loop() {
  server.handleClient();
}


