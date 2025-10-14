#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "stm32f10x.h"              // Device header

#define Pack_Header 0xAA						//帧头
#define Pack_Ender	0x55						//帧尾
#define Pack_Length	 7							//数据包长度

#define Unlock				0x01					//飞机解锁
#define Lock					0x00					//飞机上锁

extern volatile uint8_t Send_Packet_FLAG;
extern volatile uint8_t Plane_Lock_FLAG;		//飞机上锁标志位

typedef struct
{
	uint8_t Pack_Head;					 //帧头
	uint8_t Plane_Lock;					 //飞机上锁
	uint8_t Really_Thr_H;        //实际油门高8位
	uint8_t Really_Thr_L;        //实际油门低8位
	int8_t 	Pitch;							 //俯仰角
	int8_t 	Roll;								 //翻滚角
	uint8_t Pack_End;						 //帧尾
}Control_Data;

void Fill_Control_Data(Control_Data* Input);                                                     														//填充控制数据包
void Send_Packet(Control_Data* Send);                                                                                       //发送控制数据包
void Open_Packet(Control_Data* Send,uint8_t* Rece);																																					//打开数据包
#endif
