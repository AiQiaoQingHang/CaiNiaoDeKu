#ifndef I2C_H
#define I2C_H
#include "stm32f10x.h"                  																					// Device header
#define I2C_delay 1 																															//���IICͨ�ŵ�ƽ�仯�ӳ�ʱ�䣬ԽС�ٶ�Խ�죬��λ��΢�롿
/*----------------------------------------------- �� �� I I C --------------------------------------------*/
/*--------------------------------------------- �� �� O L E D --------------------------------------------*/																																							
#define SI2C_SCK_Port GPIOB                                                        //SCK����
#define SI2C_SCK_Pin  GPIO_Pin_10                                                   //SCK��Pin��
																																									 
#define SI2C_SDA_Port GPIOB                                                        //SDA����
#define SI2C_SDA_Pin  GPIO_Pin_11                                                   //SDA��Pin��
																																									 
#define SI2C_CLOCK_SCK RCC_APB2Periph_GPIOB                                        //SCK��ʱ��
#define SI2C_CLOCK_SDA RCC_APB2Periph_GPIOB                                        //SDA��ʱ��
/*---------------------------------------------- ʵ �� �� �� ---------------------------------------------*/	
void SI2C_Init(void);                                                            	 //��ʼ�����IIC
void SI2C_SDA(uint8_t value);                                                      //SDAд
void SI2C_SCK(uint8_t value);                                                      //SCKд
void SI2C_Start(void);                                                             //������ʼλ
void SI2C_Stop(void);                                                              //����ֹͣλ
void SI2C_SendByte(uint8_t data);                                                  //����һ�ֽ�
int SI2C_WaitACK(void);                                                            //�ȴ�ACK
void SI2C_SendData(uint8_t *Data,uint8_t size);                                    //��������

#endif
