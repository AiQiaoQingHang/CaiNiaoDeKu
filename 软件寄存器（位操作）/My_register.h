#ifndef MY_REGISTER
#define MY_REGISTER

/*����Ĵ�����λ������
 *ʹ��ʱ��My_8_Reg����Ĵ�������ʹ��ö�ٶ���Ĵ�����ÿһλ�����ú����ٿر�־λ
 *
 */
#include "stm32f10x.h"                  // Device header
typedef uint8_t My_8_Reg;

uint8_t My_Get_Flag(volatile My_8_Reg* Reg_name, uint8_t Flag_name);                                 //��ȡ��־λ
void My_Set_Flag(volatile My_8_Reg* Reg_name, uint8_t Flag_name, uint8_t Flag_status);               //���ı�־λ״̬
#endif
