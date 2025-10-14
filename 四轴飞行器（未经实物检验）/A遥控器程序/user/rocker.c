#include "stm32f10x.h"                  // Device header
#include "rocker.h"
#include "2.4g_protocol.h"

volatile Rocker Rocker_Data;	 										//摇杆原始数据数据，由【定时器-ADC-DMA】自动更新

/********************************************************************
 *简	介：摇杆ADC采集初始化
 *参	数：无
 *返回值：无
 *说	明：无
 *********************************************************************/
void Rocker_Init(void)
{
	ADC_GPIO_Init();				//初始化GPIO
	Rocker_Int_Init();
	ADC_InitTypeDef ADC_Initstruct;
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);			//ADC时钟不可超过14Mhz，六分频12Mhz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	ADC_Initstruct.ADC_ContinuousConvMode = DISABLE;
	ADC_Initstruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_Initstruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_Initstruct.ADC_Mode = ADC_Mode_Independent;
	ADC_Initstruct.ADC_NbrOfChannel = 3;
	ADC_Initstruct.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &ADC_Initstruct);
	//ADC_Cmd(ADC1,ENABLE);
	
	ADC_RegularChannelConfig(ADC1,Throttle_Rocker_ADC			,1,ADC_SampleTime_13Cycles5);		//油门ADC
	ADC_RegularChannelConfig(ADC1,Directional_X_Rocker_ADC,2,ADC_SampleTime_13Cycles5);		//方向【X】ADC
	ADC_RegularChannelConfig(ADC1,Directional_Y_Rocker_ADC,3,ADC_SampleTime_13Cycles5);		//方向【Y】ADC
	
	ADC_ExternalTrigConvCmd(ADC1,ENABLE);		//使能ADC外部触发,定时器3溢出事件
	/*校准ADC*/
	uint16_t Timeout=3000;			//超时变量
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1))
	{
		if(Timeout==0)
			break;
		Timeout--;
	}
	
	Timeout=3000;								//重置超时变量
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1))
	{
		if(Timeout==0)
			break;
		Timeout--;
	}
	ADC_DMACmd(ADC1,ENABLE);		//使能ADC的DMA请求	
	Rocker_Timer_Init();				//初始化定时器
	Rocker_DMA_Init();					//初始化DMA
	ADC_Cmd(ADC1,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
	DMA_Cmd(DMA1_Channel1,ENABLE);	
}

/********************************************************************
 *简	介：初始化ADC的GPIO
 *参	数：无
 *返回值：无
 *说	明：供上层函数调用
 *********************************************************************/
void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_Initstruct;	
	/*油门摇杆*/
	RCC_APB2PeriphClockCmd(Throttle_Rocker_RCC,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Initstruct.GPIO_Pin = Throttle_Rocker_Pin;
	GPIO_Init(Throttle_Rocker_Port,&GPIO_Initstruct);
	/*方向摇杆【X】*/
	RCC_APB2PeriphClockCmd(Directional_X_Rocker_RCC,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Initstruct.GPIO_Pin = Directional_X_Rocker_Pin;
	GPIO_Init(Directional_X_Rocker_Port,&GPIO_Initstruct);
	/*方向摇杆【Y】*/
	RCC_APB2PeriphClockCmd(Directional_Y_Rocker_RCC,ENABLE);
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Initstruct.GPIO_Pin = Directional_Y_Rocker_Pin;
	GPIO_Init(Directional_Y_Rocker_Port,&GPIO_Initstruct);
}

/********************************************************************
 *简	介：初始化ADC的定时器
 *参	数：无
 *返回值：无
 *说	明：供上层函数调用
 *********************************************************************/
void Rocker_Timer_Init(void)
{
	TIM_TimeBaseInitTypeDef Timer_Initstruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	Timer_Initstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	Timer_Initstruct.TIM_CounterMode = TIM_CounterMode_Up;
	Timer_Initstruct.TIM_Prescaler = (7200-1);
	Timer_Initstruct.TIM_Period = (100-1);									//10ms触发一次
	TIM_TimeBaseInit(TIM3,&Timer_Initstruct);
	
	TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);					//更新事件触发
	TIM_Cmd(TIM3,DISABLE);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
}

