/*
BILIBILI:技术宅物语
*/
#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include <Arduino.h>
#include <SPI.h>

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned long   u32;

//����
#define USE_HORIZONTAL 0  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����
#define LCD_W 240
#define LCD_H 240

//SPI

#define MOSI_PIN 11
#define SCLK_PIN 12
#define DC_PIN 9
#define REST_PIN 13
#define BLK_PIN 14

#define  LCD_DC_Set()    digitalWrite(DC_PIN, HIGH)
#define LCD_MOSI_Set()    digitalWrite(MOSI_PIN, HIGH)
#define LCD_SCLK_Set()    digitalWrite(SCLK_PIN, HIGH)
#define LCD_RES_Set()   digitalWrite(REST_PIN, HIGH)

#define LCD_DC_Clr()    digitalWrite(DC_PIN, LOW)
#define LCD_MOSI_Clr()    digitalWrite(MOSI_PIN, LOW)
#define LCD_SCLK_Clr()    digitalWrite(SCLK_PIN, LOW)
#define LCD_RES_Clr()   digitalWrite(REST_PIN, LOW)

#define LCD_BLK_Clr()  digitalWrite(BLK_PIN, LOW)
#define LCD_BLK_Set()  digitalWrite(BLK_PIN, HIGH)

void LCD_Writ_Bus(u8 dat);
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA(u16 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//�������꺯��
void LCD_Init(void);//LCD��ʼ��
void LCD_ShowGram(u16 x,u16 y,u16 length,u16 width,u16 *pic16);


#endif
