#include "stm32f10x.h"                  // Device header
#include "I2C.h"
#include "delay.h"

/********************************************************************
 *��	�飺���IIC
 *��	������
 *����ֵ����
 *˵	����ʹ��ʱ���ã�����oled
 *********************************************************************/
void SI2C_Init(void)
{
	GPIO_InitTypeDef GPIO_Initstruct;
	//SDA���ų�ʼ��
	RCC_APB2PeriphClockCmd(SI2C_CLOCK_SDA,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Initstruct.GPIO_Pin = SI2C_SDA_Pin;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SI2C_SDA_Port,&GPIO_Initstruct);
	//SCK���ų�ʼ��
	RCC_APB2PeriphClockCmd(SI2C_CLOCK_SCK,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Initstruct.GPIO_Pin = SI2C_SCK_Pin;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SI2C_SCK_Port,&GPIO_Initstruct);
}

/********************************************************************
 *��	�飺���IIC��SDAд
 *��	����0�͵�ƽ��1�ߵ�ƽ
 *����ֵ����
 *˵	�����ϲ㺯������
 *********************************************************************/
void SI2C_SDA(uint8_t value)
{
	GPIO_WriteBit(SI2C_SDA_Port,SI2C_SDA_Pin,(BitAction)value);
}

/********************************************************************
 *��	�飺���IIC��SCKд
 *��	����0�͵�ƽ��1�ߵ�ƽ
 *����ֵ����
 *˵	�����ϲ㺯������
 *********************************************************************/
void SI2C_SCK(uint8_t value)
{
	GPIO_WriteBit(SI2C_SCK_Port,SI2C_SCK_Pin,(BitAction)value);
}

/********************************************************************
 *��	�飺���IIC������ʼλ
 *��	������
 *����ֵ����
 *˵	�����ϲ㺯������
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
 *��	�飺���IICֹͣλ����
 *��	������
 *����ֵ����
 *˵	�����ϲ㺯������
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
 *��	�飺���IIC����һ�ֽ�
 *��	����һ�ֽ�����
 *����ֵ����
 *˵	�����ϲ㺯������
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
	if(SI2C_WaitACK())  //�ߵ�ƽδӦ��
			SI2C_Stop();
}

/********************************************************************
 *��	�飺���IIC�ȴ�ACK
 *��	������
 *����ֵ��ACK״̬��1NACK��0ACK
 *˵	�����ϲ㺯������
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
 *��	�飺���IIC������
 *��	���������ַ��������Ԫ�ظ���
 *����ֵ����
 *˵	������
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
