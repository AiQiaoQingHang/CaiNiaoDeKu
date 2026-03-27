#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "Drv_MMC5983MA.h"
#include "Drv_MMC5983MA_define.h"

/**
 * @brief 写MMC5983MA寄存器
 *
 * @param Driver 驱动实例
 * @param reg_adress 寄存器地址
 * @param Data 要写入的数据
 * @return true 成功
 * @return false 失败
 */
bool MMC_W_Reg(MMC_Driver_t *Driver, uint8_t reg_adress, uint8_t Data)
{
    /*发送起始位*/
    ERR_CHECK(Driver->IIC_Generate_Start(), Generate_Start_Err);
    /*发送七位地址+写*/
    ERR_CHECK((Driver->IIC_Send_Dev_Addr((uint8_t)(Driver->Dev_Addr << 1), true)), Send_Dev_Addr_Err); // 自动补零
    // 发送寄存器地址
    ERR_CHECK(Driver->IIC_Send_Data(reg_adress), Send_Reg_Addr_Err);
    /*发送要写入的数据*/
    ERR_CHECK(Driver->IIC_Send_Data(Data), Send_Data_Err);
    /*发送停止位*/
    ERR_CHECK(Driver->IIC_Generate_Stop(), Generate_Stop_Err);
    return true;
}
/**
 * @brief 读MMC5983MA寄存器
 *
 * @param Driver 驱动实例
 * @param reg_adress 寄存器地址
 * @param Rece 接收缓存区
 * @param size 读取字节数
 * @return true 成功
 * @return false 错误
 */
bool MMC_R_Regs(MMC_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size)
{
    /*发送起始位*/
    ERR_CHECK(Driver->IIC_Generate_Start(), Generate_Start_Err);
    /*发送七位地址+写*/
    ERR_CHECK((Driver->IIC_Send_Dev_Addr((uint8_t)(Driver->Dev_Addr << 1), true)), Send_Dev_Addr_Err); // 自动补零
    /*发送寄存器地址*/
    ERR_CHECK(Driver->IIC_Send_Data(reg_adress), Send_Reg_Addr_Err);
    /*重复发送起始位*/
    ERR_CHECK(Driver->IIC_Generate_Start(), RE_Generate_Start_Err);
    /*发送七位地址+读*/
    ERR_CHECK(Driver->IIC_Send_Dev_Addr((uint8_t)((Driver->Dev_Addr << 1) | 0x01), false), RE_Send_Dev_Addr_Err);
    /*循环接收数据*/
    for (uint8_t i = 0; i < size - 1; i++) // 空出最后一字节
    {
        ERR_CHECK(Driver->IIC_Rece_Data(&Rece[i], false), Rece_Data_Err);
    }
    ERR_CHECK(Driver->IIC_Rece_Data(&Rece[size - 1], true), Rece_Data_Err); // 最后一字节单独处理
    /*发送停止位*/
    ERR_CHECK(Driver->IIC_Generate_Stop(), Generate_Stop_Err);
    return true;
}
/**
 * @brief 获取ID
 *
 * @param Driver 驱动实例
 * @return uint8_t 公司ID 0x30
 */
uint8_t MMC_GetID(MMC_Driver_t *Driver)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_PRODUCT_ID, &temp, 1);
    return temp;
}

/**
 * @brief 检查状态
 * @note 不可位操作
 * @param Driver 驱动实例
 * @param CheckStatus 待检查标志位
 * @return true 成功
 * @return false 失败
 */
bool MMC_Check_Status_Done(MMC_Driver_t *Driver, MMC_Reg_Status CheckStatus)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_STATUS, &temp, 1);
    return (temp & CheckStatus) ? true : false;
}

/**
 * @brief 启动一次磁场测量
 *
 * @param Driver 驱动实例
 * @return true 成功
 * @return false 失败
 */
bool MMC_StartSingle_M_Meas(MMC_Driver_t *Driver)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_0, &temp, 1);
    if (BIT_CHECK(temp, TM_T)) // 不可同时为高
    {
        return false;
    } else {
        BIT_SET(temp, TM_M);
        MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_0, temp);
    }
    return true;
}

/**
 * @brief 启动一次温度测量
 *
 * @param Driver 驱动实例
 * @return true 成功
 * @return false 失败
 */
bool MMC_StartSingle_T_Meas(MMC_Driver_t *Driver)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_0, &temp, 1);
    if (BIT_CHECK(temp, TM_M)) // 不可同时为高
    {
        return false;
    } else {
        BIT_SET(temp, TM_T);
        MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_0, temp);
    }
    return true;
}

/**
 * @brief 使能测量中断
 *
 * @param Driver 驱动实例
 * @param EN_Status 使能状态
 */
