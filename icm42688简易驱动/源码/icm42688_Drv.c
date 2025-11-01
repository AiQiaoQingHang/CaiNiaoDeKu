#include <stdint.h>
#include "icm42688_Drv.h"
#include <math.h>
#include <string.h>

/********************************************************************
 *简	介：icm42688初始化
 *参	数：无
 *返回值：无
 *说	明：使用时调用
 *********************************************************************/
void icm42688_Default_Init(ICM42688_Driver_t *Driver)
{
    /*icm42688初始化*/
    icm42688_W_Reg(Driver, REG_BANK_SEL, 0x00);        // 寄存器组 0
    icm42688_W_Reg(Driver, GYRO_CONFIG_0, 0x06);       // 陀螺仪量程1000dbs，数据输出1Khz
    icm42688_W_Reg(Driver, ACCEL_CONFIG_0, 0x46);      // 加速度计量程+-4g，数据输出1Khz，更改量程需要更改宏定义加速度量程
    icm42688_W_Reg(Driver, GYRO_ACCEL_CONFIG_0, 0x33); // 设置芯片内数字滤波器
    icm42688_W_Reg(Driver, icm42688_PWR, 0x0F);        // 开启温度计，陀螺仪和加速度计低噪声模式
    Driver->Delay_ms(100);                             // 切换工作模式延迟
    icm42688_Cal(Driver, 20, 20);                      // 校准，只能调用一次，后面调用数据会出错
}

/**
 * @brief 写ICM寄存器
 *
 * @param Driver 驱动结构体
 * @param reg_adress 寄存器地址
 * @param Data 要写入的数据
 * @return true 成功
 * @return false 失败
 */
bool icm42688_W_Reg(ICM42688_Driver_t *Driver, uint8_t reg_adress, uint8_t Data)
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
 * @brief 读ICM寄存器
 *
 * @param Driver 驱动结构体
 * @param reg_adress 寄存器地址
 * @param Rece 接收缓存区
 * @param size 读取字节数
 * @return true 成功
 * @return false 错误
 */
bool icm42688_R_Regs(ICM42688_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size)
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
 * @brief 陀螺仪数据滤波
 *
 * @param Driver 驱动结构体
 * @param New_Data 新数据
 */
static void icm42688_GYRO_Fil(ICM42688_Driver_t *Driver, int16_t *New_Data)
{
    for (uint8_t i = 0; i < ICM_GYRO_DATA_SIZE_16BIT; i++)
    {
        New_Data[i] -= Driver->ICM_Value.GYRO_Bias[i]; // 减去零偏
        New_Data[i] = (New_Data[i] * Driver->ICM_Para.GYRO_Alone_Fil) +
                      ((1 - Driver->ICM_Para.GYRO_Alone_Fil) * Driver->ICM_Value.Pre_GYRO[i]);
        Driver->ICM_Value.Pre_GYRO[i] = New_Data[i]; // 保存滤波后的值
    }
}

/**
 * @brief 加速度计滤波
 *
 * @param Driver 驱动结构体
 * @param New_Data 新数据
 */
static void icm42688_ACCEL_Fil(ICM42688_Driver_t *Driver, int16_t *New_Data)
{
    for (uint8_t i = 0; i < ICM_ACCEL_DATA_SIZE_16BIT; i++)
    {
        New_Data[i] = (New_Data[i] * Driver->ICM_Para.ACCEL_Alone_Fil) +
                      ((1 - Driver->ICM_Para.ACCEL_Alone_Fil) * Driver->ICM_Value.Pre_ACCEL[i]);
        Driver->ICM_Value.Pre_ACCEL[i] = New_Data[i]; // 保存本次滤波后的值
    }
}

/**
 * @brief 读取芯片ID
 *
 * @param Driver 驱动结构体
 * @param ID 接收ID缓存区
 * @return true 成功
 * @return false 错误
 */
bool icm42688_GetID(ICM42688_Driver_t *Driver, uint8_t *ID_Buf)
{
    if (!icm42688_R_Regs(Driver, WHO_AM_I, ID_Buf, 1))
    {
        return false;
    }
    return true;
}

/**
 * @brief 更新温度计数据
 *
 * @param Driver 驱动结构体
 * @return true 成功
 * @return false 错误
 */
