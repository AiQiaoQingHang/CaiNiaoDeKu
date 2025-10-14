#include "stm32f10x.h"                  // Device header
#include "watch_dog.h"

/********************************************************************
 *��	�飺���Ź���ʼ��
 *��	������
 *����ֵ����
 *˵	������
 *********************************************************************/
void Watch_dog_Init(void)
{
	/*�����Ĵ�����д*/
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	/*����Ԥ��Ƶ*/
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	/*����Countֵ*/
	IWDG_SetReload(Dog_time);
	/*ʹ�ܿ��Ź�*/
	IWDG_Enable();
}
