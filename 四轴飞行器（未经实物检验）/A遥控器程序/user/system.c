#include "system.h"
#include "stm32f10x.h"                  // Device header

/********************************************************************
 *简	介：系统初始化
 *参	数：无
 *返回值：无
 *说	明：完成所需外设初始化
 *********************************************************************/
void system_Init(void)
{
	/*NVIC分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//中断分组 2
	/*调试用*/
	//oled_Init();                  //0.96oled
	//My_USART_Init(9600);          //串口
	/*外设初始化函数*/
	NRF24L01_Init();              //无线通信
	Rocker_Init();
	Watch_dog_Init();						//初始化看门狗
}
