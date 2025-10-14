#include <math.h>
#include "stm32f10x.h"                  // Device header
#include "I2C.h"
#include "Delay.h"
#include "oled.h"
#include "oled_ziku.h" 
#include "USART.h"
#include <stdio.h>
uint8_t oled_buffer[8][128]={0};

/********************************************************************
 *简	介：oled初始化
 *参	数：无
 *返回值：无
 *说	明：使用oled时调用
 *********************************************************************/
void oled_Init()
{
	SI2C_Init(); //初始化软件IIC引脚
	
	SI2C_Start();
	/*寻址*/
  SI2C_SendByte(oled_adress);
	/*发送命令字节*/
  SI2C_SendByte(oled_commandcontrol);
	/*常规初始化*/
  SI2C_SendByte(0xAE);
  SI2C_SendByte(0xD5);
  SI2C_SendByte(0x80);
  SI2C_SendByte(0xA8);
  SI2C_SendByte(0x3F);
  SI2C_SendByte(0xD3);
  SI2C_SendByte(0x00);
  SI2C_SendByte(0x40);
  SI2C_SendByte(0xA1);
  SI2C_SendByte(0xC8);
  SI2C_SendByte(0xDA);
  SI2C_SendByte(0x12);
  SI2C_SendByte(0x81);
  SI2C_SendByte(0xCF);
  SI2C_SendByte(0xD9);
  SI2C_SendByte(0xF1);
  SI2C_SendByte(0xDB);
  SI2C_SendByte(0x30);
  SI2C_SendByte(0xA4);
	SI2C_SendByte(0xA6);
  SI2C_SendByte(0x8D);
  SI2C_SendByte(0x14);
  SI2C_SendByte(0xAF);
  Delay_us(100);
  SI2C_Stop();
	/*缓存数组初始全为0，默认黑屏*/
	oled_UpData();
}

/********************************************************************
 *简	介：oled光标位置
 *参	数：x坐标【0-127】，y坐标【0-7】
 *返回值：无
 *说	明：无
 *********************************************************************/
void oled_Pointer(uint16_t x,uint8_t Page)
{
	SI2C_Start();
	SI2C_SendByte(oled_adress);								//寻址
	SI2C_SendByte(oled_commandcontrol);				//发送命令
	SI2C_SendByte(0xB0 | Page);								//设置页位置
	SI2C_SendByte(0x10 | ((x & 0xF0) >> 4));	//设置X位置高4位
	SI2C_SendByte(0x00 | (x & 0x0F));					//设置X位置低4位
	SI2C_Stop();
}

/********************************************************************
 *简	介：oled发送数据
 *参	数：数据，数据大小
 *返回值：无
 *说	明：无
 *********************************************************************/
void oled_SendData(uint8_t* Data,uint16_t size)
{
	SI2C_Start();
	SI2C_SendByte(oled_adress);				//寻址
	SI2C_SendByte(oled_datacontrol);	//表明发送数据
	for(uint16_t i=0;i<size;i++)
	{
		SI2C_SendByte(Data[i]);
	}
	SI2C_Stop();
}

/********************************************************************
 *简	介：将oled_buffer的内容更新到屏幕上
 *参	数：无
 *返回值：无
 *说	明：所有显示都需要调用
 *********************************************************************/
void oled_UpData(void)
{
	
	for(uint8_t i=0;i<8;i++)
	{
		oled_Pointer(0,i);
		oled_SendData(oled_buffer[i],128);	//一次更新一页，循环8次
	}
}

/********************************************************************
 *简	介：清屏
 *参	数：无
 *返回值：无
 *说	明：将缓存区清0，并更新到屏幕
 *********************************************************************/
void oled_clear(void)
{
	for(uint8_t i;i<8;i++)
		for(uint16_t j;j<128;j++)
				oled_buffer[i][j]=0;  //遍历填充0，?memset？
	oled_UpData();							//更新屏幕
}

