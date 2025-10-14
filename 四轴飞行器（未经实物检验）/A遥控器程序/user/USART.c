#include "stm32f10x.h"                  // Device header
#include "USART.h"
#include <stdio.h>
/********************************************************************
 *简	介：USART1初始化
 *参	数：波特率
 *返回值：无
 *说	明：无
 *********************************************************************/
void My_USART_Init(uint16_t BaudRate)
{
	GPIO_InitTypeDef GPIO_Initstruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	/*TX【PA9】*/
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_9 ;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);
	/*RX【PA10】*/
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Initstruct.GPIO_Pin =  GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_Initstruct);
	/*USART1*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_InitTypeDef USART_Initstruct;
	USART_Initstruct.USART_BaudRate = BaudRate;
	USART_Initstruct.USART_HardwareFlowControl = DISABLE;
	USART_Initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Initstruct.USART_Parity = USART_Parity_No;
	USART_Initstruct.USART_StopBits = USART_StopBits_1;
	USART_Initstruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_Initstruct);
	USART_Cmd(USART1,ENABLE);
}

/********************************************************************
 *简	介：串口重定向
 *参	数：无
 *返回值：无
 *说	明：无
 *********************************************************************/
int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(uint8_t)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return 0;
}