bool icm42688_UpTEMP(ICM42688_Driver_t *Driver)
{
    int16_t TEMP_Data[ICM_TEMP_DATA_SIZE_16BIT] = {0}; // 临时存储温度数据
    /* 从温度起始地址连续读2字节，读出全部温度数据 */
    if (!icm42688_R_Regs(Driver, icm42688_TEMP_OUT_H, (uint8_t *)TEMP_Data, ICM_TEMP_DATA_SIZE_8BIT))
    {
        return false;
    }
    /* 切换字节序 */
    BYTESWAP_ARRAY_16BIT(TEMP_Data, ICM_TEMP_DATA_SIZE_16BIT);
    memcpy((uint8_t *)&Driver->ICM_Value.icm_Data_Buf[ICM_TEMP_DATA_OFFSET_8BIT], (uint8_t *)&TEMP_Data, sizeof(TEMP_Data));
    return true;
}

/**
 * @brief 更新加速度计数据
 *
 * @param Driver 驱动结构体
 * @param Fil_EN 滤波需求
 * @return true 成功
 * @return false 错误
 */
bool icm42688_UpACCEL(ICM42688_Driver_t *Driver, bool Fil_EN)
{
    int16_t ACCEL_Data[ICM_ACCEL_DATA_SIZE_16BIT]; // 临时存储加速度数据（3个轴）
    // 从加速度计起始地址连续读6字节，读出全部加速度数据
    if (!icm42688_R_Regs(Driver, icm42688_ACCEL_XOUT_H, (uint8_t *)ACCEL_Data, ICM_ACCEL_DATA_SIZE_8BIT))
    {
        return false;
    }
    /* 切换字节序 */
    BYTESWAP_ARRAY_16BIT(ACCEL_Data, ICM_ACCEL_DATA_SIZE_16BIT);
    if (Fil_EN)
    {
        icm42688_ACCEL_Fil(Driver, ACCEL_Data);
    }
    else
    {
        for (uint8_t i = 0; i < ICM_ACCEL_DATA_SIZE_16BIT; i++)
        {
            Driver->ICM_Value.Pre_ACCEL[i] = ACCEL_Data[i]; // 保存值
        }
    }
    /* 将滤波后的值存入缓存区，使用memcpy防止对齐问题 */
    memcpy((uint8_t *)&Driver->ICM_Value.icm_Data_Buf[ICM_ACCEL_DATA_OFFSET_8BIT], (uint8_t *)ACCEL_Data, sizeof(ACCEL_Data));
    return true;
}

/**
 * @brief 更新陀螺仪数据
 *
 * @param Driver 驱动结构体
 * @param Fil_EN 滤波需求
 * @return true 成功
 * @return false 错误
 */
bool icm42688_UpGYRO(ICM42688_Driver_t *Driver, bool Fil_EN)
{
    int16_t GYRO_Data[ICM_GYRO_DATA_SIZE_16BIT] = {0}; // 临时存储3个轴的陀螺仪数据
    /* 从陀螺仪起始地址连续读6字节，读出全部陀螺仪数据 */
    if (!icm42688_R_Regs(Driver, icm42688_GYRO_XOUT_H, (uint8_t *)GYRO_Data, ICM_GYRO_DATA_SIZE_8BIT))
    {
        return false;
    }
    /* 切换字节序 */
    BYTESWAP_ARRAY_16BIT(GYRO_Data, ICM_GYRO_DATA_SIZE_16BIT);
    if (Fil_EN)
    {
        icm42688_GYRO_Fil(Driver, GYRO_Data);
    }
    else
    {
        for (uint8_t i = 0; i < ICM_GYRO_DATA_SIZE_16BIT; i++)
        {
            GYRO_Data[i] -= Driver->ICM_Value.GYRO_Bias[i]; // 减去零偏
            Driver->ICM_Value.Pre_GYRO[i] = GYRO_Data[i];   // 保存值
        }
    }
    /* 将滤波后的值存入缓存区，使用memcpy防止对齐问题 */
    memcpy((uint8_t *)&Driver->ICM_Value.icm_Data_Buf[ICM_GYRO_DATA_OFFSET_8BIT], (uint8_t *)GYRO_Data, sizeof(GYRO_Data));
    return true;
}

