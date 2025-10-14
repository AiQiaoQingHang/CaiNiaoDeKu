#include "stm32f10x.h"                  // Device header
#include "rocker.h"
#include "2.4g_protocol.h"

volatile Rocker Rocker_Data;	 										//ҡ��ԭʼ�������ݣ��ɡ���ʱ��-ADC-DMA���Զ�����

/********************************************************************
 *��	�飺ҡ��ADC�ɼ���ʼ��
 *��	������
 *����ֵ����
 *˵	������
 *********************************************************************/
void Rocker_Init(void)
{
	ADC_GPIO_Init();				//��ʼ��GPIO
	Rocker_Int_Init();
	ADC_InitTypeDef ADC_Initstruct;
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);			//ADCʱ�Ӳ��ɳ���14Mhz������Ƶ12Mhz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	ADC_Initstruct.ADC_ContinuousConvMode = DISABLE;
	ADC_Initstruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_Initstruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_Initstruct.ADC_Mode = ADC_Mode_Independent;
	ADC_Initstruct.ADC_NbrOfChannel = 3;
	ADC_Initstruct.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &ADC_Initstruct);
	//ADC_Cmd(ADC1,ENABLE);
	
	ADC_RegularChannelConfig(ADC1,Throttle_Rocker_ADC			,1,ADC_SampleTime_13Cycles5);		//����ADC
	ADC_RegularChannelConfig(ADC1,Directional_X_Rocker_ADC,2,ADC_SampleTime_13Cycles5);		//����X��ADC
	ADC_RegularChannelConfig(ADC1,Directional_Y_Rocker_ADC,3,ADC_SampleTime_13Cycles5);		//����Y��ADC
	
	ADC_ExternalTrigConvCmd(ADC1,ENABLE);		//ʹ��ADC�ⲿ����,��ʱ��3����¼�
	/*У׼ADC*/
	uint16_t Timeout=3000;			//��ʱ����
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1))
	{
		if(Timeout==0)
			break;
		Timeout--;
	}
	
	Timeout=3000;								//���ó�ʱ����
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1))
	{
		if(Timeout==0)
			break;
		Timeout--;
	}
	ADC_DMACmd(ADC1,ENABLE);		//ʹ��ADC��DMA����	
	Rocker_Timer_Init();				//��ʼ����ʱ��
	Rocker_DMA_Init();					//��ʼ��DMA
	ADC_Cmd(ADC1,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
	DMA_Cmd(DMA1_Channel1,ENABLE);	
}

/********************************************************************
 *��	�飺��ʼ��ADC��GPIO
 *��	������
 *����ֵ����
 *˵	�������ϲ㺯������
 *********************************************************************/