/********************************************************************
 *简	介：显示字符
 *参	数：x坐标【0-127】，y坐标【0-63】，显示字符
 *返回值：无
 *说	明：只能显示单个字符
 *********************************************************************/
void oled_showchar(uint8_t x,uint8_t y ,char data)
{
	for(uint8_t j=0;j<2;j++)
	{
		for(uint8_t i=0;i<8;i++)
		{
			oled_buffer[y/8+j][x+i] 			&=~(0xFF<<(y%8));//和下面一行操作的是同一片像素，实现方式是将1挪到突出下一页的部分，然后取反在与，舍弃低位
			oled_buffer[y/8+j][x+i] 			|=ASCLL8x16[16*(data-' ')+i+8*j]<<(y%8);
/*y%8计算出距离上一页差多少格，对于字库数据来说，在这一页要写的数据就是字库数据的低位，左移将低位舍弃*/	
			
			oled_buffer[y/8+1+j][x+i] 		&=~(0xFF>>(8-(y%8)));//和下面一行操作的是同一片像素，实现方式是将1挪到占此页的部分，然后取反在与，舍弃高位
			oled_buffer[y/8+1+j][x+i] 		|=ASCLL8x16[16*(data-' ')+i+8*j]>>(8-(y%8));
/*(8-(y%8))计算出在这一页占了多少格，对于字库数据来说，在这一页要写的数据就是字库数据的高位，右移将低位位舍弃	，实现方式是将0挪到占用此页的部分，然后与*/
/*要注意字库数据和缓存区数据的对应关系！！！*/
		}
	}
}

/********************************************************************
 *简	介：显示字符串
 *参	数：x坐标【0-127】，y坐标【0-63】，字符串
 *返回值：无
 *说	明：字符串必须以【\0】结尾
 *********************************************************************/
void oled_showstr(uint8_t x,uint8_t y ,char* data)
{
	uint8_t i=0;
	while(*(data+i)!='\0')
	{
		oled_showchar(x+8*i,y,*(data+i));
		i++;
	}
}


/********************************************************************
 *简	介：显示字符串
 *参	数：x坐标【0-127】，y坐标【0-63】，图像宽度，图像数组
 *返回值：无
 *说	明：无
 *********************************************************************/
void oled_showimage(uint8_t x,uint8_t y,uint8_t height,uint8_t width,const uint8_t* image)
{
	if(x<128 & y<=64)
	{
    for(uint8_t j=0; j<(height-1)/8+1; j++)
    {
        for(uint8_t i=0; i<width; i++)
        {
						oled_buffer[y/8+j][x+i] 			&=~(0xFF<<(y%8));//具体看oled_showchar
            oled_buffer[y/8+j][x+i]   |= (image[j*width+i]<<(y % 8));       //除8取余可以得到距离上一页的距离，或运算不改变其他位的值
            
						oled_buffer[y/8+1+j][x+i] 		&=~(0xFF>>(8-(y%8)));//具体看oled_showchar
						oled_buffer[y/8+1+j][x+i] |= (image[j*width+i]>>(8-(y % 8)));  //得到超出上一页多少
        }
    }
	}
}


/********************************************************************
 *简	介：计算x的y次方
 *参	数：底数x【0-127】，指数y【0-127】
 *返回值：无
 *说	明：上层函数调用，用于计算权重
 *********************************************************************/
uint32_t oled_Pow(uint8_t x,uint8_t y)
{
	uint32_t result=1;   //默认为1
	while(y--)
		result*=x;
	return result;
}

/********************************************************************
 *简	介：显示无符号十进制数
 *参	数：x坐标【0-127】，y坐标【0-63】，Num显示数字，length显示长度
 *返回值：无
 *说	明：无
 *********************************************************************/
void oled_showNum(uint8_t x,uint8_t y ,uint16_t Num,uint8_t length)
{
/* Number / OLED_Pow(10, Length - i - 1) % 10 可以十进制提取数字的每一位*/
/* + '0' 可将数字转换为字符格式*/
	for(uint8_t i=0;i<length;i++)
		oled_showchar(x+8*i,y,Num/oled_Pow(  10,(length-i-1)  ) %10+'0');
}

