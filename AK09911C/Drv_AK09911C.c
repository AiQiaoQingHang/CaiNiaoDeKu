#include <stdint.h>
#include "Drv_AK09911C.h"
#include "Drv_AK09911C_define.h"
#include "math.h"
#include "string.h"
/**
 * @brief AK09911C默认初始化
 *
 * @param Driver 驱动实列
 * @param Mode 初始化后工作模式
 */
void AK09911C_Default_Init(AK09911C_Driver_t *Driver, MODE_SEL Mode)
{
    uint8_t ASA_Temp[3] = {0};
    AK09911C_SofaReset(Driver);
    AK09911C_Mode_Sel(Driver, PD_Mode);         // 只有掉电模式能切换模式
    AK09911C_Mode_Sel(Driver, Fuse_Rom_mode);   // 只有Fuse_Rom模式可以读取校准系数
    AK09911C_R_Regs(Driver, ASAX, ASA_Temp, 3); // 获取校准系数
    Driver->ASA_Value.ASA_X = ASA_Temp[0];
    Driver->ASA_Value.ASA_Y = ASA_Temp[1];
    Driver->ASA_Value.ASA_Z = ASA_Temp[2];
    AK09911C_Mode_Sel(Driver, PD_Mode);
    AK09911C_Mode_Sel(Driver, Mode);
    // AK09911C_Cal(Driver, 20, 10);
}

/**
 * @brief AK09911C软复位
 *
 * @param Driver 驱动实例
 */
void AK09911C_SofaReset(AK09911C_Driver_t *Driver)
{
    AK09911C_W_Reg(Driver, CNTL_3, SRST);
    Driver->Delay_ms(1);
}

/**
 * @brief 写AK09911C寄存器
 *
 * @param Driver 驱动结构体
 * @param reg_adress 寄存器地址
 * @param Data 要写入的数据
 * @return true 成功
 * @return false 失败
 */
bool AK09911C_W_Reg(AK09911C_Driver_t *Driver, uint8_t reg_adress, uint8_t Data)
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
 * @brief 读AK09911C寄存器
 *
 * @param Driver 驱动结构体
 * @param reg_adress 寄存器地址
 * @param Rece 接收缓存区
 * @param size 读取字节数
 * @return true 成功
 * @return false 错误
 */
bool AK09911C_R_Regs(AK09911C_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size)
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
 * @brief 获取公司ID
 *
 * @param Driver 驱动结构体
 * @return uint8_t 公司ID 0x05
 */
uint8_t Ak09911C_GetID(AK09911C_Driver_t *Driver)
{
    uint8_t temp = 0;
    AK09911C_R_Regs(Driver, WIA_2, &temp, 1);
    return temp;
}

/**
 * @brief 选择工作模式
 *
 * @param Driver
 * @param Mode
 */
void AK09911C_Mode_Sel(AK09911C_Driver_t *Driver, MODE_SEL Mode)
{
    switch (Mode)
    {
    case PD_Mode: // 掉电模式
        AK09911C_W_Reg(Driver, CNTL_2, 0x00);
        break;
    case Single_Mode: // 单次采集模式
        AK09911C_W_Reg(Driver, CNTL_2, MODE0);
        break;
    case Con_1_Mode: // 连续模式 1 10Hz
        AK09911C_W_Reg(Driver, CNTL_2, MODE1);
        break;
    case Con_2_Mode: // 连续模式 2 20Hz
        AK09911C_W_Reg(Driver, CNTL_2, MODE2);
        break;
    case Con_3_Mode: // 连续模式 3 50Hz
        AK09911C_W_Reg(Driver, CNTL_2, (MODE1 | MODE2));
        break;
    case Con_4_Mode: // 连续模式 4 100Hz
        AK09911C_W_Reg(Driver, CNTL_2, MODE3);
        break;
    case Self_test_Mode: // 自测模式
        AK09911C_W_Reg(Driver, CNTL_2, MODE4);
        break;
    case Fuse_Rom_mode: // Fuse Rom模式
        AK09911C_W_Reg(Driver, CNTL_2, (MODE0 | MODE1 | MODE2 | MODE3 | MODE4));
        break;
    }
    Driver->Delay_ms(1); // 模式切换后需延迟 100 us，这里用 1 ms
}

