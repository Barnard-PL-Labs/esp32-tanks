/*
BILIBILI:技术宅物语
*/

#include <WiFi.h>
#include <WiFiAP.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <JPEGDecoder.h>

#include "lcd_init.h"
#include "lcd.h"
#include "lcdfont.h"

#define Rheostat_Rx_PIN 5
#define Rheostat_Ry_PIN 6
#define Rheostat_Lx_PIN 1
#define Rheostat_Ly_PIN 2

#define ButtonA_PIN 48
#define ButtonB_PIN 47
#define ButtonC_PIN 21

#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

WiFiUDP udp;

IPAddress local_IP(192, 168, 1, 1);  
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
// Set these to your desired credentials.
const char *ssid = "esp";
const char *password = "12345678";
char udpAddress[]={"192.168.1.2"};
char localAddress[20]={0};
const uint16_t udpPort = 10000;
uint16_t lcdCanvas[16][240];//画布缓存

uint8_t udp_Rbuff[1024]; //UDP接收缓存
uint8_t jpg_Buff[10240]; //jpg缓存
uint32_t jpg_len = 0;
uint32_t jpg_len_old = 0;
uint8_t id = 0;

int Rheostat_Rx_value;
int Rheostat_Ry_value;
int Rheostat_Lx_value;
int Rheostat_Ly_value;
uint8_t ButtonA_value=1;
uint8_t ButtonB_value=1;
uint8_t ButtonC_value=1;
uint8_t lampValue=0;

// 发送数据
void sendDate(){
  uint8_t buff[10]={0};
  
  if(Rheostat_Rx_value>2303){
    buff[0] = 127 + (Rheostat_Rx_value-2303)/14;
  }else if(Rheostat_Rx_value<1778){
    buff[0] = 127 - (1778-Rheostat_Rx_value)/14;
  }else{
    buff[0] = 127;
  }

  if(Rheostat_Ry_value>2303){
    buff[1] = 127 + (Rheostat_Ry_value-2303)/14;
  }else if(Rheostat_Ry_value<1778){
    buff[1] = 127 - (1778-Rheostat_Ry_value)/14;
  }else{
    buff[1] = 127;
  }

  if(Rheostat_Lx_value>2303){
    buff[2] = 127 + (Rheostat_Lx_value-2303)/14;
  }else if(Rheostat_Lx_value<1778){
    buff[2] = 127 - (1778-Rheostat_Lx_value)/14;
  }else{
    buff[2] = 127;
  }

  if(Rheostat_Ly_value>2303){
    buff[3] = 127 + (Rheostat_Ly_value-2303)/14;
  }else if(Rheostat_Ly_value<1778){
    buff[3] = 127 - (1778-Rheostat_Ly_value)/14;
  }else{
    buff[3] = 127;
  }
  
  buff[4] = lampValue;
  buff[5] = buff[0] + buff[1] + buff[2] + buff[3] + buff[4];
  
  udp.beginPacket(udpAddress,udpPort);
  udp.write(buff, 6);
  udp.endPacket();
}

//====================================================================================
//jpg图片信息显示
//====================================================================================
void jpegInfo() {
  Serial.println(F("==============="));
  Serial.println(F("JPEG image info"));
  Serial.println(F("==============="));
  Serial.print(F(  "Width      :")); Serial.println(JpegDec.width);
  Serial.print(F(  "Height     :")); Serial.println(JpegDec.height);
  Serial.print(F(  "Components :")); Serial.println(JpegDec.comps);
  Serial.print(F(  "MCU / row  :")); Serial.println(JpegDec.MCUSPerRow);
  Serial.print(F(  "MCU / col  :")); Serial.println(JpegDec.MCUSPerCol);
  Serial.print(F(  "Scan type  :")); Serial.println(JpegDec.scanType);
  Serial.print(F(  "MCU width  :")); Serial.println(JpegDec.MCUWidth);
  Serial.print(F(  "MCU height :")); Serial.println(JpegDec.MCUHeight);
  Serial.println(F("==============="));
}