void MMC_INT_EN(MMC_Driver_t *Driver, bool EN_Status)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_0, &temp, 1);
    if (EN_Status)
        BIT_SET(temp, INT_meas_done_en);
    else
        BIT_RESET(temp, INT_meas_done_en);
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_0, temp);
}

/**
 * @brief 启动一次Set操作
 *
 * @param Driver 驱动实例
 */
void MMC_StartSingle_Set(MMC_Driver_t *Driver)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_0, &temp, 1);
    BIT_SET(temp, Set);
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_0, temp);
    if (Driver->Delay_ms) {
        Driver->Delay_ms(1);
    }
}

/**
 * @brief 启动一次Reset操作
 *
 * @note 不是复位操作，是芯片特殊功能
 * @param Driver 驱动实例
 */
void MMC_StartSingle_Reset(MMC_Driver_t *Driver)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_0, &temp, 1);
    BIT_SET(temp, Reset);
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_0, temp);
    if (Driver->Delay_ms) {
        Driver->Delay_ms(1);
    }
}

/**
 * @brief 使能自动Set/Reset
 *
 * @param Driver 驱动实例
 * @param EN_Status 使能状态
 */
void MMC_AutoSR_EN(MMC_Driver_t *Driver, bool EN_Status)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_0, &temp, 1);
    if (EN_Status)
        BIT_SET(temp, Auto_SR_en);
    else
        BIT_RESET(temp, Auto_SR_en);
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_0, temp);
}

/**
 * @brief OTP读取
 *
 * @param Driver 驱动实例
 */
void MMC_OTPRead(MMC_Driver_t *Driver)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_0, &temp, 1);
    BIT_SET(temp, OTPRead);
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_0, temp);
}

/**
 * @brief 设置采样间隔
 *
 * @param Driver 驱动实例
 * @param MeasTime 采样时间
 */
void MMC_SetMeasTime(MMC_Driver_t *Driver, BWCfg MeasTime)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_1, &temp, 1);
    BIT_RESET(temp, MeasTime_0p5ms); // 清零配置位
    BIT_SET(temp, MeasTime);
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_1, temp);
}

/**
 * @brief 使能 X 通道
 *
 * @param Driver 驱动实例
 * @param EN_Status 使能状态
 */
void MMC_X_Inhibit_EN(MMC_Driver_t *Driver, bool EN_Status)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_1, &temp, 1);
    if (EN_Status) {
        BIT_RESET(temp, X_inhibit);
    } else {
        BIT_SET(temp, X_inhibit);
    }
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_1, temp);
}

/**
 * @brief 使能 YZ 通道
 *
 * @param Driver 驱动实例
 * @param EN_Status 使能状态
 */
void MMC_YZ_Inhibit_EN(MMC_Driver_t *Driver, bool EN_Status)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_1, &temp, 1);
    if (EN_Status) {
        BIT_RESET(temp, YZ_inhibit);
    } else {
        BIT_SET(temp, YZ_inhibit);
    }
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_1, temp);
}

/**
 * @brief 软件复位
 *
 * @param Driver 驱动实例
 */
void MMC_SoftReset(MMC_Driver_t *Driver)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_1, &temp, 1);
    BIT_SET(temp, SW_RST);
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_1, temp);
    if (Driver->Delay_ms) {
        Driver->Delay_ms(10);
    }
}

/**
 * @brief 设置连续模式下测量频率
 *
 * @param Driver
 * @param Cm_freq
 */
void MMC_SetCm_freq(MMC_Driver_t *Driver, Cm_freqCfg Cm_freq)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_2, &temp, 1);
    BIT_RESET(temp, Cm_freq_1000Hz); // 清零配置位
    BIT_SET(temp, Cm_freq);
    if (Cm_freq == Cm_freq_200Hz) {
        MMC_SetMeasTime(Driver, MeasTime_4ms);
    }
    if (Cm_freq == Cm_freq_1000Hz) {
        MMC_SetMeasTime(Driver, MeasTime_0p5ms);
    }
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_2, temp);
}

/**
 * @brief 使能连续模式
 *
 * @param Driver 驱动实例
 * @param EN_Status 使能状态
 * @return true 成功
 * @return false 失败
 */
bool MMC_Cmm_EN(MMC_Driver_t *Driver, bool EN_Status)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_2, &temp, 1);
    if (!BIT_CHECK(temp, Cm_freq_1000Hz)) // 配置位全为 0
        return false;
    if (EN_Status) {
        BIT_SET(temp, Cmm_en);
    } else {
        BIT_RESET(temp, Cmm_en);
    }
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_2, temp);
    return true;
}

/**
 * @brief 设置自动SET频率
 *
 * @param Driver 驱动实例
 * @param Cm_freq
 */
void MMC_SetPrd(MMC_Driver_t *Driver, Prd_setCfg Prd_set)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_2, &temp, 1);
    BIT_RESET(temp, Prd_set_2000Cnt); // 清零配置位
    BIT_SET(temp, Prd_set);
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_2, temp);
}

