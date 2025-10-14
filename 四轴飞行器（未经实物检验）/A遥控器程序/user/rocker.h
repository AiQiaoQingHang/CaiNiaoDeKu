#ifndef ROCKER_H
#define ROCKER_H
#include "stm32f10x.h"                  // Device header

/*
 *注：ADC通道配置，需和上面的Pin一一对应
 */

/*油门摇杆*/
#define Throttle_Rocker_RCC						RCC_APB2Periph_GPIOA
#define Throttle_Rocker_Port					GPIOA
#define Throttle_Rocker_Pin						GPIO_Pin_1
#define Throttle_Rocker_ADC						ADC_Channel_1
/*方向摇杆【X】*/
#define Directional_X_Rocker_RCC			RCC_APB2Periph_GPIOA
#define Directional_X_Rocker_Port     GPIOA
#define Directional_X_Rocker_Pin      GPIO_Pin_2
#define	Directional_X_Rocker_ADC			ADC_Channel_2
/*方向摇杆【Y】*/
#define Directional_Y_Rocker_RCC			RCC_APB2Periph_GPIOA
#define Directional_Y_Rocker_Port     GPIOA
#define Directional_Y_Rocker_Pin      GPIO_Pin_3
#define Directional_Y_Rocker_ADC			ADC_Channel_3
/*摇杆按压中断*/
#define Interrupt_Rocker_RCC					RCC_APB2Periph_GPIOB
#define Interrupt_Rocker_Port     		GPIOB
#define Interrupt_Rocker_Pin      		GPIO_Pin_6
//注意要一一对应，外部中断函数也需要修改
#define Interrupt_Rocker_EXTI_Port								GPIO_PortSourceGPIOB
#define Interrupt_Rocker_EXTI_Pin									GPIO_PinSource6
#define Interrupt_Rocker_EXTI_Lines								EXTI_Line6
#define Interrupt_Rocker_NVIC_Channel							EXTI9_5_IRQn
#define Interrupt_Rocker_NVIC_PreemptionPriority		2
#define Interrupt_Rocker_NVIC_SubPriority					2
/*死区,这里是原始数据*/
#define Dead_Dir			 50.0f       //方向死区
#define Dead_Thr			 50.0f       //油门死区
/*飞行器控制参数*/
#define Rocker_Mid	 1950.0f   //摇杆中间值,实测摇杆得出

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
	uint16_t Thr;                //原始油门数据
	uint16_t Dir_X;              //原始【X】方向数据
	uint16_t Dir_Y;              //原始【Y】方向数据
}Rocker;

extern volatile Rocker Rocker_Data;				//摇杆数据

void Rocker_Init(void);																														//摇杆初始化
void ADC_GPIO_Init(void);                                                         //ADC的GPIO初始化
void Rocker_Timer_Init(void);                                                     //摇杆定时器初始化
void Rocker_DMA_Init(void);                                                       //摇杆DMA初始化
void Rocker_Int_Init(void);																												//初始化摇杆中断
void EXTI9_5_IRQHandler(void);																										//外部中断函数

#endif
