#include "stm32f10x.h"                  // Device header
#include "I2C.h"
#include "delay.h"

/********************************************************************
 *简	介：软件IIC
 *参	数：无
 *返回值：无
 *说	明：使用时调用，用于oled
 *********************************************************************/
void SI2C_Init(void)
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

/********************************************************************
 *简	介：软件IIC的SDA写
 *参	数：0低电平，1高电平
 *返回值：无
 *说	明：上层函数调用
 *********************************************************************/
void SI2C_SDA(uint8_t value)
{
	GPIO_WriteBit(SI2C_SDA_Port,SI2C_SDA_Pin,(BitAction)value);
}

/********************************************************************
 *简	介：软件IIC的SCK写
 *参	数：0低电平，1高电平
 *返回值：无
 *说	明：上层函数调用
 *********************************************************************/
void SI2C_SCK(uint8_t value)
{
	GPIO_WriteBit(SI2C_SCK_Port,SI2C_SCK_Pin,(BitAction)value);
}

/********************************************************************
 *简	介：软件IIC发送起始位
 *参	数：无
 *返回值：无
 *说	明：上层函数调用
 *********************************************************************/
void SI2C_Start(void)
{
	SI2C_SCK(1);
	SI2C_SDA(1);
	Delay_us(I2C_delay);
	
	SI2C_SDA(0);
	Delay_us(I2C_delay);
}

/********************************************************************
 *简	介：软件IIC停止位发送
 *参	数：无
 *返回值：无
 *说	明：上层函数调用
 *********************************************************************/
void SI2C_Stop(void)
{
	SI2C_SCK(0);
	SI2C_SDA(0);
	Delay_us(I2C_delay);
	
	SI2C_SCK(1);
	Delay_us(I2C_delay);
	SI2C_SDA(1);
	Delay_us(I2C_delay);
}

/********************************************************************
 *简	介：软件IIC发送一字节
 *参	数：一字节数据
 *返回值：无
 *说	明：上层函数调用
 *********************************************************************/
void SI2C_SendByte(uint8_t data)
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
	if(SI2C_WaitACK())  //高电平未应答
			SI2C_Stop();
}

/********************************************************************
 *简	介：软件IIC等待ACK
 *参	数：无
 *返回值：ACK状态，1NACK，0ACK
 *说	明：上层函数调用
 *********************************************************************/
int SI2C_WaitACK(void)
{
	SI2C_SCK(0);
	SI2C_SDA(1);
	Delay_us(I2C_delay);
	
	SI2C_SCK(1);
	Delay_us(I2C_delay);
	return GPIO_ReadInputDataBit(SI2C_SDA_Port,SI2C_SDA_Pin);
}

/********************************************************************
 *简	介：软件IIC发数组
 *参	数：数组地址，数组内元素个数
 *返回值：无
 *说	明：无
 *********************************************************************/
void SI2C_SendData(uint8_t *Data,uint8_t size)
{
	SI2C_Start();
	for(uint8_t i=0;i<size;i++)
	{
		SI2C_SendByte(Data[i]);		
	}
	SI2C_Stop();
}