/**
 * @brief 更新全部数据
 *
 * @param Driver 驱动结构体
 * @param Fil_EN 滤波需求
 * @return true 成功
 * @return false 失败
 */
bool icm42688_UpAllData(ICM42688_Driver_t *Driver, bool Fil_EN)
{
    int16_t Data_Buf[ICM_ALL_DATA_SIZE_16BIT] = {0}; // 数据缓存区
    /* 从温度计起始地址连续读14字节，读出全部数据 */
    if (!icm42688_R_Regs(Driver, icm42688_TEMP_OUT_H, (uint8_t *)Data_Buf, ICM_ALL_DATA_SIZE_8BIT))
    {
        return false;
    }
    /* 切换字节序 */
    BYTESWAP_ARRAY_16BIT(Data_Buf, ICM_ALL_DATA_SIZE_16BIT);
    if (Fil_EN)
    {
        /* 【 0 温度】【 1 ~ 3 加速度计】【 4 ~ 6 陀螺仪】*/
        /* 加速度计滤波 */
        icm42688_ACCEL_Fil(Driver, &Data_Buf[ICM_ACCEL_DATA_OFFSET_16BIT]);
        /* 陀螺仪滤波 */
        icm42688_GYRO_Fil(Driver, &Data_Buf[ICM_GYRO_DATA_OFFSET_16BIT]);
    }
    else
    {
        /* 加速度计保存值*/
        for (uint8_t i = 0; i < ICM_ACCEL_DATA_SIZE_16BIT; i++)
        {
            Driver->ICM_Value.Pre_ACCEL[i] = Data_Buf[i + ICM_ACCEL_DATA_OFFSET_16BIT]; // 保存本次滤波后的值
        }
        /* 陀螺仪保存值*/
        for (uint8_t i = 0; i < ICM_GYRO_DATA_SIZE_16BIT; i++)
        {
            Data_Buf[i + ICM_GYRO_DATA_OFFSET_16BIT] -= Driver->ICM_Value.GYRO_Bias[i]; // 减去零偏
            Driver->ICM_Value.Pre_GYRO[i] = Data_Buf[i + ICM_GYRO_DATA_OFFSET_16BIT];   // 保存去除零偏后的值
        }
    }
    memcpy((uint8_t *)&Driver->ICM_Value.icm_Data_Buf[0], (uint8_t *)Data_Buf, sizeof(Data_Buf));
    return true;
}

/**
 * @brief 陀螺仪和加速度计数据滤波
 *
 * @param Driver 驱动结构体
 */
void icm42688_ACCEL_GYRO_Fil(ICM42688_Driver_t *Driver)
{
    icm42688_ACCEL_Fil(Driver, (int16_t *)&Driver->ICM_Value.icm_Data_Buf[ICM_ACCEL_DATA_OFFSET_8BIT]);
    icm42688_GYRO_Fil(Driver, (int16_t *)&Driver->ICM_Value.icm_Data_Buf[ICM_GYRO_DATA_OFFSET_8BIT]);
}

/**
 * @brief 使用DMA更新数据
 *
 * @param Driver 驱动结构体
 * @return true 成功
 * @return false 错误
 * @note 注意数据的互斥，防止DMA没有搬运完就直接启动下一次,搬运完之后需要交换字节序
 */
bool icm42688_DMA_UPData(ICM42688_Driver_t *Driver)
{
    /*发送起始位*/
    ERR_CHECK(Driver->IIC_Generate_Start(), Generate_Start_Err);
    /*发送七位地址+写*/
    ERR_CHECK((Driver->IIC_Send_Dev_Addr((uint8_t)(Driver->Dev_Addr << 1), true)), Send_Dev_Addr_Err); // 自动补零
    /*发送寄存器地址*/
    ERR_CHECK(Driver->IIC_Send_Data(icm42688_TEMP_OUT_H), Send_Reg_Addr_Err);
    /*重复发送起始位*/
    ERR_CHECK(Driver->IIC_Generate_Start(), RE_Generate_Start_Err);
    /*发送七位地址+读*/
    ERR_CHECK(Driver->IIC_Send_Dev_Addr((uint8_t)((Driver->Dev_Addr << 1) | 0x01), false), RE_Send_Dev_Addr_Err);
    /*启动DMA接收数据*/
    Driver->DMA_Start();
    return true;
}