//====================================================================================
//jpg图片解码显示到屏幕，参数为起始坐标
//====================================================================================
void renderJPEG(int xpos, int ypos) {

  //获取图像尺寸和块尺寸
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth; //块宽度
  uint16_t mcu_h = JpegDec.MCUHeight; //块高度
  uint32_t max_x = JpegDec.width; //图像宽度
  uint32_t max_y = JpegDec.height; //图像高度
  
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w); //最右边不完整块宽度
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h); //最下边不完整块高度

  //屏幕显示位置偏移后最后一个像素的坐标
  max_x += xpos;
  max_y += ypos;

  //当前MCU尺寸
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // 记录时间，用于计算帧率
  uint32_t drawTime = millis();

  // 遍历读取图像块
  while (JpegDec.read()) {
    
    // 当前图像块指针
    pImg = JpegDec.pImage;

    // 计算出在屏幕的显示位置
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    //当前MCU尺寸
    if (mcu_x + mcu_w <= max_x)
        win_w = mcu_w;
    else
        win_w = min_w;
    if(mcu_y + mcu_h <= max_y)
        win_h = mcu_h;
    else
        win_h = min_h;

    //将图像块保存到屏幕缓存
    uint16_t y2 = (JpegDec.MCUy * mcu_h) % 16;
    uint16_t *p1;
    for(uint8_t y = 0; y < win_h; y ++)
    {
        if(mcu_y + y <= LCD_H)
        {
            p1 = (uint16_t*)&lcdCanvas[JpegDec.MCUy%2] + (y * JpegDec.width + mcu_x);
            for(uint8_t x = 0; x < win_w; x ++)
            {
                if(mcu_x + x <= LCD_W){
                  *(p1 + x) = (*pImg >> 8) + (*pImg << 8);
                  pImg ++;
                }
            }
        }
    }

    //缓存满或块接收完毕时将缓存刷新到屏幕
    if((JpegDec.MCUx + 1) * mcu_w >= max_x)//图片解码到了最右边
    {
        //后面要换成DMA模式
        LCD_ShowGram(0, mcu_y, LCD_W, win_h,(uint16_t *)lcdCanvas);//目前只支持全屏显示
        //if((JpegDec.MCUy + 1) * mcu_h >= max_y)
            //JpegDec.abort();
    }
  }

  drawTime = millis() - drawTime; // Calculate the time it took
  //Serial.print  ("Total render time was    : "); Serial.print(drawTime); Serial.println(" ms");
  //Serial.println("=====================================");

}

void setup() {
  
  Serial.begin(115200);

  pinMode(Rheostat_Rx_PIN, INPUT);
  pinMode(Rheostat_Ry_PIN, INPUT);
  pinMode(Rheostat_Lx_PIN, INPUT);
  pinMode(Rheostat_Ly_PIN, INPUT);
  pinMode(ButtonA_PIN, INPUT_PULLUP);
  pinMode(ButtonB_PIN, INPUT_PULLUP);
  pinMode(ButtonC_PIN, INPUT_PULLUP);

  LCD_Init();
  LCD_Fill(0,0,240,240, BLACK);//填充颜色
  //开机动画
  char wc[] = {"WELCOME"};
  for(int n1=0;n1<4;n1++){
    LCD_ShowChar(111-n1*16,102,wc[3-n1],YELLOW,BLACK,32);
    LCD_ShowChar(111+n1*16,102,wc[3+n1],YELLOW,BLACK,32);
    delay(150);
  }
  for(int n1=0;n1<7;n1++){
    LCD_ShowChar(63+n1*16,102,wc[n1],WHITE,BLACK,32);
    delay(80);
  }
  //LCD_ShowString(63,51,"WELCOME",WHITE,BLACK,32);//显示字符串

  WiFi.softAP(ssid, password,5,0,4);//WIFI名，密码，信道（1-13），隐藏ssid，最大连接数
  WiFi.softAPConfig(local_IP, gateway, subnet);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println("Waiting for device to connect...");
  while(WiFi.softAPgetStationNum()==0){//等待有设备连接
    delay(100);
  }
  Serial.println("Connected");
  delay(1000);
  udp.begin(udpPort);
}

void loop() {
  static uint8_t btaValueOld=1,btTime=0;
  static uint8_t fid = 0, fid_old = 0;
  static uint8_t *p1 = jpg_Buff;

  Rheostat_Rx_value = analogRead(Rheostat_Rx_PIN);
  Rheostat_Ry_value = analogRead(Rheostat_Ry_PIN);
  Rheostat_Lx_value = analogRead(Rheostat_Lx_PIN);
  Rheostat_Ly_value = analogRead(Rheostat_Ly_PIN);
  ButtonA_value = digitalRead(ButtonA_PIN);
  ButtonB_value = digitalRead(ButtonB_PIN);
  ButtonC_value = digitalRead(ButtonC_PIN);

  //获取按键状态
  if(ButtonA_value!=btaValueOld){
    btTime ++;
    if(btTime >= 2){
      btaValueOld = ButtonA_value;
      if(!ButtonA_value)
        lampValue+=64;
    }
  }else{
    btTime = 0;
  }

  //读取UDP数据
  int len = udp.parsePacket(); //获取接收到的数据量
  if(len>8)
  {
    //Serial.printf("len = %d\r\n",len);
    len = udp.read((uint8_t *)udp_Rbuff,len);
    uint8_t crc1 = 0;
    for(uint16_t i = 0; i < (len - 1); i++)
      crc1 += udp_Rbuff[i];
    if(crc1 == udp_Rbuff[len - 1]){
      
      fid = udp_Rbuff[0];
      if(fid_old != fid)//换帧
      {
        JpegDec.decodeArray(jpg_Buff, jpg_len);
        //jpegInfo();
        if((JpegDec.width > 0) && (JpegDec.height > 0))
          renderJPEG(0, 0);
        sendDate();
  
        fid_old = fid;
        p1 = jpg_Buff;
        jpg_len = 0;
        jpg_len_old = ((uint32_t)udp_Rbuff[4] << 24) + ((uint32_t)udp_Rbuff[5] << 16) + ((uint32_t)udp_Rbuff[6] << 8) + udp_Rbuff[7];
      }
      for(uint16_t i = 8; i < (len - 1); i ++){
        *p1++ = udp_Rbuff[i];
      }
      jpg_len += (len - 9);
    }   
  }
}
