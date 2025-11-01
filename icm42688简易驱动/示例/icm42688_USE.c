#include "icm42688_USE.h"
#include "icm42688_Drv.h"
#include "Periph_Init.h"
#include "stm32f4xx.h"
#include "Delay_USE.h"
#include "FreeRTOS.h"
#include "task.h"

static bool IIC_GenerateStart(void);                   // 发送起始位
static bool IIC_SendAddr(uint8_t Addr, bool IS_Write); // 发送设备地址
static bool IIC_SendData(uint8_t Data_Buf);            // 发送【1字节】数据
static bool IIC_ReceData(uint8_t *Buf, bool IS_NACK);  // 接收数据
static bool IIC_GenerateStop(void);                    // 发送停止位
static void IIC_Err_CallBack(ICM42688_Driver_t *Drv);  // 错误处理
static void USER_ICM_DMA_Start(void);                  // 启动DMA

TaskHandle_t ICM_Taskhandle = NULL; // ICM42688任务
QueueHandle_t ICM_DMA_Busy = NULL;  // ICM42688的【DMA忙】信号量

ICM42688_Driver_t ICM_Drv = {
    .IIC_Generate_Start = IIC_GenerateStart,
    .IIC_Generate_Stop = IIC_GenerateStop,
    .IIC_Rece_Data = IIC_ReceData,
    .IIC_Send_Data = IIC_SendData,
    .IIC_Send_Dev_Addr = IIC_SendAddr,
    .Delay_ms = USER_Delay_ms,
    .Err_CallBack = IIC_Err_CallBack,
    .DMA_Start = USER_ICM_DMA_Start,
    .ICM_Para.ACCEL_Alone_Fil = 0.8f,
    .ICM_Para.Fil_Power = 0.8f,
    .ICM_Para.GYRO_Alone_Fil = 0.8f,
    .ICM_Para.Ts = 0.005f,
    .ICM_Value = 0,
    .Dev_Addr = ICM_Addr,
};

/**
 * @brief 初始化ICM
 *
 */
void USER_ICM_Init(void)
{
    icm42688_Default_Init(&ICM_Drv);
    ICM_DMA_Init();
    /* FreeRTOS信号量创建 */
    ICM_DMA_Busy = xSemaphoreCreateBinary();
    xSemaphoreGive(ICM_DMA_Busy);
    /* FreeRTOS任务创建 */
    xTaskCreate(ICM_FreeRTOSTask, "ICM_TASK", 4096, NULL, 2, &ICM_Taskhandle);
}

/**
 * @brief ICM任务
 *
 * @param arg
 */
void ICM_FreeRTOSTask(void *arg)
{
    TickType_t xLastWakeTime = 0;
    while (1)
    {
        xSemaphoreTake(ICM_DMA_Busy, portMAX_DELAY); // 等待DMA空闲
        xLastWakeTime = xTaskGetTickCount();
        icm42688_DMA_UPData(&ICM_Drv);                                 // 中断里给予信号量
        if (xSemaphoreTake(ICM_DMA_Busy, pdTICKS_TO_MS(3)) == pdTRUE) // 等待DMA搬运结束
        {
            BYTESWAP_ARRAY_16BIT(ICM_Drv.ICM_Value.icm_Data_Buf, ICM_ALL_DATA_SIZE_16BIT); // 交换字节序
            icm42688_ACCEL_GYRO_Fil(&ICM_Drv);                                             // 数据滤波
            icm42688_Slove_Eular(&ICM_Drv);                                                // 解算欧拉角
        }
        else // DMA超时
        {
            /* DMA重新配置 */
            DMA_ClearITPendingBit(ICM_DMA, DMA_IT_TCIF0);
            DMA_Cmd(ICM_DMA, DISABLE);
            DMA_SetCurrDataCounter(ICM_DMA, 14);
            // 错误处理，待添加...
        }
        xSemaphoreGive(ICM_DMA_Busy); // 给予信号量作为下次DMA空闲判断
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5));
    }
}

// /**
//  * @brief 更新ICM42688数据
//  *
//  * @param Drv 驱动结构体
//  * @return true
//  * @return false
//  */
// static bool USER_ICM_UPData(ICM42688_Driver_t* Drv)
// {
//     if (!(icm42688_UpAllData(Drv, true)))
//     {
//         return false;
//     }
//     icm42688_Slove_Eular(Drv);
//     return true;
// }

/**
 * @brief ICM启动DMA传输
 *
 */
static void USER_ICM_DMA_Start(void)
{
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    I2C_DMALastTransferCmd(I2C1, ENABLE);
    DMA_Cmd(ICM_DMA, ENABLE);
}

