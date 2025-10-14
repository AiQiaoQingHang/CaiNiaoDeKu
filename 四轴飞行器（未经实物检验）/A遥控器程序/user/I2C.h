#ifndef I2C_H
#define I2C_H
#include "stm32f10x.h"                  																					// Device header
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
void SI2C_Init(void);                                                            	 //初始化软件IIC
void SI2C_SDA(uint8_t value);                                                      //SDA写
void SI2C_SCK(uint8_t value);                                                      //SCK写
void SI2C_Start(void);                                                             //发送起始位
void SI2C_Stop(void);                                                              //发送停止位
void SI2C_SendByte(uint8_t data);                                                  //发送一字节
int SI2C_WaitACK(void);                                                            //等待ACK
void SI2C_SendData(uint8_t *Data,uint8_t size);                                    //发送数组

#endif
