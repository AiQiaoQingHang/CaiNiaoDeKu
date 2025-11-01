#include "NRF24L01_USER.h"
#include "NRF24L01_Drv.h"
#include "Delay_USER.h"
#include "stm32f4xx.h"
#include "Periph_Init.h"

static void USER_NRF_W_CE(GPIO_Status Status);
static void USER_NRF_W_CS(GPIO_Status Status);
static uint8_t USER_NRF_Swap_Byte(uint8_t Data);
static void USER_NRF_Delay_us(uint32_t Time_us);

NRF24L01_Driver_t NRF_Drv = {
    .Delay_us = USER_NRF_Delay_us,
    .NRF24L01_W_CE = USER_NRF_W_CE,
    .NRF24L01_W_CS = USER_NRF_W_CS,
    .SPI_SwapByte_MODE0 = USER_NRF_Swap_Byte,
};

void USER_NRF_Init(void)
{
    //NRF24L01_EnterStandby_1(&NRF_Drv);
    NRF24L01_SyncStruct_All(&NRF_Drv);
    NRF24L01_EN_FEATURE(&NRF_Drv, EN_ACK_PAY_ENABLE, NRF24L01_ENABLE);
    NRF24L01_EN_AutoACK(&NRF_Drv, EN_AA_P0, NRF24L01_ENABLE);
    NRF24L01_Set_CRCByte(&NRF_Drv, CRC_2_byte);
}

/**
 * @brief 写CE
 *
 * @param Status CE状态
 */
static void USER_NRF_W_CE(GPIO_Status Status)
{
    GPIO_WriteBit(GPIOE, GPIO_Pin_2, (BitAction)Status);
}

/**
 * @brief 写CS
 *
 * @param Status CS状态
 */
static void USER_NRF_W_CS(GPIO_Status Status)
{
    GPIO_WriteBit(GPIOE, GPIO_Pin_3, (BitAction)Status);
}

/**
 * @brief SPI交换单字节
 *
 * @param Data 发生的数据
 * @return uint8_t 接收的数据
 */
static uint8_t USER_NRF_Swap_Byte(uint8_t Data)
{
    uint16_t Timeout = 3000;
    SPI_I2S_SendData(NRF_USE_SPI, (uint16_t)Data);
    while (SPI_I2S_GetFlagStatus(NRF_USE_SPI, SPI_I2S_FLAG_TXE) == RESET) // 等待发送缓冲区空
    {
        Timeout--;
        if (!Timeout)
        {
            return 0xFF;
        }
    }
    Timeout = 3000;
    while (SPI_I2S_GetFlagStatus(NRF_USE_SPI, SPI_I2S_FLAG_BSY) == SET) // 等待传输完成
    {
        Timeout--;
        if (!Timeout)
        {
            return 0xFF;
        }
    }
    return SPI_I2S_ReceiveData(NRF_USE_SPI);
}

/**
 * @brief 微秒延迟
 *
 * @param Time_us 时间
 */
static void USER_NRF_Delay_us(uint32_t Time_us)
{
    USER_Delay_us(Time_us);
}