/********************************************************************
 *简	介：初始化摇杆中断
 *参	数：无
 *返回值：无
 *说	明：供上层函数调用，主要获取捕捉摇杆按下时的电平变化
 *********************************************************************/
void Rocker_Int_Init(void)
{
	RCC_APB2PeriphClockCmd(Interrupt_Rocker_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	/*引脚初始化*/
	GPIO_InitTypeDef GPIO_Initstruct;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Initstruct.GPIO_Pin = Interrupt_Rocker_Pin;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(Interrupt_Rocker_Port,&GPIO_Initstruct);
	/*EXTI初始化*/
	GPIO_EXTILineConfig(Interrupt_Rocker_EXTI_Port,Interrupt_Rocker_EXTI_Pin);			//映射
	EXTI_InitTypeDef EXTI_Initstruct;
	EXTI_Initstruct.EXTI_Line = Interrupt_Rocker_EXTI_Lines;
	EXTI_Initstruct.EXTI_LineCmd = ENABLE;
	EXTI_Initstruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_Initstruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_Initstruct);
	/*NVIC初始化*/
	NVIC_InitTypeDef NVIC_Initstruct;
	NVIC_Initstruct.NVIC_IRQChannel = Interrupt_Rocker_NVIC_Channel;
	NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = Interrupt_Rocker_NVIC_PreemptionPriority;
	NVIC_Initstruct.NVIC_IRQChannelSubPriority = Interrupt_Rocker_NVIC_SubPriority;
	NVIC_Init(&NVIC_Initstruct);
}
/********************************************************************
 *简	介：初始化摇杆DMA
 *参	数：无
 *返回值：无
 *说	明：供上层函数调用
 *********************************************************************/
void Rocker_DMA_Init(void)
{
	DMA_InitTypeDef DMA_Initstruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_Initstruct.DMA_BufferSize = 3;                                     			//3次
	DMA_Initstruct.DMA_DIR = DMA_DIR_PeripheralSRC;                        			//外设是来源
	DMA_Initstruct.DMA_M2M = DMA_M2M_Disable;                              			//关闭内存->内存
	DMA_Initstruct.DMA_Mode	= DMA_Mode_Normal;                           	 			//单次模式
	DMA_Initstruct.DMA_Priority	= DMA_Priority_High;                       			//高优先级
			
	DMA_Initstruct.DMA_MemoryBaseAddr = (uint32_t)(&Rocker_Data);          			//内存地址
	DMA_Initstruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;       			//16位数据
	DMA_Initstruct.DMA_MemoryInc = DMA_MemoryInc_Enable;                   			//开启内存自增
			
	DMA_Initstruct.DMA_PeripheralBaseAddr = (uint32_t)(&ADC1->DR);          		//外设地址
	DMA_Initstruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 		//16位数据
	DMA_Initstruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              	//外设地址不自增

	DMA_Init(DMA1_Channel1,&DMA_Initstruct);
	
	NVIC_InitTypeDef NVIC_Initstruct;
	NVIC_Initstruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Initstruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_Initstruct);
	
	DMA_Cmd(DMA1_Channel1,DISABLE);																				 //关闭DMA通道
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);													 //使能传输完成中断
}

/********************************************************************
 *简	介：DMA1通道1中断函数
 *参	数：无
 *返回值：无
 *说	明：重新配置DMA
 *********************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);									//清除中断标志位		
		DMA_Cmd(DMA1_Channel1,DISABLE);											//确保DMA失能
		DMA_SetCurrDataCounter(DMA1_Channel1,3);						//重新配置DMA
		DMA_Cmd(DMA1_Channel1,ENABLE);											//重新使能DMA
	}
}

/********************************************************************
 *简	介：外部中断函数
 *参	数：无
 *返回值：无
 *说	明：捕捉摇杆按压电平信号
 *********************************************************************/
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(Interrupt_Rocker_EXTI_Lines) != RESET)
	{
		EXTI_ClearITPendingBit(Interrupt_Rocker_EXTI_Lines);
		Plane_Lock_FLAG = 1;		//解锁飞机
	}
}