/**
 * @brief 使用DMA更新数据
 *
 * @param Driver 驱动实例
 * @return true 成功
 * @return false 错误
 * @note 注意数据的互斥，防止DMA没有搬运完就直接启动下一次,搬运完之后需要交换字节序
 */
bool AK09911C_UpData_DMA(AK09911C_Driver_t *Driver)
{
    /*发送起始位*/
    ERR_CHECK(Driver->IIC_Generate_Start(), Generate_Start_Err);
    /*发送七位地址+写*/
    ERR_CHECK((Driver->IIC_Send_Dev_Addr((uint8_t)(Driver->Dev_Addr << 1), true)), Send_Dev_Addr_Err); // 自动补零
    /*发送寄存器地址*/
    ERR_CHECK(Driver->IIC_Send_Data(HXL), Send_Reg_Addr_Err);
    /*重复发送起始位*/
    ERR_CHECK(Driver->IIC_Generate_Start(), RE_Generate_Start_Err);
    /*发送七位地址+读*/
    ERR_CHECK(Driver->IIC_Send_Dev_Addr((uint8_t)((Driver->Dev_Addr << 1) | 0x01), false), RE_Send_Dev_Addr_Err);
    /*启动DMA接收数据*/
    Driver->DMA_Start();
    return true;
}

/**
 * @brief AK09911C更新数据
 *
 * @param Driver 驱动实例
 * @return true 成功
 * @return false 失败
 */
bool AK09911C_UpData(AK09911C_Driver_t *Driver)
{
    uint8_t ST1_Temp = 0;
    uint8_t ST2_Temp = 0;

    AK09911C_R_Regs(Driver, ST_1, &ST1_Temp, 1);
    if (!(ST1_Temp & DRDY)) // 数据没准备完毕
    {
        __data_not_ready_proc();
        return false;
    }
    AK09911C_R_Regs(Driver, HXL, (uint8_t *)&Driver->AK_Data_Buf, AK09911C_DATA_SIZE_8B);
    AK09911C_R_Regs(Driver, ST_2, &ST2_Temp, 1); // 手册要求读取数据后必须读ST2
    if (ST2_Temp & HOFL)
    {
        __data_over_proc();
        Driver->AK_Value.Data_Over_Cnt++; // 传感器数据溢出
        return false;
    }
    return true;
}

// uint16_t AK09911C_Cal(AK09911C_Driver_t *Driver, uint16_t num_samples, uint16_t Delay_time_ms)
// {
//     int32_t Sum_Bias[3] = {0}; // 陀螺仪误差,【X】【Y】【Z】
//     int16_t Mag_Buf[3]  = {0}; // 缓存区
//     uint16_t Count      = 0;   // 采样计数
//     /*------------------ 零 偏 补 偿 ---------------------*/
//     /*对原始数据求和*/
//     for (uint16_t i = 0; i < num_samples; i++) {
//         /* 更新数据 */
//         if (AK09911C_Up_Data(Driver)) // 陀螺仪数据更新
//         {
//             memcpy((uint8_t *)Mag_Buf, (uint8_t *)&Driver->AK_Data_Buf, sizeof(Mag_Buf));
//             /*求和*/
//             Sum_Bias[0] += Mag_Buf[0]; // 【X】
//             Sum_Bias[1] += Mag_Buf[1]; // 【Y】
//             Sum_Bias[2] += Mag_Buf[2]; // 【Z】
//             Count++;
//             Driver->Delay_ms(Delay_time_ms);
//         }
//     }
//     /*求均值*/
//     Driver->Cal_Bias.X_Bias = ((float)Sum_Bias[0] / (float)Count); // 【X】
//     Driver->Cal_Bias.Y_Bias = ((float)Sum_Bias[1] / (float)Count); // 【Y】
//     Driver->Cal_Bias.Z_Bias = ((float)Sum_Bias[2] / (float)Count); // 【Z】
//     return Count;
// }

