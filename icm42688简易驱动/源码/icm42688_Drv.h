#ifndef icm42688_DRV_H
#define icm42688_DRV_H

#include <stdint.h>
#include <stdbool.h>
#include "icm42688_Drv_define.h" //引用寄存器地址映射宏定义

#define ICM_TEMP_DATA_SIZE_8BIT 2                                                                             // 温度计数据大小【单位：字节】
#define ICM_ACCEL_DATA_SIZE_8BIT 6                                                                            // 加速度计数据大小【单位：字节】
#define ICM_GYRO_DATA_SIZE_8BIT 6                                                                             // 陀螺仪数据大小【单位：字节】
#define ICM_ALL_DATA_SIZE_8BIT (ICM_TEMP_DATA_SIZE_8BIT + ICM_ACCEL_DATA_SIZE_8BIT + ICM_GYRO_DATA_SIZE_8BIT) // 全部数据大小

#define ICM_TEMP_DATA_SIZE_16BIT (ICM_TEMP_DATA_SIZE_8BIT / 2)   // 温度计数据大小【单位：半字】
#define ICM_ACCEL_DATA_SIZE_16BIT (ICM_ACCEL_DATA_SIZE_8BIT / 2) // 加速度计数据大小【单位：半字】
#define ICM_GYRO_DATA_SIZE_16BIT (ICM_GYRO_DATA_SIZE_8BIT / 2)   // 陀螺仪数据大小【单位：半字】
#define ICM_ALL_DATA_SIZE_16BIT (ICM_ALL_DATA_SIZE_8BIT / 2)     // 全部数据大小【单位：半字】

#define ICM_TEMP_DATA_OFFSET_8BIT 0  // 温度计数据在驱动结构体内的数据数组偏移【单位：字节】
#define ICM_ACCEL_DATA_OFFSET_8BIT 2 // 加速度计数据在驱动结构体内的数据数组偏移【单位：字节】
#define ICM_GYRO_DATA_OFFSET_8BIT 8  // 陀螺仪数据在驱动结构体内的数据数组偏移【单位：字节】

#define ICM_TEMP_DATA_OFFSET_16BIT 0                                 // 温度计数据在驱动结构体内的数据数组偏移【单位：半字】
#define ICM_ACCEL_DATA_OFFSET_16BIT (ICM_ACCEL_DATA_OFFSET_8BIT / 2) // 加速度计数据在驱动结构体内的数据数组偏移【单位：半字】
#define ICM_GYRO_DATA_OFFSET_16BIT (ICM_GYRO_DATA_OFFSET_8BIT / 2)   // 陀螺仪数据在驱动结构体内的数据数组偏移【单位：半字】

#define RAD_TO_ANGLE 57.2958f   // 弧度转角度系数

#define ERR_CHECK(Driver_Command, IIC_Err)    \
    do                                        \
    {                                         \
        if (!(Driver_Command))                \
        {                                     \
            Driver->Err_Flag = (IIC_Err);     \
            Driver->IIC_Generate_Stop();      \
            if (Driver->Err_CallBack)         \
            {                                 \
                Driver->Err_CallBack(Driver); \
            }                                 \
            return false;                     \
        }                                     \
    } while (0)

#define BYTESWAP_16BIT(data) ((int16_t)((((data) & 0x00FF) << 8) | (((data) & 0xFF00) >> 8)))
#define BYTESWAP_ARRAY_16BIT(Arr_Ptr_16Bit, Arr_Size_16Bit) \
    do                                                      \
    {                                                       \
        int16_t *Arr = (int16_t *)Arr_Ptr_16Bit;            \
        for (uint8_t i = 0; i < (Arr_Size_16Bit); i++)      \
        {                                                   \
            (Arr)[i] = BYTESWAP_16BIT((Arr)[i]);            \
        }                                                   \
    } while (0)

