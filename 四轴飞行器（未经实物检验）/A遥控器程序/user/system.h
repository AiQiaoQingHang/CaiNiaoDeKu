#ifndef SYSTEM_H
#define SYSTEM_H
/*������*/
#include "I2C.h"                 //���IIC����oled
#include "oled.h"                //0.96oled��Ļ
#include "USART.h"               //����
#include <stdio.h>							 //����printf
/*ϵͳ*/
#include "NRF24L01.h"            //����ͨ��
#include "Delay.h"               //�ӳٺ�������શ�ʱ��
#include "rocker.h"							 //ҡ��
#include "2.4g_protocol.h"			 //2.4gЭ�飨��Ϊ�涨��
#include "watch_dog.h"					 //���Ź�
void system_Init(void);
#endif