/**
 * @brief Ak09911C数据处理
 *
 * @param Driver 驱动实例
 * @param Pitch 俯仰角
 * @param Roll 翻滚角
 * @note 这里的Pitch和Roll是相对于磁力计的(东北天坐标系)，比如磁力计绕 X轴 转对应六轴绕 Y轴 转，此时就需要参数调用反过来，
 *       正负号由于在函数内部已经将磁力计方向与六轴方向对齐，所以旋转方向是一致的
 */
void AK09911C_Data_Proc(AK09911C_Driver_t *Driver, float Pitch_Deg, float Roll_Deg)
{
    int16_t *Ptr_16B = (int16_t *)&Driver->AK_Data_Buf; // 指向X数据
    float X_WinOut,Y_WinOut;
    /* 交换字节序 */
    // BYTESWAP_ARRAY_16BIT((int16_t *)&Driver->AK_Data_Buf, AK09911C_DATA_SIZE_16B);
    /* 校准 */
    Driver->AK_Value.X_Data = CAL_DATA((float)(*Ptr_16B), (float)Driver->ASA_Value.ASA_X);
    Ptr_16B++; // 自增指向Y数据
    Driver->AK_Value.Y_Data = CAL_DATA((float)(*Ptr_16B), (float)Driver->ASA_Value.ASA_Y);
    Ptr_16B++; // 自增指向Z数据
    Driver->AK_Value.Z_Data = CAL_DATA((float)(*Ptr_16B), (float)Driver->ASA_Value.ASA_Z);
    /* 角度单位转换 */
    float Pitch_Rad = Pitch_Deg * DEG_TO_RAD;
    float Roll_Rad = Roll_Deg * DEG_TO_RAD;

    // Driver->AK_Value.X_Comp = -Driver->AK_Value.Y_Data;
    // Driver->AK_Value.Y_Comp = Driver->AK_Value.X_Data;
    // Driver->AK_Value.Z_Comp = Driver->AK_Value.Z_Data;

    /* 4. 预计算三角函数 */
    float cosP = cosf(Pitch_Rad);
    float sinP = sinf(Pitch_Rad);
    float cosR = cosf(Roll_Rad);
    float sinR = sinf(Roll_Rad);

    // 【根据坐标系关系修改】
    // Mag_X→-IMU_Y；Mag_Y→IMU_X；Mag_Z→-IMU_Z
    float IMU_Mag_X = -Driver->AK_Value.Y_Data;
    float IMU_Mag_Y = Driver->AK_Value.X_Data;
    float IMU_Mag_Z = Driver->AK_Value.Z_Data;
    /* 5. 倾斜补偿（现在用的是“IMU坐标系下的磁力计数据”） */
    // 水平东向分量（Mex）：仅受地理东向影响，与Pitch/Roll无关
    Driver->AK_Value.X_Comp = IMU_Mag_X * cosP + IMU_Mag_Y * sinP * sinR + IMU_Mag_Z * sinP * cosR;
    // 水平北向分量（Mey）：仅受地理北向影响，与Pitch/Roll无关
    Driver->AK_Value.Y_Comp = IMU_Mag_Y * cosR - IMU_Mag_Z * sinR;
    // 垂直天向分量（Mez）：包含所有垂直方向误差，不影响水平分量
    Driver->AK_Value.Z_Comp = -IMU_Mag_X * sinP + IMU_Mag_Y * cosP * sinR + IMU_Mag_Z * cosP * cosR;

    /* 更新滑动窗口 */
    UP_SILD_WINDOW(Driver->X_Sild, Driver->AK_Value.X_Comp);
    UP_SILD_WINDOW(Driver->Y_Sild, Driver->AK_Value.Y_Comp);
    UP_SILD_WINDOW(Driver->Z_Sild, Driver->AK_Value.Z_Comp);
    /* 求和更新窗口输出 */
    UP_SILD_WINDOW_OUT(Driver->X_Sild, X_WinOut);
    UP_SILD_WINDOW_OUT(Driver->Y_Sild, Y_WinOut);
    //UP_SILD_WINDOW_OUT(Driver->Z_Sild, Z_WinOut);
    Driver->AK_Value.Yaw = (-atan2f(X_WinOut, Y_WinOut) * RAD_TO_DEG);
    if (Driver->AK_Value.Yaw < 0.0f)
    {
        Driver->AK_Value.Yaw += 360.0f;
    }
}