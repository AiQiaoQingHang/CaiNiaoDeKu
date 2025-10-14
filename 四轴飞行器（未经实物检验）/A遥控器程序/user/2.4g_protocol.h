#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "stm32f10x.h"              // Device header

#define Pack_Header 0xAA						//֡ͷ
#define Pack_Ender	0x55						//֡β
#define Pack_Length	 7							//���ݰ�����

#define Unlock				0x01					//�ɻ�����
#define Lock					0x00					//�ɻ�����

extern volatile uint8_t Send_Packet_FLAG;
extern volatile uint8_t Plane_Lock_FLAG;		//�ɻ�������־λ

typedef struct
{
	uint8_t Pack_Head;					 //֡ͷ
	uint8_t Plane_Lock;					 //�ɻ�����
	uint8_t Really_Thr_H;        //ʵ�����Ÿ�8λ
	uint8_t Really_Thr_L;        //ʵ�����ŵ�8λ
	int8_t 	Pitch;							 //������
	int8_t 	Roll;								 //������
	uint8_t Pack_End;						 //֡β
}Control_Data;

void Fill_Control_Data(Control_Data* Input);                                                     														//���������ݰ�
void Send_Packet(Control_Data* Send);                                                                                       //���Ϳ������ݰ�
void Open_Packet(Control_Data* Send,uint8_t* Rece);																																					//�����ݰ�
#endif