/**
 * @brief 使能连续模式
 *
 * @param Driver 驱动实例
 * @param EN_Status 使能状态
 */
void MMC_Prd_EN(MMC_Driver_t *Driver, bool EN_Status)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_2, &temp, 1);
    if (EN_Status) {
        BIT_SET(temp, En_prd_set);
    } else {
        BIT_RESET(temp, En_prd_set);
    }
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_2, temp);
}

/**
 * @brief
 *
 * @param Driver
 * @param EN_Status
 */
void MMC_St_enp(MMC_Driver_t *Driver, bool EN_Status)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_3, &temp, 1);
    if (EN_Status) {
        BIT_SET(temp, St_enp);
    } else {
        BIT_RESET(temp, St_enp);
    }
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_3, temp);
}

/**
 * @brief
 *
 * @param Driver
 * @param EN_Status
 */
void MMC_St_enm(MMC_Driver_t *Driver, bool EN_Status)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_3, &temp, 1);
    if (EN_Status) {
        BIT_SET(temp, St_enm);
    } else {
        BIT_RESET(temp, St_enm);
    }
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_3, temp);
}

/**
 * @brief MMC 3线SPI使能
 *
 * @param Driver 驱动实例
 * @param EN_Status 使能状态
 */
void MMC_SPI3W_EN(MMC_Driver_t *Driver, bool EN_Status)
{
    uint8_t temp = 0;
    MMC_R_Regs(Driver, MMC_REGADDR_INTERAL_CTRL_3, &temp, 1);
    if (EN_Status) {
        BIT_SET(temp, Spi_3W);
    } else {
        BIT_RESET(temp, Spi_3W);
    }
    MMC_W_Reg(Driver, MMC_REGADDR_INTERAL_CTRL_3, temp);
}

/**
 * @brief 更新磁力数据
 *
 * @param Driver 驱动实例
 * @param DataLen 数据长度16/18
 * @param RetryCnt 重试次数
 * @return true 成功
 * @return false 失败
 */
bool MMC_M_DataUp(MMC_Driver_t *Driver, MMC_DataLen_Mode DataLen, uint8_t RetryCnt)
{
    while (RetryCnt--) {
        if (MMC_Check_Status_Done(Driver, Meas_M_Done))
            break;
        if (RetryCnt == 0)
            return false;
    }
    if (DataLen == DataLenMode_16Bit)
        MMC_R_Regs(Driver, MMC_REGADDR_XOUT_0, Driver->MMC_RawData.Buf, MMC_M_DATALEN_16BIT);
    else
        MMC_R_Regs(Driver, MMC_REGADDR_XOUT_0, Driver->MMC_RawData.Buf, MMC_M_DATALEN_18BIT);
    return true;
}

/**
 * @brief 更新温度数据
 *
 * @param Driver 驱动实例
 * @param RetryCnt 重试次数
 * @return true 成功
 * @return false 失败
 */
bool MMC_T_DataUp(MMC_Driver_t *Driver, uint8_t RetryCnt)
{
    while (RetryCnt--) {
        if (MMC_Check_Status_Done(Driver, Meas_T_Done))
            break;
        if (RetryCnt == 0)
            return false;
    }
    MMC_R_Regs(Driver, MMC_REGADDR_TOUT, Driver->MMC_RawData.Buf, MMC_T_DATALEN);
    return true;
}

/**
 * @brief 数据处理
 *
 * @param Driver 驱动实例
 * @param DataLen 数据长度16/18
 */
void MMC_Data_Proc(MMC_Driver_t *Driver, MMC_DataLen_Mode DataLen)
{
    /* 获取数据 */
    if (DataLen == MMC_M_DATALEN_16BIT) {
        uint16_t X_Data, Y_Data, Z_Data;
        X_Data                 = MMC_GET_X_16BIT(Driver->MMC_RawData);
        Y_Data                 = MMC_GET_Y_16BIT(Driver->MMC_RawData);
        Z_Data                 = MMC_GET_Z_16BIT(Driver->MMC_RawData);
        Driver->MMC_Value.X_mG = MMC_16BIT_TO_MG(X_Data);
        Driver->MMC_Value.Y_mG = MMC_16BIT_TO_MG(Y_Data);
        Driver->MMC_Value.Z_mG = MMC_16BIT_TO_MG(Z_Data);
    } else {
        uint32_t X_Data, Y_Data, Z_Data;
        X_Data                 = MMC_GET_X_18BIT(Driver->MMC_RawData);
        Y_Data                 = MMC_GET_Y_18BIT(Driver->MMC_RawData);
        Z_Data                 = MMC_GET_Z_18BIT(Driver->MMC_RawData);
        Driver->MMC_Value.X_mG = MMC_18BIT_TO_MG(X_Data);
        Driver->MMC_Value.Y_mG = MMC_18BIT_TO_MG(Y_Data);
        Driver->MMC_Value.Z_mG = MMC_18BIT_TO_MG(Z_Data);
    }
    float Yaw = atan2f(Driver->MMC_Value.Z_mG - Driver->MMC_Value.Z_Offset, Driver->MMC_Value.Y_mG - Driver->MMC_Value.Y_Offset) * RAD_TO_DEG;
    if (Yaw < 0) {
        Yaw += 360.0f;
    }
    Driver->MMC_Value.Yaw = Yaw;
}

