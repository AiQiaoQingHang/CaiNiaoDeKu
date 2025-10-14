#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include "system.h"

/*˵����
 *ҡ�˵������ɡ���ʱ��-ADC-DMA��ȫ�Զ��ɼ��������ݴ����Rocker_Data��
 *������ݰ���Rocker_Data��ȡ���ݲ��������ݴ���
 *����������NRF24L01+����
 *�ɻ��Ľ������ⲿ�жϲ�׽ҡ�˰��µĵ�ƽ�任�����ж�����λ
 */




int main()
{
	system_Init();		//ϵͳ��ʼ��
	Control_Data Send_Data;
	while(1)
	{
		Fill_Control_Data(&Send_Data);		//������ݰ�
		Send_Packet(&Send_Data);					//�������ݰ�
		IWDG_ReloadCounter();							//ι��
		Delay_ms(20);											//����ע�⿴�Ź���ʱ��һ��Ҫ���ڳ�������һ������ӳٵ�ʱ�䣬��Ҫע�ⷢ�͵ļ������̫�̣���Ȼ���շ����ݻ����
	}
}















/*ҡ�˲���*/
//int main()
//{
//	system_Init();
//	while(1)
//	{
//	
//	oled_showNum(0,0,Rocker_Data.Thr,4);
//	oled_showNum(0,16,Rocker_Data.Dir_X,4);
//	oled_showNum(0,32,Rocker_Data.Dir_Y,4);	
//		
//	Up_SendData();	
////	oled_showSignNum(0,0,Send_Data.Pitch,4);
////	oled_showSignNum(0,16,Send_Data.Roll,4);
////	oled_showNum(0,32,(Send_Data.Really_Thr_H<<8) | Send_Data.Really_Thr_L,4);	
//	oled_UpData();
//	}
//}




/*�������Ǻ�������*/
//int main()
//{
//	system_Init();
//	float angle = asin_fast(-1);
//	oled_showFloatNum(0,0,angle,3,4);
//	angle = cos_fast(555);
//	oled_showFloatNum(0,20,angle,3,4);
//	angle = atan2_fast(1,2);
//	oled_showFloatNum(0,40,angle,3,4);
//	
//	oled_UpData();
//	return  0;
//}

/*����ͨ�Ų���*/
//extern int8_t SendFLAG;
//int main()
//{
//	uint8_t data[40]={7,0x01,0x02,0x03,0x04,0x05,
//										5,0x01,0x02,0x03,0x04,0x05,
//										5,0x01,0x02,0x03,0x04,0x05,
//										5,0x01,0x02,0x03,0x04,0x05,
//										5,0x01,0x02,0x03,0x04,0x05,
//										5,0x01,1,1,1,1,1,1,1,1};

//	oled_Init();
//	NRF24L01_Init();
//	uint8_t config=NRF24L01_R_Reg(NRF24L01_CONFIG);
//	oled_showHexNum(0,0,config,2);
//	
//	uint8_t AW=NRF24L01_R_Reg(NRF24L01_SETUP_AW);
//	oled_showHexNum(20,0,AW,2);
//	
//	uint8_t RC_CH=NRF24L01_R_Reg(NRF24L01_RF_CH);
//	oled_showHexNum(40,0,RC_CH,2);
//	
//	uint8_t RC_SETUP=NRF24L01_R_Reg(NRF24L01_RF_SETUP);	
//	oled_showHexNum(60,0,RC_SETUP,2);	
//	
//	uint8_t SETUP_RETR=NRF24L01_R_Reg(NRF24L01_SETUP_RETR);	
//	oled_showHexNum(80,0,SETUP_RETR,2);
//	
//	uint8_t ENAA=NRF24L01_R_Reg(NRF24L01_EN_AA);
//	oled_showHexNum(100,0,ENAA,2);	
//	
//	
//	uint8_t ENRXADDR=NRF24L01_R_Reg(NRF24L01_EN_RXADDR);
//	oled_showHexNum(0,20,ENRXADDR,2);





		//���ղ���
//	uint8_t rece[5]={0};
////	while(1)
////	{
////		NRF24L01_R_Regs(NRF24L01_R_RX_PAYLOAD,rece,5);
////		if(rece[0]!=0)
////			break;
////	}



		//���Ͳ���
//for(uint8_t i=0;i<4;i++)
//{
//	NRF24L01_SendData(data,32);
//	Delay_s(1);
//}
//	//uint8_t status=NRF24L01_R_Reg(NRF24L01_FIFO_STATUS);
//	uint8_t status=NRF24L01_R_Reg(NRF24L01_STATUS);
//	oled_showHexNum(0,40,status,2);
//	oled_showSignNum(80,40,SendFLAG,2);
//	oled_UpData();
//	





////	oled_showstr(80,40,"OK");
////	//oled_showHexNum(20,20,rece[0],2);
////	oled_showHexNum(40,20,rece[1],2);
////	oled_showHexNum(20,40,rece[2],2);
////	oled_showHexNum(40,40,rece[3],2);
////	oled_showHexNum(60,20,rece[4],2);
////	oled_UpData();
//	return 0;
//}

