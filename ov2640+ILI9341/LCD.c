#include "main.h"
#include "LCD.h"

DMA_HandleTypeDef lcd_dma;

/**
 * @brief LCD初始化
 *
 */
void LCD_Init(void)
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_SET);
    LCD_Write_Command(0x0013);   //打开正常显示模式
    LCD_Write_Command(0x0036);   LCD_Write_Data(0x0028);   //竖屏显示
    LCD_Write_Command(0x003A);   LCD_Write_Data(0x0055);   //RGB565
    LCD_Write_Command(0x0011);
    HAL_Delay(50);
    LCD_Write_Command(0x0029);   //开启显示
    LCD_SetWindow(1, 1,LCD_Length , LCD_wide);
    LCD_Write_Command(0x002C);
    for(uint8_t i = 0;i<100;i++)
    {
        LCD_Fill(0xFF80);
    }
}


/**
 * @brief LCD写命令
 *
 * @param Command
 */
void LCD_Write_Command(uint16_t Command)
{
    LCD_Command = Command;
}

/**
 * @brief LCD写数据
 *
 * @param Data
 */
void LCD_Write_Data(uint16_t Data)
{
    LCD_Data = Data;
}

/**
 * @brief LCD读数据
 *
 * @return uint16_t
 */
uint16_t LCD_Read_Data(void)
{
    return LCD_Data;
}

/**
 * @brief 获取芯片ID
 *
 * @return uint32_t 芯片ID
 */
uint32_t LCD_Get_ID(void)
{
    uint32_t temp = 0;
    LCD_Write_Command(0xD3);
    LCD_Read_Data();
    LCD_Read_Data();
    temp = LCD_Read_Data();
    temp <<= 8;
    temp |= LCD_Read_Data();
    return temp;
}

/**
 * @brief 设置显示范围【矩形】
 *
 * @param x0 矩形左上角x
 * @param y0 矩形左上角y
 * @param x1 矩形右下角x
 * @param y1 矩形右下角y
 */
void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // 设置列地址范围 (0x2A)
    LCD_Write_Command(0x2A);
    LCD_Write_Data(x0 >> 8);   // 起始列高字节
    LCD_Write_Data(x0 & 0xFF); // 起始列低字节
    LCD_Write_Data(x1 >> 8);   // 结束列高字节
    LCD_Write_Data(x1 & 0xFF); // 结束列低字节
    // 设置行地址范围 (0x2B)
    LCD_Write_Command(0x2B);
    LCD_Write_Data(y0 >> 8);   // 起始行高字节
    LCD_Write_Data(y0 & 0xFF); // 起始行低字节
    LCD_Write_Data(y1 >> 8);   // 结束行高字节
    LCD_Write_Data(y1 & 0xFF); // 结束行低字节
}

void LCD_Fill(uint16_t color)
{

    for (uint16_t i = 0; i < 1000; i++) {
        LCD_Write_Data(color);
    }
}