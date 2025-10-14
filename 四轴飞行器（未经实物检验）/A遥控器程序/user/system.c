#include "system.h"
#include "stm32f10x.h"                  // Device header

/********************************************************************
 *��	�飺ϵͳ��ʼ��
 *��	������
 *����ֵ����
 *˵	����������������ʼ��
 *********************************************************************/
void system_Init(void)
{
	/*NVIC����*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//�жϷ��� 2
	/*������*/
	//oled_Init();                  //0.96oled
	//My_USART_Init(9600);          //����
	/*�����ʼ������*/
	NRF24L01_Init();              //����ͨ��
	Rocker_Init();
	Watch_dog_Init();						//��ʼ�����Ź�
}
