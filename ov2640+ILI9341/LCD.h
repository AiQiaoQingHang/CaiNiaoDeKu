#ifndef LCD_H
#define LCD_H

#define LCD_Base_Addr 0x60000000
#define LCD_Data_Addr (LCD_Base_Addr + (1 << 19))
#define LCD_Command   (*(volatile uint16_t *)LCD_Base_Addr)
#define LCD_Data      (*(volatile uint16_t *)(LCD_Base_Addr + (1 << 19))) // A18作为RS，由于是16位数据所以需要多左移一位

#define LCD_Length    (320 - 1)
#define LCD_wide      (240 - 1)

extern DMA_HandleTypeDef lcd_dma;

void LCD_Init(void);
void LCD_DMA_Init(void);
void LCD_Write_Command(uint16_t Command);
void LCD_Write_Data(uint16_t Data);
uint16_t LCD_Read_Data(void);
uint32_t LCD_Get_ID(void);
void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_Fill(uint16_t color);
#endif