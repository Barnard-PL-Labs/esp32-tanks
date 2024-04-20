#ifndef TANK_FUNCTIONS_H
#define TANK_FUNCTIONS_H

#include <WebServer.h>

extern WebServer server;
extern const char*  serverIP;

void executeCommand(char direction, int value);

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
void checkForCommands();
// Headlamp Functions
void blinkHeadlamp(int num);

void handleBlink();

// Init Functions
void initMotors();
void initHeadlamp();
void initWiFi(const char* ssid, const char* password);

#endif
