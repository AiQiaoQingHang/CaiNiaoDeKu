#ifndef IIC_Dev_H
#define IIC_Dev_H

#include <stdint.h>
#include <stdbool.h>
#include "Drv_AK09911C_define.h"
#include "Drv_icm42688.h"

/************待完善功能************/
#define __data_not_ready_proc() (void)0
#define __data_over_proc()      (void)0
/*********************************/

#define AK09911C_DATA_SIZE_8B  6 // 数据大小
#define AK09911C_DATA_SIZE_16B (AK09911C_DATA_SIZE_8B / 2)
#define AK_DATA_BUF_SIZE 6
#define DEG_TO_RAD             0.01745f

// 手册给的校准公式，H为寄存器读数，ASA为校准系数
#define CAL_DATA(H, ASA) (H) * (((float)(ASA) / (128.0f)) + 1.0f)

/* IIC错误检查 */
#define ERR_CHECK(Driver_Command, IIC_Err)    \
    do {                                      \
        if (!(Driver_Command)) {              \
            Driver->Err_Flag = (IIC_Err);     \
            Driver->IIC_Generate_Stop();      \
            if (Driver->Err_CallBack) {       \
                Driver->Err_CallBack(Driver); \
            }                                 \
            return false;                     \
        }                                     \
    } while (0)

/* 交换字节序 */
#define BYTESWAP_16BIT(data) ((int16_t)((((data) & 0x00FF) << 8) | (((data) & 0xFF00) >> 8)))
#define BYTESWAP_ARRAY_16BIT(Arr_Ptr_16Bit, Arr_Size_16Bit) \
    do {                                                    \
        int16_t *Arr = (int16_t *)(Arr_Ptr_16Bit);          \
        for (uint8_t i = 0; i < (Arr_Size_16Bit); i++) {    \
            (Arr)[i] = BYTESWAP_16BIT((Arr)[i]);            \
        }                                                   \
    } while (0)

/* IIC错误枚举 */
// typedef enum {
//     None_Err,              // 没有错误
//     Generate_Start_Err,    // 起始位产生错误
//     RE_Generate_Start_Err, // 重复发送起始位错误
//     Send_Dev_Addr_Err,     // 设备地址发送错误
//     RE_Send_Dev_Addr_Err,  // 第二次寻址错误
//     Send_Reg_Addr_Err,     // 寄存器地址发送错误
//     Send_Data_Err,         // 发送数据错误
//     Rece_Data_Err,         // 接收数据错误
//     Generate_Stop_Err,     // 停止位产生错误
// } IIC_Err_Flag;

/* 数据结构体 */
typedef struct
{
    float Yaw;                    // 磁力计偏航角
    float X_Data, Y_Data, Z_Data; // 三轴原始数据
    float X_Comp, Y_Comp, Z_Comp; // 三轴倾斜补偿、滤波后数据
    uint8_t Data_Over_Cnt;        // 数据溢出计数
} AK09911C_Value_t;

/* 校准系数结构体 */
typedef struct
{
    uint8_t ASA_X; // X轴校准系数
    uint8_t ASA_Y; // Y轴校准系数
    uint8_t ASA_Z; // Z轴校准系数
} AK09911C_ASA_t;

/* 滑动窗口 */
#define AK09911C_SILD_WINDOW_SIZE 5 // 滑动窗口大小
typedef struct
{
    float Window_Buf[AK09911C_SILD_WINDOW_SIZE];
    uint8_t Oldest_Index;
} Sild_Window_t;
// 更新窗口值
#define UP_SILD_WINDOW(Sild_Window, New_Value)                                                                             \
    do {                                                                                                                   \
        (Sild_Window).Window_Buf[(Sild_Window).Oldest_Index] = (New_Value);                                                \
        (Sild_Window).Oldest_Index                           = (++(Sild_Window).Oldest_Index) % AK09911C_SILD_WINDOW_SIZE; \
    } while (0)

// 更新窗口输出
#define UP_SILD_WINDOW_OUT(Sild_Window, Output_Buf)               \
    do {                                                          \
        float Sum_Buf = 0.0f;                                     \
        for (uint8_t i = 0; i < AK09911C_SILD_WINDOW_SIZE; i++) { \
            Sum_Buf += (Sild_Window).Window_Buf[i];               \
        }                                                         \
        (Output_Buf) = Sum_Buf / AK09911C_SILD_WINDOW_SIZE;       \
    } while (0)

/* 校准结构体 */
// typedef struct
// {
//     float X_Bias, Y_Bias, Z_Bias; //  三轴零偏校准
//     float Yaw_Bias;               // 绝对偏航校准
// } Cal_Bias_t;

/* 芯片控制结构体 */
struct AK09911C_Driver;
typedef struct AK09911C_Driver AK09911C_Driver_t;
struct AK09911C_Driver {
    AK09911C_Value_t AK_Value;
    uint8_t AK_Data_Buf[AK09911C_DATA_SIZE_8B]; // 数据缓存区
    Sild_Window_t X_Sild;                       // X 轴滑动窗口
    Sild_Window_t Y_Sild;                       // Y 轴滑动窗口
    Sild_Window_t Z_Sild;                       // Z 轴滑动窗口
    // Cal_Bias_t Cal_Bias;                                    // 校准值
    AK09911C_ASA_t ASA_Value;                               // 校准系数，硬件内部值，从硬件读出
    IIC_Err_Flag Err_Flag;                                  // 错误标志位
    uint16_t Err_Count;                                     // 错误计数
    uint8_t Dev_Addr;                                       // 设备地址,未左移 1 位
    bool (*IIC_Generate_Start)(void);                       // 产生起始条件
    bool (*IIC_Send_Dev_Addr)(uint8_t Addr, bool IS_Write); // 发送设备地址
    bool (*IIC_Send_Data)(uint8_t Send_Data);               // 发送一字节数据
    bool (*IIC_Rece_Data)(uint8_t *Rece_Buf, bool IS_NACK); // 接收一字节数据
    bool (*IIC_Generate_Stop)(void);                        // 产生停止位
    void (*DMA_Start)(void);                                // 开启DMA
    void (*Delay_ms)(uint32_t Ms);                          // 毫秒延迟
    void (*Err_CallBack)(AK09911C_Driver_t *Drv);           // 错误回调函数
};
/*------------------------------------ A K 0 9 9 1 1 C --------------------------------------*/
uint8_t Ak09911C_GetID(AK09911C_Driver_t *Driver);

bool AK09911C_W_Reg(AK09911C_Driver_t *Driver, uint8_t reg_adress, uint8_t Data);                 // 写寄存器
bool AK09911C_R_Regs(AK09911C_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size); // 读寄存器

void AK09911C_Default_Init(AK09911C_Driver_t *Driver, MODE_SEL Mode); // 默认初始化
void AK09911C_SofaReset(AK09911C_Driver_t *Driver);                   // 软件复位
void AK09911C_Mode_Sel(AK09911C_Driver_t *Driver, MODE_SEL Mode);     // 模式选择

bool AK09911C_UpData(AK09911C_Driver_t *Driver);                                    // 更新数据
bool AK09911C_UpData_DMA(AK09911C_Driver_t *Driver);                                // DMA更新数据
void AK09911C_Data_Proc(AK09911C_Driver_t *Driver, float Pitch_Deg, float Roll_Deg); // 数据处理
// uint16_t AK09911C_Cal(AK09911C_Driver_t *Driver, uint16_t num_samples, uint16_t Delay_time_ms);
#endif