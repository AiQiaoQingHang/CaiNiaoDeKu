#include <stdint.h>
#include "NRF24L01.h"
#include "NRF24L01_define.h"
#include <string.h>

#if Debug_Enable
uint8_t DRIVER_VOID_ERR = 0; // NRF24L01_Driver_t空指针标志位,调试启用空指针检查有用
uint8_t PRINTF_VOID_ERR = 0; // 打印函数空指针标志位

/********************************************************************
 *简	介：参数检查错误回调函数
 *参	数：无
 *返 回 值：无
 *说	明：检查NRF24L01_Driver_t是否为空指针，为空指针会调用此函数,自己根据需求实现
 *********************************************************************/
__weak void NRF24L01_ASSERT_Failed(void)
{
    while(1)
    {

    };
}
#endif

/********************************************************************
 *简	介：中断服务函数
 *参	数：底层驱动结构体
 *返 回 值：无
 *说	明：中断服务函数，会调用中断回调函数
 *********************************************************************/
void NRF24L01_IRQHandler(NRF24L01_Driver_t *drv)
{
    uint8_t status = NRF24L01_R_Reg(drv, NRF24L01_STATUS);
    if (((status & MAX_RT) != 0) && (drv->MAX_RT_CallBack)) {
        NRF24L01_ClearITFLAG(drv, MAX_RT);
        drv->MAX_RT_CallBack(drv);
    }
    if (((status & TX_DS) != 0) && (drv->TX_DS_CallBack)) {
        NRF24L01_ClearITFLAG(drv, TX_DS);
        drv->TX_DS_CallBack(drv);
    }
    if (((status & RX_DR) != 0) && (drv->RX_DR_CallBack)) {
        NRF24L01_ClearITFLAG(drv, RX_DR);
        drv->RX_DR_CallBack(drv);
    }
}

/********************************************************************
 *简	介：读NRF24L01寄存器
 *参	数：寄存器地址
 *返回值：寄存器状态
 *说	明：上层函数调用
 *********************************************************************/
uint8_t NRF24L01_R_Reg(NRF24L01_Driver_t *drv, uint8_t Reg_adress)
{
    NRF24L01_ASSERT(drv);
    uint8_t status;
    /*拉低片选，开始通信*/
    drv->NRF24L01_W_CS(LOW);
    /*发送命令码和寄存器地址*/
    drv->SPI_SwapByte_MODE0(NRF24L01_R_REGISTER | Reg_adress);
    /*空字节维持时钟，交换寄存器信息*/
    status = drv->SPI_SwapByte_MODE0(dummy);
    /*拉高片选，结束通信*/
    drv->NRF24L01_W_CS(HIGH);
    return status;
}

/********************************************************************
 *简	介：读NRF24L01寄存器多字节
 *参	数：寄存器地址，缓存区，读多少字节
 *返回值：寄存器状态
 *说	明：此函数主要用于部分需要多字节读入的寄存器，如接收地址，发送地址
 *注	意：读多少字节应小于缓存区大小
 *********************************************************************/
uint8_t *NRF24L01_R_Regs(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t *Rece, uint8_t size)
{
    NRF24L01_ASSERT(drv);
    /*拉低片选，开始通信*/
    drv->NRF24L01_W_CS(LOW);
    /*发送命令码和寄存器地址*/
    drv->SPI_SwapByte_MODE0(NRF24L01_R_REGISTER | Reg_adress);
    /*写入寄存器数据*/
    for (uint8_t i = 0; i < size; i++) {
        (*(Rece + i)) = drv->SPI_SwapByte_MODE0(dummy);
    }
    /*拉高片选，结束通信*/
    drv->NRF24L01_W_CS(HIGH);
    return Rece;
}

/********************************************************************
 *简	介：写NRF24L01寄存器
 *参	数：寄存器地址，写入的数据
 *返 回 值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_W_Reg(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t Data)
{
    NRF24L01_ASSERT(drv);
    /*拉低片选，开始通信*/
    drv->NRF24L01_W_CS(LOW);
    /*发送命令码和寄存器地址*/
    drv->SPI_SwapByte_MODE0(NRF24L01_W_REGISTER | Reg_adress);
    /*写入寄存器数据*/
    drv->SPI_SwapByte_MODE0(Data);
    /*拉高片选，结束通信*/
    drv->NRF24L01_W_CS(HIGH);
}

/********************************************************************
 *简	介：写NRF24L01寄存器多字节
 *参	数：寄存器地址，要写入的数据，数据个数
 *返回值：无
 *说	明：此函数主要用于部分需要多字节写入的寄存器，如接收地址，发送地址
 *********************************************************************/
void NRF24L01_W_Regs(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t *Data, uint8_t size)
{
    NRF24L01_ASSERT(drv);
    /*拉低片选，开始通信*/
    drv->NRF24L01_W_CS(LOW);
    /*发送命令码和寄存器地址*/
    drv->SPI_SwapByte_MODE0(NRF24L01_W_REGISTER | Reg_adress);
    /*写入寄存器数据*/
    for (uint8_t i = 0; i < size; i++) {
        drv->SPI_SwapByte_MODE0(*(Data + i));
    }
    /*拉高片选，结束通信*/
    drv->NRF24L01_W_CS(HIGH);
}

/********************************************************************
 *简	介：修改2.4G信号频段
 *参	数：2.4g频段【2400-2525】Mhz
 *返回值：无
 *说	明：计算公式F= 2400 + RF_CH [MHz]
 *********************************************************************/
