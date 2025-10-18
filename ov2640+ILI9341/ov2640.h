#ifndef ov2640_H
#define ov2640_H
#include "stm32f4xx_hal.h"
#include "ov2640_define.h"

// #define Line_Count        320                                      // 行数
// #define Column_Count      240                                      // 列数
// #define Buffer_Line_Count 200                                      // 缓存区行数，一次缓存多少行
// #define DCMI_DMA_Count    ((Column_Count * Buffer_Line_Count) / 2) // DMA一次搬运32位，两个像素点
// #define FSMC_DMA_Count    (Column_Count * Buffer_Line_Count)
#define ov2640_Addr 0x60

extern const uint8_t ov2640_Init_UXGA_Config[178][2];
extern const uint8_t ov2640_Set_RGB565_Config[14][2];
extern const uint8_t ov2640_SVGA_RGB565_25fps_Config[212][2];
extern I2C_HandleTypeDef hi2c1;

void ov2640_Init(void);                                                              // 初始化
void ov2640_EnterPowerDown(void);                                                    // 进入掉电模式
void ov2640_EnterWorkMode(void);                                                     // 进入工作模式
void ov2640_Reset(void);                                                             // 复位
void ov2640_RegBank_Sel(ov2640_RegBank_Sel_t RegBank);                               // 选择寄存器组
void ov2640_W_Reg(uint8_t REG_Addr, uint8_t Data);                                   // 写寄存器
void ov2640_RGB565_Mode(void);                                                       // 设置RGB565格式
uint8_t ov2640_R_Reg(uint8_t Reg_Addr);                                              // 读寄存器
ov2640_Err_FLAG_t ov2640_Set_Output_Size(uint16_t width, uint16_t height);           // 设置图像输出大小
ov2640_Err_FLAG_t ov2640_Set_Light_Mode(ov2640_Light_Mode_t mode);                   // 设置灯光模式
ov2640_Err_FLAG_t ov2640_Set_Color_Saturation(ov2640_Color_Saturation_t saturation); // 设置色彩饱和度
ov2640_Err_FLAG_t ov2640_Set_Brightness(ov2640_Brightness_t brightness);             // 设置亮度
ov2640_Err_FLAG_t ov2640_Set_Contrast(ov2640_Contrast_t contrast);                   // 设置对比度
ov2640_Err_FLAG_t ov2640_Set_Special_Effect(ov2640_Special_Effect_t effect);         // 设置特殊效果】
#endif