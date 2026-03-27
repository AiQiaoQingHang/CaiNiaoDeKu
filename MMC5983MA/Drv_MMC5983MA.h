
#ifndef DRV_MMC5983MA_H
#define DRV_MMC5983MA_H
#include <stdint.h>
#include <stdbool.h>
#include "Drv_MMC5983MA_define.h"
/* 位操作 */
#define BIT_SET(Value, Bit_Mask)   ((Value) |= Bit_Mask)  // Bit置 1
#define BIT_RESET(Value, Bit_Mask) ((Value) &= ~Bit_Mask) // Bit置 0
#define BIT_CHECK(Value, Bit_Mask) ((Value) & Bit_Mask)   // Bit检查
/* 角弧转换 */
#define DEG_TO_RAD (0.01745f)
#define RAD_TO_DEG (57.2958f) // 弧度转角度系数
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
typedef enum {
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

/* 输出数据 */
typedef struct
{
    float Yaw;                            // 磁力计偏航角
    int32_t X_Offset, Y_Offset, Z_Offset; // 原点偏移
    int32_t X_mG, Y_mG, Z_mG;             // 单位转换后的值
} MMC_Value_t;

/* 原始数据 */
typedef union __attribute__((packed)) {
    uint8_t Buf[8];
    struct Pack {
        uint16_t Xout_17_10 : 8;
        uint16_t Xout_9_2 : 8;
        uint16_t Yout_17_10 : 8;
        uint16_t Yout_9_2 : 8;
        uint16_t Zout_17_10 : 8;
        uint16_t Zout_9_2 : 8;
        uint8_t Reserved : 2;
        uint8_t Zout_1_0 : 2;
        uint8_t Yout_1_0 : 2;
        uint8_t Xout_1_0 : 2;
        uint8_t Temperature;
    } bits;
} MMC_RawData_t;
/* 数据长度 */
#define MMC_M_DATALEN_16BIT (6)
#define MMC_M_DATALEN_18BIT (7)
#define MMC_T_DATALEN       (1)
/* 组合16位数据 */
#define MMC_GET_X_16BIT(RawData) \
    ((uint16_t)((RawData).bits.Xout_17_10) << 8 | (uint16_t)((RawData).bits.Xout_9_2))
#define MMC_GET_Y_16BIT(RawData) \
    ((uint16_t)((RawData).bits.Yout_17_10) << 8 | (uint16_t)((RawData).bits.Yout_9_2))
#define MMC_GET_Z_16BIT(RawData) \
    ((uint16_t)((RawData).bits.Zout_17_10) << 8 | (uint16_t)((RawData).bits.Zout_9_2))
/* 组合18位数据 */
#define MMC_GET_X_18BIT(RawData)                   \
    ((uint32_t)((RawData).bits.Xout_17_10) << 10 | \
     (uint32_t)((RawData).bits.Xout_9_2) << 2 |    \
     (uint32_t)((RawData).bits.Xout_1_0))
#define MMC_GET_Y_18BIT(RawData)                   \
    ((uint32_t)((RawData).bits.Yout_17_10) << 10 | \
     (uint32_t)((RawData).bits.Yout_9_2) << 2 |    \
     (uint32_t)((RawData).bits.Yout_1_0))
#define MMC_GET_Z_18BIT(RawData)                   \
    ((uint32_t)((RawData).bits.Zout_17_10) << 10 | \
     (uint32_t)((RawData).bits.Zout_9_2) << 2 |    \
     (uint32_t)((RawData).bits.Zout_1_0))
/* 单位转换 LSB >> mG */
#define MMC_16BIT_TO_MG(raw_16bit) \
    (((int32_t)(raw_16bit) - 32768) * 25 / 100) // 32768对应0G，0.25mG/LSB
#define MMC_18BIT_TO_MG(raw_18bit) \
    (((int32_t)(raw_18bit) - 131072) * 625 / 10000) // 131072对应0G，0.0625mG/LSB
/* 驱动实例 */
struct MMC_Driver;
typedef struct MMC_Driver MMC_Driver_t;
struct MMC_Driver {
    MMC_Value_t MMC_Value;                                  // MMC解析后数据
    MMC_RawData_t MMC_RawData;                              // MMC原始数据
    uint8_t Dev_Addr;                                       // IIC设备地址
    IIC_Err_Flag Err_Flag;                                  // IIC错误标志位
    uint8_t Err_Count;                                      // IIC错误计数
    bool (*IIC_Generate_Start)(void);                       // 产生起始条件
    bool (*IIC_Send_Dev_Addr)(uint8_t Addr, bool IS_Write); // 发送设备地址
    bool (*IIC_Send_Data)(uint8_t Send_Data);               // 发送一字节数据
    bool (*IIC_Rece_Data)(uint8_t *Rece_Buf, bool IS_NACK); // 接收一字节数据
    bool (*IIC_Generate_Stop)(void);                        // 产生停止位
    // void (*DMA_Start)(void);                                // 开启DMA
    void (*Delay_ms)(uint32_t Ms);           // 毫秒延迟
    void (*Err_CallBack)(MMC_Driver_t *Drv); // 错误回调函数
};
/* 基础函数 */
bool MMC_W_Reg(MMC_Driver_t *Driver, uint8_t reg_adress, uint8_t Data);                 // 写MMC5983MA寄存器
bool MMC_R_Regs(MMC_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size); // 读MMC5983MA寄存器
/* 框架提供函数 */
bool MMC_DefaultInit_Single(MMC_Driver_t *Driver);                                                // 单次测量默认初始化
bool MMC_DefaultInit_Continues(MMC_Driver_t *Driver);                                             // 连续测量默认初始化
bool MMC_M_DataUp(MMC_Driver_t *Driver, MMC_DataLen_Mode DataLen, uint8_t RetryCnt);              // 更新磁力数据
bool MMC_T_DataUp(MMC_Driver_t *Driver, uint8_t RetryCnt);                                        // 更新温度数据
void MMC_Data_Proc(MMC_Driver_t *Driver, MMC_DataLen_Mode DataLen);                               // 数据处理
void MMC_Calib(MMC_Driver_t *Driver, MMC_DataLen_Mode DataLen, uint8_t RetryCnt, uint16_t Count); // 校准
/* 检查状态 */
uint8_t MMC_GetID(MMC_Driver_t *Driver);                                      // 获取ID
bool MMC_Check_Status_Done(MMC_Driver_t *Driver, MMC_Reg_Status CheckStatus); // 检查状态
/* 测量 */
bool MMC_StartSingle_M_Meas(MMC_Driver_t *Driver); // 启动一次磁场测量
bool MMC_StartSingle_T_Meas(MMC_Driver_t *Driver); // 启动一次温度测量
void MMC_StartSingle_Set(MMC_Driver_t *Driver);    // 启动一次Set操作
void MMC_StartSingle_Reset(MMC_Driver_t *Driver);  // 启动一次Reset操作
/* 配置 */
void MMC_INT_EN(MMC_Driver_t *Driver, bool EN_Status);         // 使能测量中断
void MMC_AutoSR_EN(MMC_Driver_t *Driver, bool EN_Status);      // 使能自动Set/Reset
void MMC_OTPRead(MMC_Driver_t *Driver);                        // OTP读取
void MMC_SetMeasTime(MMC_Driver_t *Driver, BWCfg MeasTime);    // 设置采样间隔
void MMC_X_Inhibit_EN(MMC_Driver_t *Driver, bool EN_Status);   // 使能 X 通道
void MMC_YZ_Inhibit_EN(MMC_Driver_t *Driver, bool EN_Status);  // 使能 YZ 通道
void MMC_SoftReset(MMC_Driver_t *Driver);                      // 软件复位
void MMC_SetCm_freq(MMC_Driver_t *Driver, Cm_freqCfg Cm_freq); // 设置连续模式下测量频率
bool MMC_Cmm_EN(MMC_Driver_t *Driver, bool EN_Status);         // 使能连续模式
void MMC_SetPrd(MMC_Driver_t *Driver, Prd_setCfg Prd_set);     // 设置自动SET频率
void MMC_Prd_EN(MMC_Driver_t *Driver, bool EN_Status);         // 使能连续模式
void MMC_St_enp(MMC_Driver_t *Driver, bool EN_Status);         //
void MMC_St_enm(MMC_Driver_t *Driver, bool EN_Status);         //
void MMC_SPI3W_EN(MMC_Driver_t *Driver, bool EN_Status);       //
#endif