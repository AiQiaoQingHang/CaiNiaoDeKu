#include "stm32f10x.h"                  // Device header
#include "2.4g_protocol.h"
#include "NRF24L01.h"
#include "rocker.h"

volatile uint8_t Send_Packet_FLAG = 0;			//发送数据包标志位，在中断里被置位
volatile uint8_t Plane_Lock_FLAG = Lock;		//飞机上锁标志位

/********************************************************************
 *简	介：填充控制数据包
 *参	数：存储控制数据结构体
 *返回值：无
 *说	明：从Rocker_Data里获取数据
 *********************************************************************/
void Fill_Control_Data(Control_Data* Input)
{
	/*死区上下限计算*/
	static const float Thr_Dead_limUP  = Rocker_Mid + Dead_Thr;		//油门死区
	static const float Dir_Dead_limUP  = Rocker_Mid + Dead_Dir;		//方向死区上限
	static const float Dir_Dead_limLOW = Rocker_Mid - Dead_Dir;		//方向死区下限
	int16_t Thr_temp = 0;
	/*油门,由于油门采用全向摇杆，所以舍弃摇杆向下的部分*/
	if(Rocker_Data.Thr >= Thr_Dead_limUP)																										 //死区外正常计算
	{		
		Thr_temp = (int16_t)((((float)Rocker_Data.Thr) / ADC_Range) * Thr_Range);
		Input->Really_Thr_H = Thr_temp>>8;		//存储高八位
	  Input->Really_Thr_L = Thr_temp;			//存储低八位
	}
	else
	{
		Input->Really_Thr_H = 0;		//死区内与摇杆向下时油门为0
		Input->Really_Thr_L = 0;
	}
	/*俯仰角*/
	if((Rocker_Data.Dir_X >= Dir_Dead_limLOW) && (Rocker_Data.Dir_X <= Dir_Dead_limUP))			//处于死区内
		Input->Pitch = 0;				//归0
	else
		Input->Pitch = (int8_t)((((float)Rocker_Data.Dir_X - Rocker_Mid) / ADC_Range) * Pitch_Range);	//死区外正常计算
	/*翻滚角*/
	if((Rocker_Data.Dir_Y >= Dir_Dead_limLOW) && (Rocker_Data.Dir_Y <= Dir_Dead_limUP))			//处于死区内
		Input->Roll = 0;					//归0
	else
		Input->Roll = (int8_t)((((float)Rocker_Data.Dir_Y - Rocker_Mid) / ADC_Range) * Roll_Range);	//死区外正常计算
	/*添加帧头和帧尾*/
	Input->Pack_Head = Pack_Header;		//帧头
	Input->Pack_End  = Pack_Ender;		//帧尾
	/*添加飞机解锁状况*/
	Input->Plane_Lock = Plane_Lock_FLAG;
}

/********************************************************************
 *简	介：打开数据包
 *参	数：待打开的数据包
 *返回值：无
 *说	明：无
 *********************************************************************/
void Open_Packet(Control_Data* Send,uint8_t* Rece)
{
	uint8_t* ptr = (uint8_t*)Send;
	for(uint8_t i = 0;i < Pack_Length;i++)
	{
		Rece[i] = ptr[i];
	}
}
/********************************************************************
 *简	介：发送数据包
 *参	数：待发送数据包
 *返回值：无
 *说	明：无
 *********************************************************************/
void Send_Packet(Control_Data* Send)
{
	if(!Send)	return;		//空指针检查
	NRF24L01_W_Tx(W_TX_PAYLOAD,(uint8_t*)Send,Pack_Length,0);		//结构体内存是连续的，直接取第一个数据地址然后自增就可以
	/*产生脉冲，发送数据*/
	NRF24L01_SendData();
}
