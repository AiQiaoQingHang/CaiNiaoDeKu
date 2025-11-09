#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "oled.h"
#include "OLED_USER.h"

static bool SI2C_Start(void);
static bool SI2C_Stop(void);
static bool SI2C_SendByte(uint8_t data);
static int SI2C_WaitACK(void);
static void SI2C_Init(void);

OLED_Driver_t OLED_DRV = {
    .Delay_us = Delay_us,
    .IIC_Generate_Start = SI2C_Start,
    .IIC_Generate_Stop = SI2C_Stop,
    .IIC_Send_Data = SI2C_SendByte,
    .oled_adress = 0x78,
};

void USER_OLED_Init()
{
    SI2C_Init();
    oled_Init(&OLED_DRV); // 0.96oled
};

static void SI2C_Init(void)
{
	GPIO_InitTypeDef GPIO_Initstruct;
	//SDA引脚初始化
	RCC_APB2PeriphClockCmd(SI2C_CLOCK_SDA,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Initstruct.GPIO_Pin = SI2C_SDA_Pin;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SI2C_SDA_Port,&GPIO_Initstruct);
	//SCK引脚初始化
	RCC_APB2PeriphClockCmd(SI2C_CLOCK_SCK,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Initstruct.GPIO_Pin = SI2C_SCK_Pin;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SI2C_SCK_Port,&GPIO_Initstruct);
}

static void SI2C_SDA(uint8_t value)
{
	GPIO_WriteBit(SI2C_SDA_Port,SI2C_SDA_Pin,(BitAction)value);
}

static void SI2C_SCK(uint8_t value)
{
	GPIO_WriteBit(SI2C_SCK_Port,SI2C_SCK_Pin,(BitAction)value);
}

static bool SI2C_Start(void)
{
	SI2C_SCK(1);
	SI2C_SDA(1);
	Delay_us(I2C_delay);
	
	SI2C_SDA(0);
	Delay_us(I2C_delay);
    return true;
}

static bool SI2C_Stop(void)
{
	SI2C_SCK(0);
	SI2C_SDA(0);
	Delay_us(I2C_delay);
	
	SI2C_SCK(1);
	Delay_us(I2C_delay);
	SI2C_SDA(1);
	Delay_us(I2C_delay);
    return true;
}

static bool SI2C_SendByte(uint8_t data)
{
	for(uint8_t i=0;i<8;i++)
	{
		SI2C_SCK(0);
		if( (   (data>>(7-i)   )&0x01) == 0 )
			SI2C_SDA(0);
		else
			SI2C_SDA(1);
		Delay_us(I2C_delay);
		SI2C_SCK(1);
	}
    SI2C_SCK(0); // 拉低SCK，准备接收ACK
    SI2C_SDA(1); // 释放SDA（开漏输出，OLED可拉低）
    Delay_us(I2C_delay);
	if(SI2C_WaitACK())  //高电平未应答
    {
        SI2C_Stop();
        return false;
    }
return true;
}

// static int SI2C_WaitACK(void)
// {
// 	SI2C_SCK(0);
// 	SI2C_SDA(1);
// 	Delay_us(I2C_delay);
	
// 	SI2C_SCK(1);
// 	Delay_us(I2C_delay);
// 	return GPIO_ReadInputDataBit(SI2C_SDA_Port,SI2C_SDA_Pin);
// }

static int SI2C_WaitACK(void)
{
    uint8_t ack = 1; // 默认NACK
    SI2C_SDA(1); // 释放SDA
    Delay_us(I2C_delay);
    
    SI2C_SCK(1); // 高电平：读取ACK
    Delay_us(I2C_delay);
    ack = GPIO_ReadInputDataBit(SI2C_SDA_Port,SI2C_SDA_Pin); // 0=ACK，1=NACK
    
    SI2C_SCK(0); // 关键：拉低SCK，结束ACK读取
    Delay_us(I2C_delay);
    return ack;
}

// static void SI2C_SendData(uint8_t *Data,uint8_t size)
// {
// 	SI2C_Start();
// 	for(uint8_t i=0;i<size;i++)
// 	{
// 		SI2C_SendByte(Data[i]);		
// 	}
// 	SI2C_Stop();
// }
