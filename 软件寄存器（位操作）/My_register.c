#include "stm32f10x.h"                  // Device header
#include "My_register.h"


/*****************************************************************
 *简	介：软件寄存器【读】
 *参	数：寄存器名字，标志位名字
 *返回值：标志位状态
 *说	明：标志位名字最好用枚举定义
 *****************************************************************/
uint8_t My_Get_Flag(volatile My_8_Reg* Reg_name, uint8_t Flag_name)
{
	return (*Reg_name & Flag_name) ? 1 : 0 ;
}

/*****************************************************************
 *简	介：软件寄存器【写】
 *参	数：寄存器名字，标志位名字，标志位状态
 *返回值：无
 *说	明：标志位名字最好用枚举定义
 *****************************************************************/
void My_Set_Flag(volatile My_8_Reg* Reg_name, uint8_t Flag_name, uint8_t Flag_status)
{
	/*先清除目标标志位（置 0）*/
	*Reg_name &= ~(Flag_name);
	/*再写入新值（0 或 1）*/
	if(!Flag_status)		//0
		return;
	else								//1
		*Reg_name |=(Flag_name);
}
