#ifndef OLED_H
#define OLED_H
#include <stdbool.h>
#include <stdint.h>

#define OLED_DATA_CONTROL    0x40 // 数据控制位
#define OLED_COMMAND_CONTROL 0x00 // 命令控制位

#define ERR_CHECK(Driver_Command, IIC_Err)        \
    do {                                          \
        if (!(Driver_Command)) {                  \
            OLED_Drv->Err_Flag = (IIC_Err);       \
            OLED_Drv->IIC_Generate_Stop();        \
            if (OLED_Drv->Err_CallBack) {         \
                OLED_Drv->Err_CallBack(OLED_Drv); \
            }                                     \
            return false;                         \
        }                                         \
    } while (0)

/* IIC错误枚举 */
typedef enum {
    None_Err,                   // 没有错误
    Generate_Start_Err,         // 起始位产生错误
    Send_Dev_Addr_Err,          // 设备地址发送错误
    Send_Data_Control_Err,      // 发送数据控制位错误
    Send_Command_Control_Error, // 发送命令控制位错误
    Send_Data_Err,              // 发送数据错误
    Generate_Stop_Err,          // 停止位产生错误
} IIC_Err_Flag;

/* 芯片控制结构体 */
struct OLED_Driver_t;
typedef struct OLED_Driver_t OLED_Driver_t;
struct OLED_Driver_t {
    uint8_t oled_buffer[8][128]; // 缓存区
    IIC_Err_Flag Err_Flag;       // 错误标志位
    uint8_t oled_adress;
    bool (*IIC_Generate_Start)(void);         // 产生起始条件
    bool (*IIC_Send_Data)(uint8_t Send_Data); // 发送一字节数据
    bool (*IIC_Generate_Stop)(void);          // 产生停止位
    void (*Delay_us)(uint32_t us);            // 微秒延迟
    void (*Err_CallBack)(OLED_Driver_t *Drv); // 错误回调函数
};

extern uint8_t ASCLL8x16[]; // 8x16ASCLL字库
/*------------------------------------------ 实 现 函 数 ------------------------------------------------------*/
bool oled_Init(OLED_Driver_t *OLED_Drv);                                   // 初始化
bool oled_Pointer(OLED_Driver_t *OLED_Drv, uint16_t x, uint8_t Page);      // 指针位置
bool oled_UpData(OLED_Driver_t *OLED_Drv);                                 // 更新屏幕
bool oled_SendData(OLED_Driver_t *OLED_Drv, uint8_t *Data, uint16_t size); // 发送数据
/*------------------------------------------ 显 ------示 ------------------------------------------------------*/
/*------------------------------以下所有函数都要调用oled_UpData更新屏幕 ----------------------------------------*/
bool oled_clear(OLED_Driver_t *OLED_Drv);                                                                                  // 清屏
bool oled_showimage(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, uint8_t height, uint8_t width, const uint8_t *image);   // 显示图片
bool oled_showchar(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, char data);                                              // 显示字符
bool oled_showstr(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, char *data);                                              // 显示字符串
bool oled_showHexNum(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, uint16_t HexNum, uint8_t length);                      // 显示无符号十六进制
bool oled_showNum(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, uint16_t Num, uint8_t length);                            // 显示无符号十进制
bool oled_showSignNum(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, int16_t Num, uint8_t length);                         // 显示有符号十进制
bool oled_showFloatNum(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, float Number, uint8_t IntLength, uint8_t FraLength); // 显示浮点数

#endif