/********************************************************************
 *简	介：显示无符号16进制数
 *参	数：x坐标【0-127】，y坐标【0-63】，Num显示数字，length显示长度
 *返回值：无
 *说	明：无
 *********************************************************************/
void oled_showHexNum(uint8_t x,uint8_t y ,uint16_t HexNum,uint8_t length)
{
	uint8_t SingleNumber;
	for(uint8_t i=0;i<length;i++)
	{
		/*提取单个数字*/
		SingleNumber=HexNum/(oled_Pow(16, (length-i-1) ))%16;
		/*判断是否属于0-9，
			+'0'转字符格式*/
		if(SingleNumber<10)
		{
			oled_showchar(x+8*i,y,SingleNumber+'0');
		}
		/*属于10-16，
			+'A'转字符格式*/
		else
		{
			oled_showchar(x+8*i,y,SingleNumber-10+'A');
		}	
	}
}

/********************************************************************
 *简	介：显示有符号10进制
 *参	数：x坐标【0-127】，y坐标【0-63】，Num显示数字，length显示长度
 *返回值：无
 *说	明：x坐标会多出8，排版注意
 *********************************************************************/
void oled_showSignNum(uint8_t x,uint8_t y ,int16_t Num,uint8_t length)
{
	uint16_t Num1;
	/*数字大于0*/
	if(Num>0)
	{
		Num1=Num;
		oled_showchar(x,y,'+');	//显示‘+’号，x坐标会多8位
	}
	/*数字小于0*/
	else
	{
		Num1=-Num;	//取补码
		oled_showchar(x,y,'-');	//显示‘-’号，x坐标会多8位
	}
	/*在oled_showNum中完成单独取某一位*/
	oled_showNum(x+8,y,Num1,length);
}

/********************************************************************
 *简	介：显示浮点数
 *参	数：x坐标【0-127】，y坐标【0-63】，Num显示数字，IntLength整数显示长度，FraLength小数显示长度
 *返回值：无
 *说	明：无
 *********************************************************************/
void oled_showFloatNum(int8_t x, int8_t y, float Number, uint8_t IntLength, uint8_t FraLength)
{
	/*显示正负号*/
	if(Number>0)
	{
		oled_showchar(x,y,'+');
	}
	else
	{
		Number=-Number;
		oled_showchar(x,y,'-');		
	}
	/*直接赋值提取整数部分*/
	uint32_t Int=Number;
	/*将小数位乘到整数位*/
	uint32_t Power=oled_Pow(10,FraLength);	//获取10^n次方，用32位，不然最大只能65536
	Number-=(uint32_t)Number;	//整数部分置0
	uint32_t Fra=round(Number*Power);	//将小数乘到整数,得到小数部分,用标准库round四舍五入，避免浮点误差
	//为什么需要四舍五入：1.23存储可能是1.2299999...，经过去整数后变为0.229999...,在将小数移入整数就变为22.999...不符合
	/*处理四舍五入的进位问题*/
	Int += Fra / Power;
	//假设要显示的数为123.999...，去整数后为0.999...，正常四舍五入后应该为124.00，而此时整数部分已经被提取出去，所以需要看小数最高位（假设为n位），
	//那就需要小数部分对10^n次方整除，例如小数移入整数后为99.999...,四舍五入为100.000，此时已经等于10^2次方，整除后就是1，实现进位
	/*目前Int里为整数部分，Fra为小数部分*/
	/*显示*/
	oled_showNum(x+8,y,Int,IntLength);		//显示整数部分，‘+8’由于多了一个符号
	oled_showchar(IntLength*8+8+x,y,'.');		//显示小数点
	oled_showNum(IntLength*8+8+8+x,y,Fra,FraLength);		//显示小数
}
