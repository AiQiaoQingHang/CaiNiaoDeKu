#ifndef IIC_Dev_H
#define IIC_Dev_H

#include <stdint.h>
#include <stdbool.h>

#define Err_Check(Driver_Command, IIC_Err)    \
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

/* 芯片控制结构体 */
typedef struct
{
    IIC_Err_Flag Err_Flag;                                  // 错误标志位
    uint16_t Err_Count;                                     // 错误计数
    uint8_t Dev_Addr;                                       // 设备地址,未左移 1 位
    bool (*IIC_Generate_Start)(void);                       // 产生起始条件
    bool (*IIC_Send_Dev_Addr)(uint8_t Addr, bool IS_Write); // 发送设备地址
    bool (*IIC_Send_Data)(uint8_t Send_Data);               // 发送一字节数据
    bool (*IIC_Rece_Data)(uint8_t *Rece_Buf, bool IS_NACK); // 接收一字节数据
    bool (*IIC_Generate_Stop)(void);                        // 产生停止位
    void (*Err_CallBack)(IIC_Err_Flag Err);                 // 错误回调函数
} IIC_Dev_Driver_t;

bool IIC_Dev_W_Reg(IIC_Dev_Driver_t *Driver, uint8_t reg_adress, uint8_t Data);                 // 写寄存器
bool IIC_Dev_R_Regs(IIC_Dev_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size); // 读寄存器
#endif