/**
 * @brief  产生IIC起始位（实现驱动的Generate_Start函数指针）
 * @return true：成功；false：失败
 */
static bool IIC_GenerateStart(void)
{
    uint32_t timeout = 10000; // 超时计数（防止死等）
    // 发送起始位
    I2C_GenerateSTART(I2C1, ENABLE);
    // 等待起始位发送完成（EV5：SB=1）
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if (--timeout == 0)
            return false; // 超时失败
    }
    return true;
}

/**
 * @brief IIC发送设备地址
 *
 * @param Addr 设备地址
 * @param IS_Write 是否为读方向
 * @return true 成功
 * @return false 失败
 */
static bool IIC_SendAddr(uint8_t Addr, bool IS_Write)
{
    uint32_t timeout = 300000; // 延长超时，避免误判
    I2C_Send7bitAddress(I2C1, Addr, (IS_Write) ? I2C_Direction_Transmitter : I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1, (IS_Write) ? I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED : I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if (--timeout == 0)
        {
            I2C_GenerateSTOP(I2C1, ENABLE); // 发送停止位
            return false;
        }
    }
    return true;
}

/**
 * @brief IIC发送【1字节】数据
 *
 * @param Data_Buf 数据
 * @return true 成功
 * @return false 失败
 */
static bool IIC_SendData(uint8_t Data_Buf)
{
    uint32_t timeout = 100000; // 延长超时，避免误判
    I2C_SendData(I2C1, Data_Buf);
    // 发送普通数据（寄存器地址/数据），检测“数据发送完成”事件（EV8_2）
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if (--timeout == 0)
        {
            I2C_GenerateSTOP(I2C1, ENABLE); // 发送停止位
            return false;
        }
    }
    return true;
}

/**
 * @brief  接收1字节数据（实现驱动的Rece_Data函数指针）
 * @param  ACK：true=发送NACK；false=发送ACK
 * @return true 成功
 * @return false 失败
 */
static bool IIC_ReceData(uint8_t *Buf, bool IS_NACK)
{
    uint32_t timeout = 10000;
    // 配置应答位（接收最后1字节时发送NACK）
    if (IS_NACK)
    {
        I2C_AcknowledgeConfig(I2C1, DISABLE); // NACK
    }
    else
    {
        I2C_AcknowledgeConfig(I2C1, ENABLE); // ACK
    }
    // 等待数据接收完成（EV7：RxNE=1）
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
        if (--timeout == 0)
            return false; // 超时返回无效值
    }
    // 读取数据
    *Buf = I2C_ReceiveData(I2C1);
    return true;
}

/**
 * @brief  产生IIC停止位（实现驱动的Generate_Stop函数指针）
 * @return true 成功
 * @return false 失败
 */
static bool IIC_GenerateStop(void)
{
    I2C_GenerateSTOP(I2C1, ENABLE);      // 发送停止位
    I2C_AcknowledgeConfig(I2C1, ENABLE); // 重新配置ACK信号，保证每次传输开始时都是ACK
    return true;
}

/**
 * @brief IIC错误回调函数
 *
 * @param Err 错误标志位
 */
static void IIC_Err_CallBack(ICM42688_Driver_t *Drv)
{
    /* 配置为推挽输出，强制拉高总线 */
    GPIOB->BSRRL = (1 << 6) | (1 << 7);
    GPIOB->MODER &= ~((0x3 << 12) | (0x3 << 14)); // 清除PB6（bit12-13）和PB7（bit14-15）
    GPIOB->MODER |= (0x1 << 12) | (0x1 << 14);    // PB6=通用输出（01），PB7=通用输出（01）
    // 配置为推挽输出（OTYPER位6和7=0）
    GPIOB->OTYPER &= ~((1 << 6) | (1 << 7)); // 清除两位，确保推挽
    // 延迟（确保总线稳定拉高）
    for (uint16_t i = 0; i < 500; i++) // 约几微秒，根据实际调整
    {
        __nop();    // 防止编译器优化
    }
    /* 恢复为复用开漏模式 */
    // 清除PB6和PB7的MODER位，配置为复用功能模式（10）
    GPIOB->MODER &= ~((0x3 << 12) | (0x3 << 14)); // 再次清除，避免残留
    GPIOB->MODER |= ((0x2 << 12) | (0x2 << 14));    // PB6=复用（10），PB7=复用（10）
    // 配置为开漏输出（OTYPER位6和7=1，符合IIC协议）
    GPIOB->OTYPER |= (1 << 6) | (1 << 7); // 开漏输出
    Drv->Err_Count++;
    Drv->Err_Flag = None_Err;
}