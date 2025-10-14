#include "stm32f10x.h"                  // Device header
#include "watch_dog.h"

/********************************************************************
 *简	介：看门狗初始化
 *参	数：无
 *返回值：无
 *说	明：无
 *********************************************************************/
void Watch_dog_Init(void)
{
	/*解锁寄存器读写*/
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	/*设置预分频*/
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	/*设置Count值*/
	IWDG_SetReload(Dog_time);
	/*使能看门狗*/
	IWDG_Enable();
}