void NRF24L01_SetFrequencyMHz(NRF24L01_Driver_t *drv, uint16_t freqMHz)
{
    NRF24L01_ASSERT(drv);
    // 限制范围 2400 MHz ~ 2525 MHz
    if (freqMHz < 2400) freqMHz = 2400;
    if (freqMHz > 2525) freqMHz = 2525;
    uint8_t channel = (uint8_t)(freqMHz - 2400);
    NRF24L01_W_Reg(drv, NRF24L01_RF_CH, channel);
    /*更新结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_RF_CH, channel);
}

/********************************************************************
 *简	介：修改空中数据速率
 *参	数：查看宏定义中【RF_SETUP】
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_SetAir_Data_Rate(NRF24L01_Driver_t *drv, Reg_RF_SETUP RF_DR)
{
    NRF24L01_ASSERT(drv);
    uint8_t RF_SETUP_Reg = 0;
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    RF_SETUP_Reg = NRF24L01_R_Reg(drv, NRF24L01_RF_SETUP);
    /*位操作*/
    RF_SETUP_Reg &= ~(RF_DR_Config_Bit); //	(RF_SETUP_Reg & (1 1 0 1		 0 1 1 0)) = (x x 0 x 		0 x x 0)清零位3、5,最低位必须为0
    /*写入空中数据速率*/
    RF_SETUP_Reg |= RF_DR;
    /*写回寄存器*/
    NRF24L01_W_Reg(drv, NRF24L01_RF_SETUP, RF_SETUP_Reg);
    /*更新结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_RF_SETUP, RF_SETUP_Reg);
}

/*********************************************************************
 *简	介：修改射频输出功率
 *参	数：查看宏定义中【RF_SETUP】
 *返回值：无
 *说	明：无
 **********************************************************************/
void NRF24L01_PA_Control(NRF24L01_Driver_t *drv, Reg_RF_SETUP RF_PWR)
{
    NRF24L01_ASSERT(drv);
    uint8_t RF_SETUP_Reg = 0;
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    RF_SETUP_Reg = NRF24L01_R_Reg(drv, NRF24L01_RF_SETUP);
    /*位操作*/
    RF_SETUP_Reg &= ~(RF_PWR_Config_Bit); //	(RF_SETUP_Reg & (1 1 1 1		1 0 0 0)) = (x x x x 		x 0 0 0)清零位1、2，最低位必须为0
    /*写入射频输出功率*/
    RF_SETUP_Reg |= RF_PWR;
    /*写回寄存器*/
    NRF24L01_W_Reg(drv, NRF24L01_RF_SETUP, RF_SETUP_Reg);
    /*更新结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_RF_SETUP, RF_SETUP_Reg);
}

/********************************************************************
 *简	介：清空FIFOTx
 *参	数：无
 *返回值：无
 *说	明：FIFOTX有三级，只能一次清除
 *********************************************************************/
void NRF24L01_ClearFIFOTx(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    /*拉低CS，开始通信*/
    drv->NRF24L01_W_CS(LOW);
    /*发送【清空FIFOTx】命令字节*/
    drv->SPI_SwapByte_MODE0(NRF24L01_FLUSH_TX);
    /*拉高CS，结束通信*/
    drv->NRF24L01_W_CS(HIGH);
    /*等待清除完成，具体时间待测*/
    drv->Delay_us(20);
}

/********************************************************************
 *简	介：清空FIFORx
 *参	数：无
 *返回值：无
 *说	明：FIFORX有三级，只能一次清除
 *********************************************************************/
void NRF24L01_ClearFIFORx(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    /*拉低CS，开始通信*/
    drv->NRF24L01_W_CS(LOW);
    /*发送【清空FIFORx】命令字节*/
    drv->SPI_SwapByte_MODE0(NRF24L01_FLUSH_RX);
    /*拉高CS，结束通信*/
    drv->NRF24L01_W_CS(HIGH);
    /*等待清除完成，具体时间待测*/
    drv->Delay_us(20);
}

/********************************************************************
 *简	介：读RXFIFO
 *参	数：缓存区结构体
 *返回值：无
 *说	明：此函数用于读取RX
 *注	意：缓存区大小要 >= 个数
 *********************************************************************/
void NRF24L01_R_RxFIFO(NRF24L01_Driver_t *drv, NRF24L01_Pack_t *Rece)
{
    NRF24L01_ASSERT(drv);
    uint8_t length = 0; // 数据长度
    /*获取下一个数据来自哪个管道*/
    uint8_t Pipe = NRF24L01_GetDataPipe(drv);
    if (Pipe > 5) return; // RxFIFO内无数据
/*判断此管道是动态载荷还是静态载荷*/
#if (Status_SYNC && SYNC_INCLUDE_FEATURE_Reg && SYNC_INCLUDE_DYNPD_Reg)
    if ((drv->NRF_Status.EN_DPL_FLAG) && ((drv->NRF_Status.EN_Pipe_DPL) & (0x01 << Pipe)))
#else
    if ((NRF24L01_R_Reg(drv, NRF24L01_FEATURE) & EN_DPL_ENABLE) && (NRF24L01_R_Reg(drv, NRF24L01_DYNPD) & (0x01 << Pipe))) // 无需在意数据位，if只判断是不是0
#endif
    // 通道号与动态载荷的使能刚好一一对应，左移刚好得到对应通道的动态载荷使能情况，只有动态载荷的总开关和通道的动态载荷同时使能才算使能动态载荷
    {
        /*动态载荷，用命令获取长度*/
        drv->NRF24L01_W_CS(LOW);                       // 开始通信
        drv->SPI_SwapByte_MODE0(NRF24L01_R_RX_PL_WID); // 发送命令字节
        length = drv->SPI_SwapByte_MODE0(dummy);       // 哑元字节维持时钟，得到长度
        drv->NRF24L01_W_CS(HIGH);                      // 结束通信
    } else                                             // 静态载荷，读取对应寄存器
    {
        length = NRF24L01_R_Reg(drv, NRF24L01_RX_PW_P0 + Pipe); // 地址连续，加上Pipe偏移即可
    }
    /*判断参数合法*/
    if (length > 0 && length < 32)
        return;
    /*更新数据*/
    drv->NRF24L01_W_CS(LOW);
    /*发送命令字节*/
    drv->SPI_SwapByte_MODE0(NRF24L01_R_RX_PAYLOAD);
    /*读取数据*/
    for (uint8_t i = 0; i < length; i++) {
        Rece->Data[i] = drv->SPI_SwapByte_MODE0(dummy); // 哑元字节维持时钟
    }
    /*结束通信*/
    drv->NRF24L01_W_CS(HIGH);
    /*更新通道信息*/
    Rece->pipe = Pipe;
}

/********************************************************************
 *简	介：写TX
 *参	数：指令，要写的数据，数据大小，管道号
 *返回值：无
 *说	明：有三种写TX指令，其中写ACK附带载荷需要指定管道号，Pipe_X参数只在此时有用
 *********************************************************************/
void NRF24L01_W_Tx(NRF24L01_Driver_t *drv, W_TX_Command W_TX, uint8_t *Send, uint8_t size, uint8_t Pipe_X)
{
    NRF24L01_ASSERT(drv);
    /*确保最大就32字节*/
    if (size > 32) {
        size = 32;
        NRF24L01_ERR_PRINT(drv, "NRF24L01_W_Tx Size Inval,use 32 Size");
    }
    /*拉低CS，开始通信*/
    drv->NRF24L01_W_CS(LOW);
    /*根据命令完成不同操作*/
    if (W_TX == W_TX_ACK_PAYLOAD) {
        if (Pipe_X > 5) Pipe_X = 5;             // 防止传入0-5以外的参数
        drv->SPI_SwapByte_MODE0(W_TX | Pipe_X); // ACK附带有效载荷需指定管道号
    } else {
        drv->SPI_SwapByte_MODE0(W_TX); // 剩下两种直接写入即可
    }
    /*SPI保持发送*/
    for (uint8_t i = 0; i < size; i++) {
        drv->SPI_SwapByte_MODE0(Send[i]); // 往TX内写数据
    }
    /*拉高CS，结束通信*/
    drv->NRF24L01_W_CS(HIGH);
}

/********************************************************************
 *简	介：进入TX
 *参	数：无
 *返回值：无
 *说	明：这里实际是进入【待机模式1】，发送数据需要CE脉冲
 *********************************************************************/
void NRF24L01_EnterTx(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t config_reg = 0;
    /*拉低CE*/
    drv->NRF24L01_W_CE(LOW);
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    config_reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*清除第0，1位*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*将PWR_UP置1,PRIM_Rx置0*/
    config_reg |= PWR_UP; // x x x x		x x 1 0，PRIM_RX在上面已经是0
    /*写回寄存器*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, config_reg);
    /*更新结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, config_reg);
}

/********************************************************************
 *简	介：进入RX
 *参	数：无
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_EnterRx(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t config_reg = 0;
    /*拉低CE，修改寄存器*/
    drv->NRF24L01_W_CE(LOW);
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    config_reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*清除第0，1位*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*将PWR_UP和PRIM_Rx置1*/
    config_reg |= (PWR_UP | PRIM_RX); // x x x x		x x 1 1
    /*写回寄存器*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, config_reg);
    /*更新结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, config_reg);
    /*拉高CE，持续监听*/
    drv->NRF24L01_W_CE(HIGH); // 检查同步有寄存器读写，先检查同步
}

/********************************************************************
 *简	介：进入待机模式1
 *参	数：无
 *返回值：无
 *说	明：用于修改寄存器值
 *********************************************************************/
void NRF24L01_EnterStandby_1(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t config_reg = 0;
    /*拉低CE*/
    drv->NRF24L01_W_CE(LOW);
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    config_reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*将PWR_UP置1*/
    config_reg |= PWR_UP;
    /*写回寄存器*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, config_reg);
    /*更新结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, config_reg);
}

/********************************************************************
 *简	介：进入掉电模式
 *参	数：无
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_EnterPowerDown(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t config_reg = 0;
    /*拉低CE*/
    drv->NRF24L01_W_CE(LOW);
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    config_reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*清除PWR_UP位*/
    config_reg &= ~(PWR_UP); // x x x x		x x 0 x
    /*写回寄存器*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, config_reg);
    /*更新结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, config_reg);
}

/********************************************************************
 *简	介：发送数据
 *参	数：无
 *返回值：无
 *说	明：产生CE脉冲
 *********************************************************************/
void NRF24L01_SendData(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    /*进入TX，此时CE没有脉冲*/
    NRF24L01_EnterTx(drv);
    /*产生脉冲*/
    drv->NRF24L01_W_CE(HIGH);
    drv->Delay_us(20);
    drv->NRF24L01_W_CE(LOW);
    /*剩余内容在中断中处理*/
    /*在中断中重新进入接收模式*/
}

/********************************************************************
 *简	介：使能通道自动应答
 *参	数：通道，使能还是失能
 *返回值：无
 *说	明：这个函数只能操作一个通道
 *********************************************************************/
void NRF24L01_EN_AutoACK(NRF24L01_Driver_t *drv, Reg_EN_AA EN_AA, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_EN_AA);
    /*清0对应位置*/
    Reg &= ~EN_AA; // 这里通道与数据位刚好一一对应
    /*判断使能还是失能*/
    if (new_status) // 使能置位，失能不管
    {
        Reg |= EN_AA;
    }
    /*重新写回数据*/
    NRF24L01_W_Reg(drv, NRF24L01_EN_AA, Reg);
    /*更新结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_EN_AA, Reg);
}

/********************************************************************
 *简	介：使能接收通道
 *参	数：接收通道，使能还是失能
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_EN_RxAddr(NRF24L01_Driver_t *drv, Reg_EN_RXADDR EN_RXADDR, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_EN_RXADDR);
    /*清0对应位置*/
    Reg &= ~EN_RXADDR; // 这里通道与数据位刚好一一对应
    /*判断使能还是失能*/
    if (new_status) // 使能置位，失能不管
    {
        Reg |= EN_RXADDR;
    }
    NRF24L01_W_Reg(drv, NRF24L01_EN_RXADDR, Reg);
    /*更新状态结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_EN_RXADDR, Reg);
}

/********************************************************************
 *简	介：设置地址宽度
 *参	数：地址宽度
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_Set_Addr_Width(NRF24L01_Driver_t *drv, Reg_SETUP_AW Address_Width)
{
    NRF24L01_ASSERT(drv);
    /*检查参数合法*/
    if (Address_Width < Address_Width_3 || Address_Width > Address_Width_5) {
        Address_Width = Address_Width_5;
        NRF24L01_ERR_PRINT(drv, "NRF24L01_Set_Addr_Width INval,Use 5 AddrWith");
    }
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_SETUP_AW);
    /*第0位与第1位清0*/
    Reg &= ~(Address_Width_Config_Bit);
    /*写入参数*/
    Reg |= Address_Width;
    /*重新写回数据*/
    NRF24L01_W_Reg(drv, NRF24L01_SETUP_AW, Reg);
    /*更新状态结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_SETUP_AW, Reg);
}

/********************************************************************
 *简	介：设置自动重传
 *参	数：重传延迟，重传次数
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_SET_RETR(NRF24L01_Driver_t *drv, Reg_SETUP_RETR_ARD ARD, Reg_SETUP_RETR_ARC ARC)
{
    NRF24L01_ASSERT(drv);
    /*直接写入即可*/
    NRF24L01_W_Reg(drv, NRF24L01_SETUP_RETR, (ARD | ARC));
    /*更新状态结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_SETUP_RETR, (ARD | ARC));
}

/********************************************************************
 *简	介：使能动态载荷
 *参	数：通道，使能还是失能
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_EN_DPL(NRF24L01_Driver_t *drv, Reg_DYNPD EN_DPL, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_DYNPD);
    /*清0对应位置*/
    Reg &= ~EN_DPL; // 这里通道与数据位刚好一一对应
    /*判断使能还是失能*/
    if (new_status) // 使能置位，失能不管
    {
        Reg |= EN_DPL;
    }
    /*重新写回数据*/
    NRF24L01_W_Reg(drv, NRF24L01_DYNPD, Reg);
    /*更新状态结构体,并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_DYNPD, Reg);
}

/********************************************************************
 *简	介：使能特殊功能
 *参	数：特殊功能，使能还是失能
 *返回值：无
 *说	明：动态载荷总开关，带负载ACK，数据包可无ACK
 *********************************************************************/
void NRF24L01_EN_FEATURE(NRF24L01_Driver_t *drv, Reg_FEATURE EN_FEATURE, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_FEATURE);
    /*清0对应位置*/
    Reg &= ~EN_FEATURE; // 这里特殊功能与数据位一一对应
    /*判断使能还是失能*/
    if (new_status) // 使能置位，失能不管
    {
        Reg |= EN_FEATURE;
    }
    /*重新写回数据*/
    NRF24L01_W_Reg(drv, NRF24L01_FEATURE, Reg);
    /*更新状态结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_FEATURE, Reg);
}

/********************************************************************
 *简	介：设置CRC校验码字节数
 *参	数：CRC字节数
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_Set_CRCByte(NRF24L01_Driver_t *drv, Reg_CONFIG CRC_x)
{
    NRF24L01_ASSERT(drv);
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*清0对应位置*/
    Reg &= ~(1 << 2);
    /*写入参数*/
    Reg |= CRC_x; // 如果是1字节，第2位就是0，如果是两字节第2位就是1
    /*重新写回数据*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, Reg);
    /*更新状态结构体，并检查同步*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, Reg);
}

/********************************************************************
 *简	介：使能中断
 *参	数：中断位，使能还是失能
 *返回值：无
 *说	明：最大重传中断，发送中断，接收中断
 *********************************************************************/
void NRF24L01_EN_Interrupt(NRF24L01_Driver_t *drv, Reg_CONFIG MASK_Int, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*清0对应位置*/
    Reg &= ~MASK_Int; // 写 0 使能中断
    /*判断使能还是失能*/
    if (new_status == NRF24L01_DISABLE) // 使能写0不管，失能写1，芯片规定 0 使能，1失能
        Reg |= MASK_Int;                // 写 1 失能中断
    /*重新写回数据*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, Reg);
}

/********************************************************************
 *简	介：设置接收通道地址
 *参	数：通道X，要设置的地址数组，数组长度
 *返回值：无
 *说	明：数组长度要满足要求
 *********************************************************************/
void NRF24L01_Set_RxAddr(NRF24L01_Driver_t *drv, Reg_RX_ADDR RX_ADDR_Px, uint8_t *Addr, uint8_t Addr_length)
{
    NRF24L01_ASSERT(drv);
    /*空指针检查*/
    if (!Addr)
        return;
    /*确定写入通道，决定最大地址字节数*/
    if ((RX_ADDR_Px == RX_ADDR_P0) || (RX_ADDR_Px == RX_ADDR_P1)) // 通道 0 和通道 1 最多可以写入5字节
    {
        /*获取芯片设置的地址长度*/
        uint8_t Reg_length = (NRF24L01_R_Reg(drv, NRF24L01_SETUP_AW) & 0x03) + 2; // 加2是因为如果是3字节地址，寄存器的值是0x01，四字节地址就是0x02，加2正好是几字节地址,&0x03确保取低2位
        /*确定写入数组长度，防止非法访问*/
        uint8_t count = (Addr_length >= Reg_length) ? Reg_length : Addr_length;
        NRF24L01_W_Regs(drv, RX_ADDR_Px, Addr, count);
    } else // 通道 2 到通道 5 只能写入1字节地址
    {
        if (Addr_length >= 1) // 确保至少有一个数据
            NRF24L01_W_Reg(drv, RX_ADDR_Px, Addr[0]);
    }
}

/********************************************************************
 *简	介：设置目标地址
 *参	数：目标地址数组，数组长度
 *返回值：无
 *说	明：数组长度要满足要求
 *********************************************************************/
void NRF24L01_Set_TxAddr(NRF24L01_Driver_t *drv, uint8_t *Addr, uint8_t Addr_length)
{
    NRF24L01_ASSERT(drv);
    /*检查参数合法*/
    if (Addr_length >= 5) {
        Addr_length = 5;
        NRF24L01_ERR_PRINT(drv, "NRF24L01_Set_TxAddr use 5 ByteLength");
    }
    NRF24L01_W_Regs(drv, NRF24L01_TX_ADDR, Addr, Addr_length);
}

/********************************************************************
 *简	介：获取中断标志位
 *参	数：中断位
 *返回值：无
 *说	明：无
 *********************************************************************/
NRF24L01_FLAG_Status NRF24L01_GetITFLAG(NRF24L01_Driver_t *drv, Reg_STATUS IT_FLAG)
{
    NRF24L01_ASSERT(drv);
    /*检查参数有效*/
    if ((IT_FLAG == MAX_RT) || (IT_FLAG == TX_DS) || (IT_FLAG == RX_DR)) {
        if ((NRF24L01_R_Reg(drv, NRF24L01_STATUS) & IT_FLAG)) // 检查标志位，如果是0，&操作会清0所有位，if不成立
            return NRF24L01_SET;
        else
            return NRF24L01_RESET;
    } else // 参数错误
        return NRF24L01_RESET;
}

/********************************************************************
 *简	介：清除中断标志位
 *参	数：中断位
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_ClearITFLAG(NRF24L01_Driver_t *drv, Reg_STATUS IT_FLAG)
{
    NRF24L01_ASSERT(drv);
    /*检查参数有效*/
    if ((IT_FLAG == MAX_RT) || (IT_FLAG == TX_DS) || (IT_FLAG == RX_DR)) {
        /*读*/
        uint8_t status = NRF24L01_R_Reg(drv, NRF24L01_STATUS);
        /*清除标志位*/
        status |= IT_FLAG; // 写 1 清除标志位
        /*写回寄存器*/
        NRF24L01_W_Reg(drv, NRF24L01_STATUS, status);
    }
}

/********************************************************************
 *简	介：设置接收通道接收字节数【静态载荷】
 *参	数：通道，字节数
 *返回值：无
 *说	明：传入的通道就是对应寄存器的地址
 *********************************************************************/
void NRF24L01_Set_RxNum(NRF24L01_Driver_t *drv, Reg_RX_PW RX_PW_Px, uint8_t count)
{
    NRF24L01_ASSERT(drv);
    /*检查参数合法性*/
    if (count > 32) {
        count = 32;
        NRF24L01_ERR_PRINT(drv, "NRF24L01_Set_RxNum use 32 count");
    }
    NRF24L01_W_Reg(drv, RX_PW_Px, count);
}

/********************************************************************
 *简	介：读取接收通道接收字节数【静态载荷】
 *参	数：通道
 *返回值：无
 *说	明：传入的通道就是对应寄存器的地址
 *********************************************************************/
uint8_t NRF24L01_R_RxNum(NRF24L01_Driver_t *drv, Reg_RX_PW RX_PW_Px)
{
    NRF24L01_ASSERT(drv);
    /*检查参数合法性*/
    if ((RX_PW_Px >= RX_PW_P0) && (RX_PW_Px <= RX_PW_P5)) // 这里寄存器地址是连续的，只需要判断地址的大小即可
        return NRF24L01_R_Reg(drv, RX_PW_Px);
    else
        NRF24L01_ERR_PRINT(drv, "NRF24L01_R_RxNum Inval");
    return 0;
}

/********************************************************************
 *简	介：获取数据来自哪个管道
 *参	数：无
 *返回值：数据管道
 *说	明：无
 *********************************************************************/
uint8_t NRF24L01_GetDataPipe(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t Reg = ((NRF24L01_R_Reg(drv, NRF24L01_STATUS) & RX_P_NO_Config_Bit) >> 1); // 右移一位将数据位对齐到第0位，正好就是对应通道,&操作取出配置位
    if (Reg <= 5)
        return Reg;
    else
        NRF24L01_ERR_PRINT(drv, "NRF24L01_GetDataPipe Inval");
    return 7; // 返回一个错误通道值
}

#if Status_SYNC
/********************************************************************
 *简	介：芯片状态结构体同步【全部】
 *参	数：待更新结构体
 *返 回 值：无
 *说	明：主要用于将芯片的状态实时同步到主控上
 *********************************************************************/
void NRF24L01_SyncStruct_All(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
#if SYNC_INCLUDE_CONFIG_Reg // 【当前工作模式】【CRC长度】
    uint8_t Config_temp = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    // 工作状态
    if (!(Config_temp & 0x02))
        drv->NRF_Status.Cur_Mode = Power_Down; // 掉电模式
    else if (Config_temp & (1 << 0))
        drv->NRF_Status.Cur_Mode = RX_Mode; // 接收模式
    else
        drv->NRF_Status.Cur_Mode = TX_Mode; // 发送模式
    // CRC校验长度
    drv->NRF_Status.Cur_CRC_Length = (Config_temp & (1 << 2)) ? 2 : 1;
#endif

#if SYNC_INCLUDE_EN_AA_Reg // 【通道自动应答使能】
    uint8_t EN_AA_temp = NRF24L01_R_Reg(drv, NRF24L01_EN_AA);
    EN_AA_temp &= 0x3F; // 取低6位
    drv->NRF_Status.EN_Pipe_AA = EN_AA_temp;
#endif

#if SYNC_INCLUDE_EN_RXADDR_Reg // 【接收通道使能】
    uint8_t EN_RX_ADDR_temp = NRF24L01_R_Reg(drv, NRF24L01_EN_RXADDR);
    EN_RX_ADDR_temp &= 0x3F; // 取低6位
    drv->NRF_Status.EN_Pipe_RX = EN_RX_ADDR_temp;
#endif

#if SYNC_INCLUDE_SETUP_AW_Reg // 【接收地址长度】
    uint8_t SETUP_AW_temp = NRF24L01_R_Reg(drv, NRF24L01_SETUP_AW);
    SETUP_AW_temp &= 0x03; // 取低2位
    if (SETUP_AW_temp)
        drv->NRF_Status.Cur_Addr_Length = SETUP_AW_temp + 2; // 加上偏移
    else
        drv->NRF_Status.Cur_Addr_Length = 0;
#endif

#if SYNC_INCLUDE_SETUP_RETR_Reg // 【自动重传次数】【自动重传延迟】
    uint8_t SETUP_RETR_temp = NRF24L01_R_Reg(drv, NRF24L01_SETUP_RETR);
    drv->NRF_Status.Cur_ARC = SETUP_RETR_temp & 0x0F;                        // 取低四位
    drv->NRF_Status.Cur_ARD = (((SETUP_RETR_temp >> 4) & 0x0F) * 250 + 250); // 取高四位
#endif

#if SYNC_INCLUDE_RF_CH_Reg // 【2.4G频道】
    uint8_t RF_CH_temp = NRF24L01_R_Reg(drv, NRF24L01_RF_CH);
    // 2.4G频道
    RF_CH_temp &= 0x7F; // 取低7位
    drv->NRF_Status.Cur_Frequency = (uint16_t)(2400 + RF_CH_temp);
#endif

#if SYNC_INCLUDE_RF_SETUP_Reg // 【空中传输速率】【射频功率】
    uint8_t RF_SETUP_temp = NRF24L01_R_Reg(drv, NRF24L01_RF_SETUP);
    // 空中传输速率
    if (RF_SETUP_temp & RF_DR_1Mbps)
        drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_1Mbps; // 这里直接使用寄存器配置位
    else if (RF_SETUP_temp & RF_DR_2Mbps)
        drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_2Mbps;
    else if (RF_SETUP_temp & RF_DR_250Kbps)
        drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_250Kbps;
    // 射频功率
    if (RF_SETUP_temp & RF_PWR_18dBm)
        drv->NRF_Status.Cur_PA_Control = RF_PWR_18dBm;
    else if (RF_SETUP_temp & RF_PWR_12dBm)
        drv->NRF_Status.Cur_PA_Control = RF_PWR_12dBm;
    else if (RF_SETUP_temp & RF_PWR_6dBm)
        drv->NRF_Status.Cur_PA_Control = RF_PWR_6dBm;
    else if (RF_SETUP_temp & RF_PWR_0dBm)
        drv->NRF_Status.Cur_PA_Control = RF_PWR_0dBm;
#endif

#if SYNC_INCLUDE_DYNPD_Reg // 【通道动态载荷使能】
    uint8_t DYNPD_temp = NRF24L01_R_Reg(drv, NRF24L01_DYNPD);
    DYNPD_temp &= 0x3F; // 取低6位
    drv->NRF_Status.EN_Pipe_DPL = DYNPD_temp;
#endif

#if SYNC_INCLUDE_FEATURE_Reg // 【特殊功能开关】
    uint8_t FEATURE_temp            = NRF24L01_R_Reg(drv, NRF24L01_FEATURE);
    drv->NRF_Status.EN_DYN_ACK_FLAG = (FEATURE_temp & (1 << 0)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
    drv->NRF_Status.EN_ACK_PAY_FLAG = (FEATURE_temp & (1 << 1)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
    drv->NRF_Status.EN_DPL_FLAG     = (FEATURE_temp & (1 << 2)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
#endif
}
#endif

#if Status_SYNC
/********************************************************************
 *简	介：状态结构体检查
 *参	数：驱动结构体，芯片寄存器地址
 *返 回 值：无
 *说	明：直接传入寄存器地址，函数会自动检测寄存器可同步的配置
 *********************************************************************/
void NRF24L01_SyncCheck(NRF24L01_Driver_t *drv, uint8_t Reg_Addr)
{
    NRF24L01_ASSERT(drv);
    drv->Delay_us(20); // 延迟给芯片时间置位
    uint8_t temp = 0;
    switch (Reg_Addr) {
#if SYNC_INCLUDE_CONFIG_Reg
        case NRF24L01_CONFIG: // 检查工作模式和CRC长度
            temp = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
            // 检查CRC长度
            if ((((temp & (1 << 2)) >> 2) + 1) != drv->NRF_Status.Cur_CRC_Length) // 先取出第 2 位，移动到第 0 位，加上 1 的偏移，刚好就是CRC长度
            {
                NRF24L01_Config_Err_Proc(drv, Cur_CRC_Length_Config_Err, "Err: Config CRC Length Failed\r\n");
            }
            // 检查当前工作模式
            temp &= 0x03;        // 取出 0 ，1 位
            if (temp & (1 << 1)) // 芯片处于上电模式
            {
                if (temp & (1 << 0)) // 芯片处于接收模式
                {
                    if (drv->NRF_Status.Cur_Mode != RX_Mode) // 结构体状态与芯片状态不一致
                    {
                        NRF24L01_Config_Err_Proc(drv, Cur_Mode_Config_Err, "Err: Config Mode Config Failed\r\n");
                    }
                } else // 芯片处于发送模式
                {
                    if (drv->NRF_Status.Cur_Mode != TX_Mode) // 结构体状态与芯片状态不一致
                    {
                        NRF24L01_Config_Err_Proc(drv, Cur_Mode_Config_Err, "Err: Config Mode Config Failed\r\n");
                    }
                }
            } else // 芯片处于掉电模式
            {
                if (drv->NRF_Status.Cur_Mode != Power_Down) // 结构体状态与芯片状态不一致
                {
                    NRF24L01_Config_Err_Proc(drv, Cur_Mode_Config_Err, "Err: Config Mode Config Failed\r\n");
                }
            }
            break;
#endif
#if SYNC_INCLUDE_EN_AA_Reg
        case NRF24L01_EN_AA: // 检查通道自动应答使能
            temp = NRF24L01_R_Reg(drv, NRF24L01_EN_AA);
            if ((temp & 0x3F) != drv->NRF_Status.EN_Pipe_AA) {
                NRF24L01_Config_Err_Proc(drv, EN_Pipe_AA_Config_Err, "Err: EN Pipe_AA Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_EN_RXADDR_Reg
        case NRF24L01_EN_RXADDR: // 检查接收通道使能
            temp = NRF24L01_R_Reg(drv, NRF24L01_EN_RXADDR);
            if ((temp & 0x3F) != drv->NRF_Status.EN_Pipe_RX) {
                NRF24L01_Config_Err_Proc(drv, EN_Pipe_RX_Config_Err, "Err: EN Pipe_RX Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_SETUP_AW_Reg
        case NRF24L01_SETUP_AW: // 检查接收通道地址长度
            temp = NRF24L01_R_Reg(drv, NRF24L01_SETUP_AW);
            if (((temp & 0x03) + 2) != drv->NRF_Status.Cur_Addr_Length) {
                NRF24L01_Config_Err_Proc(drv, Cur_Addr_Length_Config_Err, "Err: Cur_Addr_Length Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_SETUP_RETR_Reg
        case NRF24L01_SETUP_RETR: // 检查自动重传次数和自动重传延时
            temp = NRF24L01_R_Reg(drv, NRF24L01_SETUP_RETR);
            // 自动重传
            if ((temp & 0x0F) != drv->NRF_Status.Cur_ARC) {
                NRF24L01_Config_Err_Proc(drv, Cur_ARC_Config_Err, "Err: Cur ARC Config Failed\r\n");
            }
            // 自动重传延时
            if (((temp >> 4) & 0x0F) != ((drv->NRF_Status.Cur_ARD - 250) / 4)) {
                NRF24L01_Config_Err_Proc(drv, Cur_ARD_Config_Err, "Err: Cur ARD Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_RF_CH_Reg
        case NRF24L01_RF_CH: // 检查2.4G频道
            temp = NRF24L01_R_Reg(drv, NRF24L01_RF_CH);
            if ((temp & 0x7F) != ((uint8_t)(drv->NRF_Status.Cur_Frequency - 2400))) {
                NRF24L01_Config_Err_Proc(drv, Cur_Frequency_Config_Err, "Err: Cur Frequency Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_RF_SETUP_Reg
        case NRF24L01_RF_SETUP: // 检查空中传输速率和射频功率,这里直接使用寄存器配置位
            temp = NRF24L01_R_Reg(drv, NRF24L01_RF_SETUP);
            // 空中传输速率
            if ((temp & RF_DR_Config_Bit) != drv->NRF_Status.Cur_Air_Data_Rate) // 取出配置位，因为给结构体赋值时直接用的和寄存器一样的配置位，所以直接判断是不是一样就行
            {
                NRF24L01_Config_Err_Proc(drv, Cur_Air_Data_Rate_Config_Err, "Err: Cur Air_Data_Rate Config Failed\r\n");
            }
            // 射频功率
            if ((temp & RF_PWR_Config_Bit) != drv->NRF_Status.Cur_PA_Control) {
                NRF24L01_Config_Err_Proc(drv, Cur_PA_Control_Config_Err, "Err: Cur PA_Control Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_DYNPD_Reg
        case NRF24L01_DYNPD: // 通道动态包长使能
            temp = NRF24L01_R_Reg(drv, NRF24L01_DYNPD);
            if ((temp & 0x3F) != drv->NRF_Status.EN_Pipe_DPL) {
                NRF24L01_Config_Err_Proc(drv, EN_Pipe_DPL_Config_Err, "Err: EN Pipe_DPL Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_FEATURE_Reg
        case NRF24L01_FEATURE: // 特殊功能使能
            temp = NRF24L01_R_Reg(drv, NRF24L01_FEATURE);
            if ((temp & (1 << 0)) != drv->NRF_Status.EN_DYN_ACK_FLAG) { // 无ACK载荷使能
                NRF24L01_Config_Err_Proc(drv, EN_DYN_ACK_Config_Err, "Err: EN DYN_ACK FEATURE Config Failed\r\n");
            }
            if ((temp & (1 << 1)) != drv->NRF_Status.EN_ACK_PAY_FLAG) { // ACK附带载荷使能
                NRF24L01_Config_Err_Proc(drv, EN_ACK_PAY_Config_Err, "Err: EN ACK_PAY FEATURE Config Failed\r\n");
            }
            if ((temp & (1 << 2)) != drv->NRF_Status.EN_DPL_FLAG) { // 动态载荷使能
                NRF24L01_Config_Err_Proc(drv, EN_DPL_Config_Err, "Err: EN DPL Config FEATURE Failed\r\n");
            }
            break;
#endif
        default:
            break;
    }
}
#endif

/********************************************************************
 *简	介：芯片状态结构体同步【单个】
 *参	数：驱动结构体，芯片寄存器地址，寄存器值
 *返 回 值：无
 *说	明：直接传入寄存器地址，函数会自动检测寄存器可同步的配置
 *********************************************************************/
void NRF24L01_SyncStruct_Single(NRF24L01_Driver_t *drv, uint8_t Reg, uint8_t Reg_Value)
{
    NRF24L01_ASSERT(drv);
    switch (Reg) {
#if SYNC_INCLUDE_CONFIG_Reg // 【当前工作模式】【CRC长度】
        case NRF24L01_CONFIG:
            // 工作状态
            if (!(Reg_Value & 0x02))
                drv->NRF_Status.Cur_Mode = Power_Down; // 掉电模式
            else if (Reg_Value & (1 << 0))
                drv->NRF_Status.Cur_Mode = RX_Mode; // 接收模式
            else
                drv->NRF_Status.Cur_Mode = TX_Mode; // 发送模式
            // CRC校验长度
            drv->NRF_Status.Cur_CRC_Length = (Reg_Value & (1 << 2)) ? 2 : 1;
            break;
#endif

#if SYNC_INCLUDE_EN_AA_Reg // 【通道自动应答使能】
        case NRF24L01_EN_AA:
            Reg_Value &= 0x3F; // 取低6位
            drv->NRF_Status.EN_Pipe_AA = Reg_Value;
            break;
#endif

#if SYNC_INCLUDE_EN_RXADDR_Reg // 【接收通道使能】
        case NRF24L01_EN_RXADDR:
            Reg_Value &= 0x3F; // 取低6位
            drv->NRF_Status.EN_Pipe_RX = Reg_Value;
            break;
#endif

#if SYNC_INCLUDE_SETUP_AW_Reg // 【接收地址长度】
        case NRF24L01_SETUP_AW:
            Reg_Value &= 0x03; // 取低2位
            if (Reg_Value)
                drv->NRF_Status.Cur_Addr_Length = Reg_Value + 2; // 加上偏移
            else
                drv->NRF_Status.Cur_Addr_Length = 0;
            break;
#endif

#if SYNC_INCLUDE_SETUP_RETR_Reg // 【自动重传次数】【自动重传延迟】
        case NRF24L01_SETUP_RETR:
            drv->NRF_Status.Cur_ARC = Reg_Value & 0x0F;                        // 取低四位
            drv->NRF_Status.Cur_ARD = (((Reg_Value >> 4) & 0x0F) * 250 + 250); // 取高四位
            break;
#endif

#if SYNC_INCLUDE_RF_CH_Reg // 【2.4G频道】
        case NRF24L01_RF_CH:
            // 2.4G频道
            Reg_Value &= 0x7F; // 取低7位
            drv->NRF_Status.Cur_Frequency = (uint16_t)(2400 + Reg_Value);
            break;
#endif

#if SYNC_INCLUDE_RF_SETUP_Reg // 【空中传输速率】【射频功率】
        case NRF24L01_RF_SETUP:
            // 空中传输速率
            if (Reg_Value & RF_DR_1Mbps)
                drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_1Mbps; // 这里直接使用寄存器配置位
            else if (Reg_Value & RF_DR_2Mbps)
                drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_2Mbps;
            else if (Reg_Value & RF_DR_250Kbps)
                drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_250Kbps;
            // 射频功率
            if (Reg_Value & RF_PWR_18dBm)
                drv->NRF_Status.Cur_PA_Control = RF_PWR_18dBm;
            else if (Reg_Value & RF_PWR_12dBm)
                drv->NRF_Status.Cur_PA_Control = RF_PWR_12dBm;
            else if (Reg_Value & RF_PWR_6dBm)
                drv->NRF_Status.Cur_PA_Control = RF_PWR_6dBm;
            else if (Reg_Value & RF_PWR_0dBm)
                drv->NRF_Status.Cur_PA_Control = RF_PWR_0dBm;
            break;
#endif

#if SYNC_INCLUDE_DYNPD_Reg // 【通道动态载荷使能】
        case NRF24L01_DYNPD:
            Reg_Value &= 0x3F; // 取低6位
            drv->NRF_Status.EN_Pipe_DPL = Reg_Value;
            break;
#endif

#if SYNC_INCLUDE_FEATURE_Reg // 【特殊功能开关】
        case NRF24L01_FEATURE:
            drv->NRF_Status.EN_DYN_ACK_FLAG = (Reg_Value & (1 << 0)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
            drv->NRF_Status.EN_ACK_PAY_FLAG = (Reg_Value & (1 << 1)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
            drv->NRF_Status.EN_DPL_FLAG     = (Reg_Value & (1 << 2)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
            break;
        default:
            break;
#endif
    }
}
