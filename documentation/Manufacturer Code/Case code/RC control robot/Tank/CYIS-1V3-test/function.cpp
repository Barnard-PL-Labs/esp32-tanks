#include <Bridge.h>
#include <BridgeClient.h>
#include <BridgeServer.h>
#include <BridgeSSLClient.h>
#include <BridgeUdp.h>
#include <Console.h>
#include <FileIO.h>
#include <HttpClient.h>
#include <Mailbox.h>
#include <Process.h>
#include <YunClient.h>
#include <YunServer.h>

/*
BILIBILI:技术宅物语
*/
#include "esp_timer.h"
#include "esp_camera.h"
#include "Arduino.h"
#include <WiFiUdp.h>
#include "function.h"
#include "config.h"
#include "stdlib.h"

extern String myPassword;

extern "C"
{
uint8_t temprature_sens_read();
}
uint8_t temprature_sens_read();

SemaphoreHandle_t  camera_state; //图像获取完成信号量
SemaphoreHandle_t  sendJpg_state; //图像发送完成信号量
camera_fb_t * fb = NULL; //图像缓存

WiFiUDP udp;
char udpAddress[20]={"192.168.1.1"};
char localAddress[20]={0};
const uint16_t udpPort = 10000;


// 照明灯初始化
void headlampInit(void)
{
    ledcSetup(Headlamp_Ch,10000,8);  //设置LEDC通道8频率为10000，分辨率为8位
    ledcAttachPin(Headlamp_Pin, Headlamp_Ch); //设置LEDC通道8在Headlamp_Pin上输出
}
// 照明灯设置
void headlampSet(int bri)
{
    ledcWrite(Headlamp_Ch, bri); //输出PWM 
}

// 舵机初始化
void servoInit(void)
{
    ledcSetup(Servo_Ch,50,14);  //设置LEDC通道8频率为50，分辨率为14位
    ledcAttachPin(Servo_Pin, Servo_Ch); //设置LEDC通道8在Headlamp_Pin上输出
}
// 舵机设置
void servoSet(float angle)
{
    if((angle>=0) && (angle<=180))
        ledcWrite(Servo_Ch, (int)(410+9.1*angle)); //输出PWM
}

// 电机初始化
void motorInit(void)
{
    ledcSetup(MotorL_A_Ch,10000,8);
    ledcAttachPin(MotorL_A_Pin, MotorL_A_Ch); 
    ledcSetup(MotorR_A_Ch,10000,8);
    ledcAttachPin(MotorR_A_Pin, MotorR_A_Ch); 
    ledcSetup(MotorL_B_Ch,10000,8);
    ledcAttachPin(MotorL_B_Pin, MotorL_B_Ch); 
    ledcSetup(MotorR_B_Ch,10000,8);
    ledcAttachPin(MotorR_B_Pin, MotorR_B_Ch); 
    
}
// 电机设置
void motorSet(uint8_t motor, int spe)
{
    if(motor == MOTOR_L)
    {
        if(spe>=0)
        { 
            ledcWrite(MotorL_A_Ch, spe); //输出PWM
            ledcWrite(MotorL_B_Ch, 0); //输出PWM
        }
        else
        {
            ledcWrite(MotorL_A_Ch, 0); //输出PWM
            ledcWrite(MotorL_B_Ch, 0-spe); //输出PWM
        }
    }
    else if(motor == MOTOR_R)
    {
        if(spe>=0)
        {
            ledcWrite(MotorR_A_Ch, spe); //输出PWM  
            ledcWrite(MotorR_B_Ch, 0); //输出PWM
        }
        else
        {
            ledcWrite(MotorR_A_Ch, 0); //输出PWM  
            ledcWrite(MotorR_B_Ch, 0-spe); //输出PWM
        }
    }
}

