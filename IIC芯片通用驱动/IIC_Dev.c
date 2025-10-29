#include <stdint.h>
#include "IIC_Dev.h"

/**
 * @brief 写IIC_Dev寄存器
 *
 * @param Driver 驱动结构体
 * @param reg_adress 寄存器地址
 * @param Data 要写入的数据
 * @return true 成功
 * @return false 失败
 */
bool IIC_Dev_W_Reg(IIC_Dev_Driver_t *Driver, uint8_t reg_adress, uint8_t Data)
{
    /*发送起始位*/
    Err_Check(Driver->IIC_Generate_Start(), Generate_Start_Err);
    /*发送七位地址+写*/
    Err_Check((Driver->IIC_Send_Dev_Addr((uint8_t)(Driver->Dev_Addr << 1), true)), Send_Dev_Addr_Err); // 自动补零
    // 发送寄存器地址
    Err_Check(Driver->IIC_Send_Data(reg_adress), Send_Reg_Addr_Err);
    /*发送要写入的数据*/
    Err_Check(Driver->IIC_Send_Data(Data), Send_Data_Err);
    /*发送停止位*/
    Err_Check(Driver->IIC_Generate_Stop(), Generate_Stop_Err);
    return true;
}

/**
 * @brief 读IIC_Dev寄存器
 *
 * @param Driver 驱动结构体
 * @param reg_adress 寄存器地址
 * @param Rece 接收缓存区
 * @param size 读取字节数
 * @return true 成功
 * @return false 错误
 */
bool IIC_Dev_R_Regs(IIC_Dev_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size)
{
    /*发送起始位*/
    Err_Check(Driver->IIC_Generate_Start(), Generate_Start_Err);
    /*发送七位地址+写*/
    Err_Check((Driver->IIC_Send_Dev_Addr((uint8_t)(Driver->Dev_Addr << 1), true)), Send_Dev_Addr_Err); // 自动补零
    /*发送寄存器地址*/
    Err_Check(Driver->IIC_Send_Data(reg_adress), Send_Reg_Addr_Err);
    /*重复发送起始位*/
    Err_Check(Driver->IIC_Generate_Start(), RE_Generate_Start_Err);
    /*发送七位地址+读*/
    Err_Check(Driver->IIC_Send_Dev_Addr((uint8_t)((Driver->Dev_Addr << 1) | 0x01), false), RE_Send_Dev_Addr_Err);
    /*循环接收数据*/
    for (uint8_t i = 0; i < size - 1; i++) // 空出最后一字节
    {
        Err_Check(Driver->IIC_Rece_Data(&Rece[i], false), Rece_Data_Err);
    }
    Err_Check(Driver->IIC_Rece_Data(&Rece[size - 1], true), Rece_Data_Err); // 最后一字节单独处理
    /*发送停止位*/
    Err_Check(Driver->IIC_Generate_Stop(), Generate_Stop_Err);
    return true;
}
