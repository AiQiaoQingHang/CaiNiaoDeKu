#ifndef OLED_H
#define OLED_H
#include "stm32f10x.h"                  																													//Device header
//#include "oled_ziku.h"                  																													//����oled�ֿ�
#define oled_adress 					0x78      																													//oled��ַ
#define oled_datacontrol 			0x40																																//���ݿ���λ
#define oled_commandcontrol 	0x00																																//�������λ
																													
extern uint8_t ASCLL8x16[];             																													//8x16ASCLL�ֿ�
/*------------------------------------------ ʵ �� �� �� ------------------------------------------------------*/
void oled_Init(void);                                                                         		//��ʼ��
void oled_Pointer(uint16_t x,uint8_t Page);                                                   		//ָ��λ��
void oled_UpData(void);                                                                       		//������Ļ
void oled_SendData(uint8_t* Data,uint16_t size);                                              		//��������
uint32_t oled_Pow(uint8_t x,uint8_t y);                                                       		//�η�����������Ȩ��

/*------------------------------------------ �� ------ʾ ------------------------------------------------------*/
/*------------------------------�������к�����Ҫ����oled_UpData������Ļ ----------------------------------------*/
void oled_clear(void);                                                                            //����
void oled_showimage(uint8_t x,uint8_t y,uint8_t height,uint8_t width,const uint8_t* image);   		//��ʾͼƬ
void oled_showchar(uint8_t x,uint8_t y ,char data);                                           		//��ʾ�ַ�
void oled_showstr(uint8_t x,uint8_t y ,char* data);                                           		//��ʾ�ַ���
void oled_showHexNum(uint8_t x,uint8_t y ,uint16_t HexNum,uint8_t length);                    		//��ʾ�޷���ʮ������
void oled_showNum(uint8_t x,uint8_t y ,uint16_t Num,uint8_t length);                          		//��ʾ�޷���ʮ����
void oled_showSignNum(uint8_t x,uint8_t y ,int16_t Num,uint8_t length);                       		//��ʾ�з���ʮ����
void oled_showFloatNum(int8_t x, int8_t y, float Number, uint8_t IntLength, uint8_t FraLength);		//��ʾ������																																															

#endif
