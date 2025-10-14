#ifndef SYSTEM_H
#define SYSTEM_H
/*调试用*/
#include "I2C.h"                 //软件IIC用于oled
#include "oled.h"                //0.96oled屏幕
#include "USART.h"               //串口
#include <stdio.h>							 //串口printf
/*系统*/
#include "NRF24L01.h"            //无线通信
#include "Delay.h"               //延迟函数，嘀嗒定时器
#include "rocker.h"							 //摇杆
#include "2.4g_protocol.h"			 //2.4g协议（人为规定）
#include "watch_dog.h"					 //看门狗
void system_Init(void);
#endif
