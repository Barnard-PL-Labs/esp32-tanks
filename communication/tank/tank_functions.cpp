#include <WiFi.h>
#include "stdlib.h"
#include "config.h"
#include <BLEDevice.h>
#include <WebServer.h>

// For BTE (Bluetooth)
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");  // The remote service we wish to connect to.
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");     // The characteristic of the remote service we are interested in.
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

extern WebServer server;
extern String serverIP;

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

// use for easy physical feedback
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

void executeCommand(char function, int value) {
  switch (function) {
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

// A simple check to make sure we have the server's IP saved.
void checkForServerIP() {
  if (serverIP.length() < 1) {
    IPAddress remoteIP = server.client().remoteIP();
    serverIP = String(remoteIP[0]) + "." + String(remoteIP[1]) + "." + String(remoteIP[2]) + "." + String(remoteIP[3]);
    Serial.println("serverIP set to: " + serverIP);
  }
}

//Input should be in the form of function+value. For left and right ('l','r'), the value is the degrees. For
// forward and backward ('f','b'), the value is the milliseconds it drives for. For 'h' (for blinking) value 
// is how many times to blink.  For 'c' (changing headlamp) value is how bright to set headlamp
void handleCommand() {
  checkForServerIP();
  String command = server.arg("command");

  if (command.length() >= 3) {
    char direction = command.charAt(0);
    int value = command.substring(2).toInt();

    executeCommand(direction, value);

    server.send(200, "text/plain", "Command received successfully");
  } else {
    server.send(400, "text/plain", "Invalid command format");
  }
}

void handleBlink() {
  checkForServerIP();

  int num = server.arg("num").toInt();
  blinkHeadlamp(num);
  server.send(200, "text/plain", "Blink received successfully");
}


static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  Serial.write(pData, length);
  Serial.println();
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();

  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remote BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  connected = true;

  return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.print("BLE Advertised Device found: ");
    //Serial.println(advertisedDevice.toString().c_str());
    //Serial.println("77");

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }  // Found our server
  }    // onResult
};     // MyAdvertisedDeviceCallbacks


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

void initBLE(String mac, IPAddress ip) {
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();

  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);


  if (doConnect == true) {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    BLEClient* pClient = BLEDevice::createClient();

    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remote BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
    }
    Serial.println(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if (pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if (pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;

    // If we are connected to a peer BLE Server, update the characteristic each time we are reached
    // with the current time since boot.
    if (connected) {
      String ipString = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
      String newValue = mac + "*" + ipString;

      Serial.println("Setting new characteristic value to \"" + newValue + "\"");

      // Set the characteristic's value to be the array of bytes that is actually a string.
      pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    } else if (doScan) {
      BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
    }
    pClient->disconnect();
  }
}