void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_Initstruct;	
	/*����ҡ��*/
	RCC_APB2PeriphClockCmd(Throttle_Rocker_RCC,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Initstruct.GPIO_Pin = Throttle_Rocker_Pin;
	GPIO_Init(Throttle_Rocker_Port,&GPIO_Initstruct);
	/*����ҡ�ˡ�X��*/
	RCC_APB2PeriphClockCmd(Directional_X_Rocker_RCC,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Initstruct.GPIO_Pin = Directional_X_Rocker_Pin;
	GPIO_Init(Directional_X_Rocker_Port,&GPIO_Initstruct);
	/*����ҡ�ˡ�Y��*/
	RCC_APB2PeriphClockCmd(Directional_Y_Rocker_RCC,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Initstruct.GPIO_Pin = Directional_Y_Rocker_Pin;
	GPIO_Init(Directional_Y_Rocker_Port,&GPIO_Initstruct);
}

/********************************************************************
 *��	�飺��ʼ��ADC�Ķ�ʱ��
 *��	������
 *����ֵ����
 *˵	�������ϲ㺯������
 *********************************************************************/
void Rocker_Timer_Init(void)
{
	TIM_TimeBaseInitTypeDef Timer_Initstruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	Timer_Initstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	Timer_Initstruct.TIM_CounterMode = TIM_CounterMode_Up;
	Timer_Initstruct.TIM_Prescaler = (7200-1);
	Timer_Initstruct.TIM_Period = (100-1);									//10ms����һ��
	TIM_TimeBaseInit(TIM3,&Timer_Initstruct);
	
	TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);					//�����¼�����
	TIM_Cmd(TIM3,DISABLE);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
}

/********************************************************************
 *��	�飺��ʼ��ҡ���ж�
 *��	������
 *����ֵ����
 *˵	�������ϲ㺯�����ã���Ҫ��ȡ��׽ҡ�˰���ʱ�ĵ�ƽ�仯
 *********************************************************************/
void Rocker_Int_Init(void)
{
	RCC_APB2PeriphClockCmd(Interrupt_Rocker_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	/*���ų�ʼ��*/
	GPIO_InitTypeDef GPIO_Initstruct;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Initstruct.GPIO_Pin = Interrupt_Rocker_Pin;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(Interrupt_Rocker_Port,&GPIO_Initstruct);
	/*EXTI��ʼ��*/
	GPIO_EXTILineConfig(Interrupt_Rocker_EXTI_Port,Interrupt_Rocker_EXTI_Pin);			//ӳ��
	EXTI_InitTypeDef EXTI_Initstruct;
	EXTI_Initstruct.EXTI_Line = Interrupt_Rocker_EXTI_Lines;
	EXTI_Initstruct.EXTI_LineCmd = ENABLE;
	EXTI_Initstruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_Initstruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_Initstruct);
	/*NVIC��ʼ��*/
	NVIC_InitTypeDef NVIC_Initstruct;
	NVIC_Initstruct.NVIC_IRQChannel = Interrupt_Rocker_NVIC_Channel;
	NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = Interrupt_Rocker_NVIC_PreemptionPriority;
	NVIC_Initstruct.NVIC_IRQChannelSubPriority = Interrupt_Rocker_NVIC_SubPriority;
	NVIC_Init(&NVIC_Initstruct);
}
/********************************************************************
 *��	�飺��ʼ��ҡ��DMA
 *��	������
 *����ֵ����
 *˵	�������ϲ㺯������
 *********************************************************************/
void Rocker_DMA_Init(void)
{
	DMA_InitTypeDef DMA_Initstruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_Initstruct.DMA_BufferSize = 3;                                     			//3��
	DMA_Initstruct.DMA_DIR = DMA_DIR_PeripheralSRC;                        			//��������Դ
	DMA_Initstruct.DMA_M2M = DMA_M2M_Disable;                              			//�ر��ڴ�->�ڴ�
	DMA_Initstruct.DMA_Mode	= DMA_Mode_Normal;                           	 			//����ģʽ
	DMA_Initstruct.DMA_Priority	= DMA_Priority_High;                       			//�����ȼ�
			
	DMA_Initstruct.DMA_MemoryBaseAddr = (uint32_t)(&Rocker_Data);          			//�ڴ��ַ
	DMA_Initstruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;       			//16λ����
	DMA_Initstruct.DMA_MemoryInc = DMA_MemoryInc_Enable;                   			//�����ڴ�����
			
	DMA_Initstruct.DMA_PeripheralBaseAddr = (uint32_t)(&ADC1->DR);          		//�����ַ
	DMA_Initstruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 		//16λ����
	DMA_Initstruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              	//�����ַ������

	DMA_Init(DMA1_Channel1,&DMA_Initstruct);
	
	NVIC_InitTypeDef NVIC_Initstruct;
	NVIC_Initstruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Initstruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_Initstruct);
	
	DMA_Cmd(DMA1_Channel1,DISABLE);																				 //�ر�DMAͨ��
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);													 //ʹ�ܴ�������ж�
}

/********************************************************************
 *��	�飺DMA1ͨ��1�жϺ���
 *��	������
 *����ֵ����
 *˵	������������DMA
 *********************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);									//����жϱ�־λ		
		DMA_Cmd(DMA1_Channel1,DISABLE);											//ȷ��DMAʧ��
		DMA_SetCurrDataCounter(DMA1_Channel1,3);						//��������DMA
		DMA_Cmd(DMA1_Channel1,ENABLE);											//����ʹ��DMA
	}
}

/********************************************************************
 *��	�飺�ⲿ�жϺ���
 *��	������
 *����ֵ����
 *˵	������׽ҡ�˰�ѹ��ƽ�ź�
 *********************************************************************/
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(Interrupt_Rocker_EXTI_Lines) != RESET)
	{
		EXTI_ClearITPendingBit(Interrupt_Rocker_EXTI_Lines);
		Plane_Lock_FLAG = 1;		//�����ɻ�
	}
}