/* IIC错误枚举 */
typedef enum
{
    None_Err,              // 没有错误
    Generate_Start_Err,    // 起始位产生错误
    RE_Generate_Start_Err, // 重复发送起始位错误
    Send_Dev_Addr_Err,     // 设备地址发送错误
    RE_Send_Dev_Addr_Err,  // 第二次寻址错误
    Send_Reg_Addr_Err,     // 寄存器地址发送错误
    Send_Data_Err,         // 发送数据错误
    Rece_Data_Err,         // 接收数据错误
    Generate_Stop_Err,     // 停止位产生错误
} IIC_Err_Flag;

/* 芯片数据结构体 */
typedef struct
{
    int16_t Pre_GYRO[3];               // 上一次陀螺仪原始数据
    int16_t Pre_ACCEL[3];              // 上一次加速度计原始数据
    int16_t GYRO_Bias[3];              // 陀螺仪零偏校准
    float Yaw, Pitch, Roll;            // 欧拉角
    volatile uint8_t icm_Data_Buf[14]; // 数据缓存区
} ICM42688_Value_t;

/* 数据处理参数结构体 */
typedef struct
{
    float Ts;              // 采样周期
    float Fil_Power;       // 互补滤波系数
    float GYRO_Alone_Fil;  // 陀螺仪单独滤波系数
    float ACCEL_Alone_Fil; // 加速度计单独滤波系数
} ICM42688_Para_t;

struct ICM42688_Driver;
typedef struct ICM42688_Driver ICM42688_Driver_t;
/* 芯片控制结构体 */
struct ICM42688_Driver
{
    ICM42688_Value_t ICM_Value;                             // ICM数据
    ICM42688_Para_t ICM_Para;                               // ICM滤波系数
    IIC_Err_Flag Err_Flag;                                  // 错误标志位
    uint16_t Err_Count;                                     // 错误计数
    uint8_t Dev_Addr;                                       // 设备地址（原始地址）
    bool (*IIC_Generate_Start)(void);                       // 产生起始条件
    bool (*IIC_Send_Dev_Addr)(uint8_t Addr, bool IS_Write); // 发送设备地址
    bool (*IIC_Send_Data)(uint8_t Send_Data);               // 发送一字节数据
    bool (*IIC_Rece_Data)(uint8_t *Rece_Buf, bool IS_NACK); // 接收一字节数据
    bool (*IIC_Generate_Stop)(void);                        // 产生停止位
    void (*DMA_Start)(void);                                // DMA传输开始
    void (*Delay_ms)(uint32_t ms);                          // 毫秒延迟
    void (*Err_CallBack)(ICM42688_Driver_t *Drv);           // 错误回调函数
};

/*----------------------------------------- I C M 4 2 6 8 8 --------------------------------------*/
void icm42688_Default_Init(ICM42688_Driver_t *Driver);                                            // 默认初始化
bool icm42688_W_Reg(ICM42688_Driver_t *Driver, uint8_t reg_adress, uint8_t Data);                 // 写寄存器
bool icm42688_R_Regs(ICM42688_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size); // 读寄存器void icm42688_Cal(uint16_t num_samples);                                  // 校准
bool icm42688_GetID(ICM42688_Driver_t *Driver, uint8_t *ID_Buf);                                  // 获取芯片ID
bool icm42688_UpTEMP(ICM42688_Driver_t *Driver);                                                  // 更新温度计数据
bool icm42688_UpACCEL(ICM42688_Driver_t *Driver, bool Fil_EN);                                    // 更新加速度计数据
bool icm42688_UpGYRO(ICM42688_Driver_t *Driver, bool Fil_EN);                                     // 更新陀螺仪数据
bool icm42688_UpAllData(ICM42688_Driver_t *Driver, bool Fil_EN);                                  // 更新全部数据
void icm42688_ACCEL_GYRO_Fil(ICM42688_Driver_t *Driver);                                          // 陀螺仪和加速度数据滤波
bool icm42688_DMA_UPData(ICM42688_Driver_t *Driver);                                              // 使用DMA更新全部数据
uint16_t icm42688_Cal(ICM42688_Driver_t *Driver, uint16_t num_samples, uint16_t Delay_time_ms);   // 平均值校准
void icm42688_Slove_Eular(ICM42688_Driver_t *Driver);                                             // 解算欧拉角
#endif
