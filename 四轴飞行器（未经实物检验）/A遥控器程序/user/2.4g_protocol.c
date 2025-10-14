#include "stm32f10x.h"                  // Device header
#include "2.4g_protocol.h"
#include "NRF24L01.h"
#include "rocker.h"

volatile uint8_t Send_Packet_FLAG = 0;			//�������ݰ���־λ�����ж��ﱻ��λ
volatile uint8_t Plane_Lock_FLAG = Lock;		//�ɻ�������־λ

/********************************************************************
 *��	�飺���������ݰ�
 *��	�����洢�������ݽṹ��
 *����ֵ����
 *˵	������Rocker_Data���ȡ����
 *********************************************************************/
void Fill_Control_Data(Control_Data* Input)
{
	/*���������޼���*/
	static const float Thr_Dead_limUP  = Rocker_Mid + Dead_Thr;		//��������
	static const float Dir_Dead_limUP  = Rocker_Mid + Dead_Dir;		//������������
	static const float Dir_Dead_limLOW = Rocker_Mid - Dead_Dir;		//������������
	int16_t Thr_temp = 0;
	/*����,�������Ų���ȫ��ҡ�ˣ���������ҡ�����µĲ���*/
	if(Rocker_Data.Thr >= Thr_Dead_limUP)																										 //��������������
	{		
		Thr_temp = (int16_t)((((float)Rocker_Data.Thr) / ADC_Range) * Thr_Range);
		Input->Really_Thr_H = Thr_temp>>8;		//�洢�߰�λ
	  Input->Really_Thr_L = Thr_temp;			//�洢�Ͱ�λ
	}
	else
	{
		Input->Really_Thr_H = 0;		//��������ҡ������ʱ����Ϊ0
		Input->Really_Thr_L = 0;
	}
	/*������*/
	if((Rocker_Data.Dir_X >= Dir_Dead_limLOW) && (Rocker_Data.Dir_X <= Dir_Dead_limUP))			//����������
		Input->Pitch = 0;				//��0
	else
		Input->Pitch = (int8_t)((((float)Rocker_Data.Dir_X - Rocker_Mid) / ADC_Range) * Pitch_Range);	//��������������
	/*������*/
	if((Rocker_Data.Dir_Y >= Dir_Dead_limLOW) && (Rocker_Data.Dir_Y <= Dir_Dead_limUP))			//����������
		Input->Roll = 0;					//��0
	else
		Input->Roll = (int8_t)((((float)Rocker_Data.Dir_Y - Rocker_Mid) / ADC_Range) * Roll_Range);	//��������������
	/*���֡ͷ��֡β*/
	Input->Pack_Head = Pack_Header;		//֡ͷ
	Input->Pack_End  = Pack_Ender;		//֡β
	/*��ӷɻ�����״��*/
	Input->Plane_Lock = Plane_Lock_FLAG;
}

/********************************************************************
 *��	�飺�����ݰ�
 *��	�������򿪵����ݰ�
 *����ֵ����
 *˵	������
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
 *��	�飺�������ݰ�
 *��	�������������ݰ�
 *����ֵ����
 *˵	������
 *********************************************************************/
void Send_Packet(Control_Data* Send)
{
	if(!Send)	return;		//��ָ����
	NRF24L01_W_Tx(W_TX_PAYLOAD,(uint8_t*)Send,Pack_Length,0);		//�ṹ���ڴ��������ģ�ֱ��ȡ��һ�����ݵ�ַȻ�������Ϳ���
	/*�������壬��������*/
	NRF24L01_SendData();
}