/**
 * @brief 单次测量默认初始化
 *
 * @param Driver 驱动实例
 * @return true 成功
 * @return false 失败
 */
bool MMC_DefaultInit_Single(MMC_Driver_t *Driver)
{
    if (MMC_GetID(Driver) != MMC_PRODUCT_ID)
        return false;
    MMC_SoftReset(Driver);                 // 软件复位
    MMC_StartSingle_Set(Driver);           // 开始一次Set操作
    MMC_StartSingle_Reset(Driver);         // 开始一次Reset操作
    MMC_SetMeasTime(Driver, MeasTime_4ms); // 采样时间4ms
    MMC_AutoSR_EN(Driver, true);           // 自动Set
    MMC_X_Inhibit_EN(Driver, true);        // 使能 X 通道
    MMC_YZ_Inhibit_EN(Driver, true);       // 使能 YZ 通道
    return true;
}

/**
 * @brief 连续测量默认初始化
 *
 * @param Driver 驱动实例
 * @return true 成功
 * @return false 失败
 */
bool MMC_DefaultInit_Continues(MMC_Driver_t *Driver)
{
    if (!MMC_DefaultInit_Single(Driver))
        return false;
    MMC_SetCm_freq(Driver, Cm_freq_100Hz); // 设置采样频率
    if (!MMC_Cmm_EN(Driver, true)) {       // 内部已检查CM_Freq合法性
        return false;
    }
    return true;
}

/**
 * @brief 校准
 * 
 * @param Driver 驱动实例
 * @param DataLen 数据长度16/18
 * @param RetryCnt 更新数据重试次数
 * @param Count 采样次数
 */
void MMC_Calib(MMC_Driver_t *Driver, MMC_DataLen_Mode DataLen, uint8_t RetryCnt, uint16_t Count)
{
    int32_t X_Max, X_Min, Y_Max, Y_Min, Z_Max, Z_Min, X_Rawmg, Y_Rawmg, Z_Rawmg;
    X_Max = -99999;
    Y_Max = -99999;
    Z_Max = -99999;

    X_Min = 99999;
    Y_Min = 99999;
    Z_Min = 99999;
    uint32_t X_Data, Y_Data, Z_Data;
    while (Count--) {
        if (MMC_M_DataUp(Driver, DataLen, RetryCnt)) {
            if (DataLen == MMC_M_DATALEN_16BIT) {
                X_Data  = MMC_GET_X_16BIT(Driver->MMC_RawData);
                Y_Data  = MMC_GET_Y_16BIT(Driver->MMC_RawData);
                Z_Data  = MMC_GET_Z_16BIT(Driver->MMC_RawData);
                X_Rawmg = MMC_16BIT_TO_MG(X_Data);
                Y_Rawmg = MMC_16BIT_TO_MG(Y_Data);
                Z_Rawmg = MMC_16BIT_TO_MG(Z_Data);
            } else {
                X_Data  = MMC_GET_X_18BIT(Driver->MMC_RawData);
                Y_Data  = MMC_GET_Y_18BIT(Driver->MMC_RawData);
                Z_Data  = MMC_GET_Z_18BIT(Driver->MMC_RawData);
                X_Rawmg = MMC_18BIT_TO_MG(X_Data);
                Y_Rawmg = MMC_18BIT_TO_MG(Y_Data);
                Z_Rawmg = MMC_18BIT_TO_MG(Z_Data);
            }
            // 更新最大值
            if (X_Rawmg > X_Max) X_Max = X_Rawmg;
            if (Y_Rawmg > Y_Max) Y_Max = Y_Rawmg;
            if (Z_Rawmg > Z_Max) Z_Max = Z_Rawmg;

            // 更新最小值
            if (X_Rawmg < X_Min) X_Min = X_Rawmg;
            if (Y_Rawmg < Y_Min) Y_Min = Y_Rawmg;
            if (Z_Rawmg < Z_Min) Z_Min = Z_Rawmg;
        }
    }
    Driver->MMC_Value.X_Offset = (X_Max + X_Min) / 2;
    Driver->MMC_Value.Y_Offset = (Y_Max + Y_Min) / 2;
    Driver->MMC_Value.Z_Offset = (Z_Max + Z_Min) / 2;
}