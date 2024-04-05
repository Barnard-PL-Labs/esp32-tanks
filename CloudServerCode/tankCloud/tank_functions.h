#ifndef TANK_FUNCTIONS_H
#define TANK_FUNCTIONS_H

#include <WebServer.h>

extern WebServer server;
extern String serverIP;

void executeCommand(char direction, int value);

// Network Function
void checkForCommands();

// Tank Movement Functions
void handleCommand();
void accelerateForward();
void accelerateForward(int duration);
void accelerateBackward();
void accelerateBackward(int duration);
void turnLeft(int degrees);
void turnRight(int degrees);
void rightMotorForward();
void leftMotorForward();
void rightMotorReverse();
void leftMotorReverse();
void rightMotorStop();
void leftMotorStop();
void forward();
void reverse();
void stop();

// Headlamp Functions
void blinkHeadlamp(int num);

void handleBlink();

// Init Functions
void initMotors();
void initHeadlamp();
void initBLE(String mac, IPAddress ip);
void initWiFi(const char* ssid, const char* password);

#endif