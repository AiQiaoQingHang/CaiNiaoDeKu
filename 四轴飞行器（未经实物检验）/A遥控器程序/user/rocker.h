#ifndef ROCKER_H
#define ROCKER_H
#include "stm32f10x.h"                  // Device header

/*
 *ע��ADCͨ�����ã���������Pinһһ��Ӧ
 */

/*����ҡ��*/
#define Throttle_Rocker_RCC						RCC_APB2Periph_GPIOA
#define Throttle_Rocker_Port					GPIOA
#define Throttle_Rocker_Pin						GPIO_Pin_1
#define Throttle_Rocker_ADC						ADC_Channel_1
/*����ҡ�ˡ�X��*/
#define Directional_X_Rocker_RCC			RCC_APB2Periph_GPIOA
#define Directional_X_Rocker_Port     GPIOA
#define Directional_X_Rocker_Pin      GPIO_Pin_2
#define	Directional_X_Rocker_ADC			ADC_Channel_2
/*����ҡ�ˡ�Y��*/
#define Directional_Y_Rocker_RCC			RCC_APB2Periph_GPIOA
#define Directional_Y_Rocker_Port     GPIOA
#define Directional_Y_Rocker_Pin      GPIO_Pin_3
#define Directional_Y_Rocker_ADC			ADC_Channel_3
/*ҡ�˰�ѹ�ж�*/
#define Interrupt_Rocker_RCC					RCC_APB2Periph_GPIOB
#define Interrupt_Rocker_Port     		GPIOB
#define Interrupt_Rocker_Pin      		GPIO_Pin_6
//ע��Ҫһһ��Ӧ���ⲿ�жϺ���Ҳ��Ҫ�޸�
#define Interrupt_Rocker_EXTI_Port								GPIO_PortSourceGPIOB
#define Interrupt_Rocker_EXTI_Pin									GPIO_PinSource6
#define Interrupt_Rocker_EXTI_Lines								EXTI_Line6
#define Interrupt_Rocker_NVIC_Channel							EXTI9_5_IRQn
#define Interrupt_Rocker_NVIC_PreemptionPriority		2
#define Interrupt_Rocker_NVIC_SubPriority					2
/*����,������ԭʼ����*/
#define Dead_Dir			 50.0f       //��������
#define Dead_Thr			 50.0f       //��������
/*���������Ʋ���*/
#define Rocker_Mid	 1950.0f   //ҡ���м�ֵ,ʵ��ҡ�˵ó�

#define Pitch_Max			 20.0f
#define Pitch_Min			-20.0f
#define Pitch_Range		 40.0f  //(Pitch_Max - Pitch_Min)

#define Roll_Max			 20.0f
#define Roll_Min			-20.0f
#define Roll_Range		 40.0f	//(Roll_Max - Roll_Min)

#define Thr_Range			1000.0f	//(Thr_Max - Thr_Min)

#define ADC_Range			4095.0f

typedef struct
{
	uint16_t Thr;                //ԭʼ��������
	uint16_t Dir_X;              //ԭʼ��X����������
	uint16_t Dir_Y;              //ԭʼ��Y����������
}Rocker;

extern volatile Rocker Rocker_Data;				//ҡ������

void Rocker_Init(void);																														//ҡ�˳�ʼ��
void ADC_GPIO_Init(void);                                                         //ADC��GPIO��ʼ��
void Rocker_Timer_Init(void);                                                     //ҡ�˶�ʱ����ʼ��
void Rocker_DMA_Init(void);                                                       //ҡ��DMA��ʼ��
void Rocker_Int_Init(void);																												//��ʼ��ҡ���ж�
void EXTI9_5_IRQHandler(void);																										//�ⲿ�жϺ���

#endif
