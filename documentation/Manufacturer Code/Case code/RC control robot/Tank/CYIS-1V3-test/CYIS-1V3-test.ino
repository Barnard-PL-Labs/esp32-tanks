/*
BILIBILI:技术宅物语
*/
#include "esp_camera.h"
#include <WiFi.h>
#include "Arduino.h"
#include "config.h"
#include "function.h"
#include <ArduinoJson.h>

char WifiName[] = {"esp"};
char WifiPassword[] = {"12345678"};


void setup() {
  //初始化串口
  Serial.begin(115200);

  //外设初始化
  headlampInit();
  motorInit();
  servoInit();
  servoSet(90);

  //摄像头配置信息
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 15000000;
  
  config.pixel_format = PIXFORMAT_JPEG;//PIXFORMAT_RGB565;
  config.frame_size = FRAMESIZE_240X240;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  //摄像头初始化
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);//摄像头初始化失败，结束函数
    return;
  }
  sensor_t * s = esp_camera_sensor_get();
  s->set_hmirror(s, 1); //垂直翻转
  //s->set_vflip(s, 1); //水平翻转

  // 连接WIFI
  headlampSet(100);
  Serial.printf("SSID:%s  PASS:%s\r\n",WifiName,WifiPassword);

  int n1=0;
  WiFi.begin(WifiName, WifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    headlampSet(0);
    delay(400);
    headlampSet(100);
    delay(100);
    Serial.print(".");
    n1++;
    if(n1>30)
      ESP.restart();
  }
  headlampSet(0);
  Serial.println("");
  Serial.println("WiFi connected");

  //打印本地IP
  String ip1 = WiFi.localIP().toString();
  ip1.toCharArray(localAddress, ip1.length()+1);
  Serial.printf("localAddress:%s\r\n",localAddress);
  
  // 启动通信线程，运行在内核0
  camera_state = xSemaphoreCreateBinary();//二进制信号量
  sendJpg_state = xSemaphoreCreateBinary();//二进制信号量
  xTaskCreatePinnedToCore(Communication, "Communication", 4096, NULL, 1, NULL,  0);
  
}

void loop() {
  
  capture_handler();
  delay(300);
}
