#include <WiFi.h>
#include "stdlib.h"
#include "config.h"
#include <BLEDevice.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>



extern WebServer server;
extern const char* serverIP;
extern String serverName;
/**** Tank Movement Functions ****/

void accelerateForward() {
  analogWrite(Left_Motor_Reverse, 0);
  analogWrite(Right_Motor_Reverse, 0);
  for (int i = 0; i < 255; i = i + 4) {
    analogWrite(Right_Motor_Forward, i);
    analogWrite(Left_Motor_Forward, i);
    delay(10);
  }
}

void accelerateForward(int duration) {
  analogWrite(Left_Motor_Reverse, 0);
  analogWrite(Right_Motor_Reverse, 0);

  for (int i = 0; i < 255; i = i + 4) {
    analogWrite(Right_Motor_Forward, i);
    analogWrite(Left_Motor_Forward, i);
    delay(10);
  }

  delay(duration);

  for (int i = 255; i >= 0; i = i - 4) {
    analogWrite(Right_Motor_Forward, i);
    analogWrite(Left_Motor_Forward, i);
    delay(5);
  }
}
void accelerateBackward() {
  analogWrite(Left_Motor_Forward, 0);
  analogWrite(Right_Motor_Forward, 0);

  for (int i = 0; i < 255; i = i + 4) {
    analogWrite(Right_Motor_Reverse, i);
    analogWrite(Left_Motor_Reverse, i);
    delay(10);
  }
}

void accelerateBackward(int duration) {
  analogWrite(Left_Motor_Forward, 0);
  analogWrite(Right_Motor_Forward, 0);

  for (int i = 0; i < 255; i = i + 4) {
    analogWrite(Right_Motor_Reverse, i);
    analogWrite(Left_Motor_Reverse, i);
    delay(10);
  }

  delay(duration);

  for (int i = 255; i >= 0; i = i - 4) {
    analogWrite(Right_Motor_Reverse, i);
    analogWrite(Left_Motor_Reverse, i);
    delay(5);
  }
}

void turnLeft(int degrees) {
  int duration = int(degrees * 4.0);

  analogWrite(Left_Motor_Forward, 0);
  analogWrite(Right_Motor_Reverse, 0);

  analogWrite(Right_Motor_Forward, 255);
  analogWrite(Left_Motor_Reverse, 255);

  delay(duration);
  analogWrite(Right_Motor_Forward, 0);
  analogWrite(Left_Motor_Reverse, 0);
}

void turnRight(int degrees) {
  int duration = int(degrees * 4.0);

  analogWrite(Left_Motor_Forward, 0);
  analogWrite(Right_Motor_Reverse, 0);

  analogWrite(Right_Motor_Reverse, 255);
  analogWrite(Left_Motor_Forward, 255);

  delay(duration);

  analogWrite(Right_Motor_Reverse, 0);
  analogWrite(Left_Motor_Forward, 0);
}

void rightMotorForward() {
  analogWrite(Right_Motor_Reverse, 0);

  for (int i = 0; i < 255; i = i + 4) {
    analogWrite(Right_Motor_Reverse, i);
    delay(8);
  }
}
void leftMotorForward() {
  analogWrite(Left_Motor_Reverse, 0);

  for (int i = 0; i < 255; i = i + 4) {
    analogWrite(Left_Motor_Forward, i);
    delay(8);
  }
}

void rightMotorReverse() {
  analogWrite(Right_Motor_Reverse, 0);

  for (int i = 0; i < 255; i = i + 4) {
    analogWrite(Right_Motor_Forward, i);
    delay(8);
  }
}
void leftMotorReverse() {
  analogWrite(Left_Motor_Forward, 0);

  for (int i = 0; i < 255; i = i + 4) {
    analogWrite(Left_Motor_Reverse, i);
    delay(8);
  }
}

void rightMotorStop() {
  analogWrite(Right_Motor_Forward, 0);
  analogWrite(Right_Motor_Reverse, 0);
}


void leftMotorStop() {
  analogWrite(Left_Motor_Forward, 0);
  analogWrite(Left_Motor_Reverse, 0);
}

void forward() {
  leftMotorForward();
  rightMotorForward();
}

void reverse() {
  leftMotorReverse();
  rightMotorReverse();
}

void stop() {
  analogWrite(Right_Motor_Forward, 0);
  analogWrite(Left_Motor_Reverse, 0);
  leftMotorStop();
  rightMotorStop();
}

/******* Headlamp Functions ******/

void blinkHeadlamp(int num) {
   for (int i = 0; i < num; i++) {
     analogWrite(Headlamp_Pin, 255);
     delay(150);
     analogWrite(Headlamp_Pin, 0);
     delay(150);
   }
 }

 void changeHeadlamp(int num){
   analogWrite(Headlamp_Pin, num);
 }

void executeCommand(char direction, int value) {
  switch (direction) {
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
    case 'h':
       blinkHeadlamp(value);
    case 'c':
       changeHeadlamp(value);
    default:
      Serial.println("Invalid Command");
  }
}

/******* Network Functions *******/

void checkForCommands() {
  if (serverName.length() > 0) {
    HTTPClient http;
    //Serial.println("\nRequesting command...");
    http.begin(serverName);  // Construct the URL for the GET request

    int httpCode = http.GET();

    if (httpCode == 200) {  // Check for OK response
      String payload = http.getString();  // Get the command

      StaticJsonDocument<200> doc;  // Create a JSON document 
      DeserializationError error = deserializeJson(doc, payload);  // Parse the json

      if (!error) {  // Check if it succeeded
        const char* command = doc["command"];  // Extract the command string


        char direction = command[0];  // Get the first character as the direction
        int value = String(&command[2]).toInt();  // Convert the rest to an integer

        executeCommand(direction, value); 
      } else {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("HTTP request failed with code: ");
      Serial.println(httpCode);
    }

    http.end();  // Free resources
   }
}


/******** Init Functions *********/
void initMotors() {
  pinMode(Left_Motor_Forward, OUTPUT);
  pinMode(Left_Motor_Reverse, OUTPUT);
  pinMode(Right_Motor_Forward, OUTPUT);
  pinMode(Right_Motor_Reverse, OUTPUT);
}

void initWiFi(const char* ssid, const char* password) {

  if (strlen(password) > 0) {
    WiFi.begin(ssid, password);
  } else {
    WiFi.begin(ssid);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void initHeadlamp() {
  pinMode(Headlamp_Pin, OUTPUT);
}