// 发送图片数据
void sendVideoDate(uint8_t* frame, size_t len, size_t frameCount)
{
    uint8_t tx_buffer[1000] = {0};
    uint8_t *ps = frame;
    uint8_t crc = 0;
    uint8_t pCount = len / 960;
    
    if(len % 960)
      pCount += 1;
      
    for(uint8_t *p1 = frame; p1 < frame + len; p1++)
      crc += *p1;
      
    for(int ln=0;ln<pCount;ln++)
    {
        tx_buffer[0] = frameCount;
        tx_buffer[1] = ln;
        tx_buffer[2] = pCount;
        tx_buffer[3] = crc;
        tx_buffer[4] = len >> 24;
        tx_buffer[5] = (len >> 16) & 0xff;
        tx_buffer[6] = (len >> 8) & 0xff;
        tx_buffer[7] = len & 0xff;
        uint16_t px = 960 + 8;
        if(ln == (pCount - 1))
          px = len % 960 + 8;
        for(uint8_t *pb=&tx_buffer[8];pb<&tx_buffer[px];pb++)
        {
            *pb = *ps++;
        }
        //加校验
        tx_buffer[px] = 0;
        for(uint8_t *pb=&tx_buffer[0];pb<&tx_buffer[px];pb++)
            tx_buffer[px] += *pb;
        //发送
        udp.beginPacket(udpAddress,udpPort);
        udp.write(tx_buffer, px + 1);
        udp.endPacket();
        delay(1);
    }
}

//获取图像
int capture_handler(void){
    esp_err_t res = ESP_OK; //结果

    xSemaphoreTake(sendJpg_state, portMAX_DELAY);
    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return ESP_FAIL;
    }
    xSemaphoreGive(camera_state);
    esp_camera_fb_return(fb);

    return res;
}

// 通信服务
void Communication( void * parameter) {
    char udp_Rbuff[UDP_RBUFF_SIZE]; //UDP接收缓存
    uint8_t id=0; //图像帧号
    int rx_time = 0;
    BaseType_t xReturn;//图像数据是否准备就绪
    camera_fb_t * fbSend;
    
    udp.begin(udpPort);
    xSemaphoreGive(sendJpg_state);//通知获取新图像
    while(1)
    {
        // 图片传输
        xReturn = xSemaphoreTake(camera_state, 1);//查询图像数据，阻塞1ms
        if(xReturn == pdTRUE)
        {
            fbSend = fb;
            int len = fbSend->len;
            if(len > 500)
            {
                sendVideoDate((uint8_t *)fbSend->buf, len, ++id);//将图像发送出去
                //Serial.printf("Send complete! Jpg_Len = %u,id=%u\r\n",len,id);
                delay(100);
            }
            xSemaphoreGive(sendJpg_state);//通知获取新图像
        }

        // 接收/运行指令
        int len = udp.parsePacket(); //获取接收到的数据量
        if(len>0)
        {
            
            //Serial.printf("udp_Rbuff len = %d\r\n",len);
            len = udp.read(udp_Rbuff,UDP_RBUFF_SIZE-1);
            Serial.printf("B0:%d, B1:%d, B2:%d, B3:%d, B4:%d\r\n",udp_Rbuff[0],udp_Rbuff[1],udp_Rbuff[2],udp_Rbuff[3],udp_Rbuff[4]);
            //Serial.write((uint8_t *)udp_Rbuff,len);
            //Serial.println(" ");
            //校验接收到的数据
            if((len==6)&&((uint8_t)(udp_Rbuff[0]+udp_Rbuff[1]+udp_Rbuff[2]+udp_Rbuff[3]+udp_Rbuff[4])==udp_Rbuff[5]))
            {
                int lx=0,ly=0,ry=0,lp=0;
                if(udp_Rbuff[2]>127)
                    lx = (udp_Rbuff[2]-127)*2;
                else if(udp_Rbuff[2]<127)
                    lx = 0-(127-udp_Rbuff[2])*2;

                if(udp_Rbuff[3]>127)
                    ly = (udp_Rbuff[3]-127)*2;
                else if(udp_Rbuff[3]<127)
                    ly = 0-(127-udp_Rbuff[3])*2;

                lp = udp_Rbuff[4];
                headlampSet(lp);
                
                if(abs(lx)>abs(ly))
                {
                    motorSet(MOTOR_L, 0-lx);
                    motorSet(MOTOR_R, lx);
                }
                else
                {
                    motorSet(MOTOR_L, ly);
                    motorSet(MOTOR_R, ly);
                }

                ry = udp_Rbuff[1];
                servoSet(ry/1.42);
                
                rx_time = 0;
            }
        }
        else
        {
            if(rx_time<70000)
                rx_time ++; 
        }  
        if(rx_time == 60000) //长时间未接收到数据停止摄像头运行并进入低功耗模式
        {
            Serial.println("Disconnect!");
        }
        else if(rx_time == 500) //长时间未接收到数据停止外设运行
        {
            headlampSet(0);
            motorSet(MOTOR_L, 0);
            motorSet(MOTOR_R, 0);  
        }
    }
}
