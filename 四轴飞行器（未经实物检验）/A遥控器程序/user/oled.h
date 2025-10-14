#ifndef OLED_H
#define OLED_H
#include "stm32f10x.h"                  																													//Device header
//#include "oled_ziku.h"                  																													//引用oled字库
#define oled_adress 					0x78      																													//oled地址
#define oled_datacontrol 			0x40																																//数据控制位
#define oled_commandcontrol 	0x00																																//命令控制位
																													
extern uint8_t ASCLL8x16[];             																													//8x16ASCLL字库
/*------------------------------------------ 实 现 函 数 ------------------------------------------------------*/
void oled_Init(void);                                                                         		//初始化
void oled_Pointer(uint16_t x,uint8_t Page);                                                   		//指针位置
void oled_UpData(void);                                                                       		//更新屏幕
void oled_SendData(uint8_t* Data,uint16_t size);                                              		//发送数据
uint32_t oled_Pow(uint8_t x,uint8_t y);                                                       		//次方函数，计算权重

/*------------------------------------------ 显 ------示 ------------------------------------------------------*/
/*------------------------------以下所有函数都要调用oled_UpData更新屏幕 ----------------------------------------*/
void oled_clear(void);                                                                            //清屏
void oled_showimage(uint8_t x,uint8_t y,uint8_t height,uint8_t width,const uint8_t* image);   		//显示图片
void oled_showchar(uint8_t x,uint8_t y ,char data);                                           		//显示字符
void oled_showstr(uint8_t x,uint8_t y ,char* data);                                           		//显示字符串
void oled_showHexNum(uint8_t x,uint8_t y ,uint16_t HexNum,uint8_t length);                    		//显示无符号十六进制
void oled_showNum(uint8_t x,uint8_t y ,uint16_t Num,uint8_t length);                          		//显示无符号十进制
void oled_showSignNum(uint8_t x,uint8_t y ,int16_t Num,uint8_t length);                       		//显示有符号十进制
void oled_showFloatNum(int8_t x, int8_t y, float Number, uint8_t IntLength, uint8_t FraLength);		//显示浮点数																																															

#endif
