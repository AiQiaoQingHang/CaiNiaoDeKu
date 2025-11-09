#ifndef OLED_USER_H
#define OLED_USER_H
#include "oled.h"
#include <stdint.h>

extern OLED_Driver_t OLED_DRV;
#define I2C_delay 1 																															//软件IIC通信电平变化延迟时间，越小速度越快，单位【微秒】
/*----------------------------------------------- 软 件 I I C --------------------------------------------*/
/*--------------------------------------------- 用 于 O L E D --------------------------------------------*/																																							
#define SI2C_SCK_Port GPIOB                                                        //SCK引脚
#define SI2C_SCK_Pin  GPIO_Pin_10                                                   //SCK的Pin脚
																																									 
#define SI2C_SDA_Port GPIOB                                                        //SDA引脚
#define SI2C_SDA_Pin  GPIO_Pin_11                                                   //SDA的Pin脚
																																									 
#define SI2C_CLOCK_SCK RCC_APB2Periph_GPIOB                                        //SCK的时钟
#define SI2C_CLOCK_SDA RCC_APB2Periph_GPIOB                                        //SDA的时钟
/*---------------------------------------------- 实 现 函 数 ---------------------------------------------*/
void USER_OLED_Init();
#endif