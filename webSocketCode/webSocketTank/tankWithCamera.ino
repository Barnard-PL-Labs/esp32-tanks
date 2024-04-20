#include <esp_now.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "tank_functions.h"
#include <BLEDevice.h>
#include "esp_camera.h"
#include <HTTPClient.h>
#include <ArduinoWebsockets.h> // ArduinoWebsockets by Gil Maimon from the arduino library


const char* ssid = "Columbia University";  // Enter the name of your WiFi
const char* password = "";       // Enter WiFi password. Leave empty if none

const char* websocket_server = "ws://45.55.200.204:5001/ws/device/device2"; // IP address, i.e.  ws://192.168.1.100:5001
// The device has to changed per each device that is supposed to get connected, device1 device2 device3 etc
using namespace websockets;

WebsocketsClient webSocket;

void executeCommand();

void setup() {
  Serial.begin(115200);
  initWiFi(ssid, password);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  // Camera Setup
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_VGA; // was FRAMSIZE_UXGA, changed to VGA for smaller frame for testing
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  // config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 5; // was 12, reduced for testing purposes
  config.fb_count = 1;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(config.pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config.jpeg_quality = 10;
      config.fb_count = 2; //was 2
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.println(F("Camera init failed"));
    return;
  } else {
    // digitalWrite(Headlamp_Pin, HIGH);
    Serial.println(F("Camera init complete!!"));
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if(config.pixel_format == PIXFORMAT_JPEG){
    s->set_framesize(s, FRAMESIZE_QVGA);
  }


    // Setup WebSocket connection
webSocket.onMessage([](WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());

    // Attempt to parse JSON only if the message might be JSON
    if (message.data().startsWith("{")) {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, message.data());
        if (!error) {
            const char* function = doc["function"];
            int value = doc["value"].as<int>();
            executeCommand(function[0], value);
        } else {
            Serial.println("Failed to parse JSON!");
        }
    } else {
        // Handle non-JSON messages (shouldn't receive any really)
        Serial.println("Received non-JSON message.");
    }
});


    webSocket.onEvent([](WebsocketsClient& client, WebsocketsEvent event, String data) {
        if(event == WebsocketsEvent::ConnectionOpened) {
            Serial.println("Connection Opened");
        } else if(event == WebsocketsEvent::ConnectionClosed) {
            Serial.println("Connection Closed");
        }
    });


   webSocket.connect(websocket_server);
}

void loop() {

    webSocket.poll();

  if (webSocket.available()) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Send frame as binary
    webSocket.sendBinary((const char*)fb->buf, fb->len);

    // Return the frame buffer back to the driver for reuse
    esp_camera_fb_return(fb);
  }

  // Can adjust delay and experiment to see what performs best 
  delay(100);

}


  