/**
 * @brief 平均值校准
 *
 * @param Driver 驱动结构体
 * @param num_samples 采样次数
 * @param Delay_time_ms 采样间隔
 */
uint16_t icm42688_Cal(ICM42688_Driver_t *Driver, uint16_t num_samples, uint16_t Delay_time_ms)
{
    int32_t Sum_GYRO_Bias[3] = {0}; // 陀螺仪误差,【X】【Y】【Z】
    int16_t GYRO_Buf[3] = {0};      // 缓存区
    uint16_t Count = 0;             // 采样计数
    /*------------------ 零 偏 补 偿 ---------------------*/
    /*对原始数据求和*/
    for (uint16_t i = 0; i < num_samples; i++)
    {
        /* 更新数据 */
        if (icm42688_UpGYRO(Driver, false)) // 陀螺仪数据更新
        {
            memcpy((uint8_t *)GYRO_Buf, (uint8_t *)&Driver->ICM_Value.icm_Data_Buf[8], sizeof(GYRO_Buf));
            /*求和*/
            Sum_GYRO_Bias[0] += GYRO_Buf[0]; // 【X】
            Sum_GYRO_Bias[1] += GYRO_Buf[1]; // 【Y】
            Sum_GYRO_Bias[2] += GYRO_Buf[2]; // 【Z】
            Count++;
            Driver->Delay_ms(Delay_time_ms);
        }
    }
    /*求均值*/
    Driver->ICM_Value.GYRO_Bias[0] = (int16_t)((float)Sum_GYRO_Bias[0] / (float)num_samples); // 【X】
    Driver->ICM_Value.GYRO_Bias[1] = (int16_t)((float)Sum_GYRO_Bias[1] / (float)num_samples); // 【Y】
    Driver->ICM_Value.GYRO_Bias[2] = (int16_t)((float)Sum_GYRO_Bias[2] / (float)num_samples); // 【Z】
    return Count;
}

/**
 * @brief 解算欧拉角
 *
 * @param Driver 驱动结构体
 */
void icm42688_Slove_Eular(ICM42688_Driver_t *Driver)
{
    int16_t ACCEL_Buf[3] = {0};
    int16_t GYRO_Buf[3] = {0};
    /* 从缓存区获取数据 */
    memcpy((uint8_t *)ACCEL_Buf, (uint8_t *)&Driver->ICM_Value.icm_Data_Buf[ICM_ACCEL_DATA_OFFSET_8BIT], sizeof(ACCEL_Buf));
    memcpy((uint8_t *)GYRO_Buf, (uint8_t *)&Driver->ICM_Value.icm_Data_Buf[ICM_GYRO_DATA_OFFSET_8BIT], sizeof(GYRO_Buf));
    /*计算【陀螺仪】欧拉角，【Y】轴正方向为前进*/
    float G_Roll = Driver->ICM_Value.Roll + GYRO_Buf[1] * Driver->ICM_Para.Ts;   // 陀螺仪翻滚角	Y
    float G_Pitch = Driver->ICM_Value.Pitch + GYRO_Buf[0] * Driver->ICM_Para.Ts; // 陀螺仪俯仰角	X
    float G_Yaw = Driver->ICM_Value.Yaw + GYRO_Buf[2] * Driver->ICM_Para.Ts;     // 陀螺仪偏航角	Z
    /*计算【加速度计】欧拉角，重力方向为参考*/
    float A_Pitch = (atan2(ACCEL_Buf[1], ACCEL_Buf[2]) * RAD_TO_ANGLE); // 加速度计俯仰角	X		y/z		180/pi=57.2958弧度到角度
    float A_Roll = (-atan2(ACCEL_Buf[0], ACCEL_Buf[2]) * RAD_TO_ANGLE); // 加速度计翻滚角	Y		x/z		为保证正方向与芯片一样注意取反
    /*互补滤波*/
    Driver->ICM_Value.Pitch = (G_Pitch * (Driver->ICM_Para.Fil_Power) + (1 - (Driver->ICM_Para.Fil_Power)) * A_Pitch);
    Driver->ICM_Value.Roll = (G_Roll * (Driver->ICM_Para.Fil_Power) + (1 - (Driver->ICM_Para.Fil_Power)) * A_Roll);
    Driver->ICM_Value.Yaw = G_Yaw;
}
