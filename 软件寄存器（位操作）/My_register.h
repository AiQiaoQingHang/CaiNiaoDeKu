#ifndef MY_REGISTER
#define MY_REGISTER

/*软件寄存器【位操作】
 *使用时用My_8_Reg定义寄存器，再使用枚举定义寄存器的每一位，调用函数操控标志位
 *
 */
#include "stm32f10x.h"                  // Device header
typedef uint8_t My_8_Reg;

uint8_t My_Get_Flag(volatile My_8_Reg* Reg_name, uint8_t Flag_name);                                 //获取标志位
void My_Set_Flag(volatile My_8_Reg* Reg_name, uint8_t Flag_name, uint8_t Flag_status);               //更改标志位状态
#endif
