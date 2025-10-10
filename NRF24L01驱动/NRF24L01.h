#ifndef NRF24L01_H
#define NRF24L01_H
#include <stdint.h>
#include "NRF24L01_define.h" //引用NRF24L01寄存器映射与指令宏定义
#include "NRF24L01_Config.h"
/*--------------------------------- 调 试 相 关 -----------------------------------*/
// 这一块主要是调试时检查NRF24L01_Driver_t这个类型传入函数时是否为空指针

#if Debug_Enable
extern uint8_t DRIVER_VOID_ERR;    // NRF24L01_Driver_t空指针标志位
extern uint8_t PRINTF_VOID_ERR;    // 打印函数空指针标志位
void NRF24L01_ASSERT_Failed(void); // 这个函数需要自己添加内容
#define NRF24L01_ASSERT(drv)          \
    do {                              \
        if (!(drv)) {                 \
            DRIVER_VOID_ERR = 1;      \
            NRF24L01_ASSERT_Failed(); \
        }                             \
    } while (0)

// 用于打印错误信息，多芯片挂载时可以在drv里的printf函数中加入一些标识，方便查看是哪个芯片出问题
#define NRF24L01_ERR_PRINT(drv, str)   \
    do {                               \
        if (!(drv))                    \
            DRIVER_VOID_ERR = 1;       \
        else {                         \
            if (!((drv)->My_Printf))   \
                PRINTF_VOID_ERR = 1;   \
            else                       \
                (drv)->My_Printf(str); \
        }                              \
    } while (0)

/*配置错误处理宏*/
#define NRF24L01_Config_Err_Proc(drv, Err_Flag, str)                          \
    do {                                                                      \
        (drv)->Config_Err_FLAG = Err_Flag;                                    \
        if ((drv)->My_Printf != (void *)0) {                                  \
            NRF24L01_ERR_PRINT((drv), str);                                   \
            if ((drv)->Config_Err_CallBack != (void *)0) {                    \
                (drv)->Config_Err_CallBack(drv);                              \
            } else                                                            \
                NRF24L01_ERR_PRINT((drv), "Config_Err_CallBack is NULL\r\n"); \
        }                                                                     \
    } while (0)
#else
#define NRF24L01_ASSERT(drv)                         ((void)0)
#define NRF24L01_ERR_PRINT(drv, str)                 ((void)0)
#define NRF24L01_Config_Err_Proc(drv, Err_Flag, str) ((void)0)
#endif
/*---------------------------------- 结 构 体 同 步 ---------------------------------*/
#if Status_SYNC
#define __NRF24L01_SYNC_Struct(drv, Reg_Addr, Reg_Value)        \
    do {                                                        \
        NRF24L01_SyncStruct_Single((drv), Reg_Addr, Reg_Value); \
        NRF24L01_SyncCheck((drv), Reg_Addr);                    \
    } while (0)
#else
#define __NRF24L01_SYNC_Struct(drv, Reg_Addr, Reg_Value) ((void)0)
#endif

/*---------------------------------- 结 构 体 定 义 ---------------------------------*/
/*NRF控制块定义*/
/*NRF控制块创建宏*/
/*用这个宏定义创建变量，需手动调用状态同步函数*/
#define NRF24L01_Creat_CtrlBlock(Name, Delay_func, Spi_func, W_CE_func, W_CS_func) \
    NRF24L01_Driver_t Name = {                                                     \
        .Delay_us           = Delay_func,                                          \
        .SPI_SwapByte_MODE0 = Spi_func,                                            \
        .NRF24L01_W_CE      = W_CE_func,                                           \
        .NRF24L01_W_CS      = W_CS_func,                                           \
        .MAX_RT_CallBack    = ((void *)0),                                         \
        .TX_DS_CallBack     = ((void *)0),                                         \
        .RX_DR_CallBack     = ((void *)0),                                         \
    }

/*接收数结构体*/
typedef struct {
    uint8_t Data[32]; // 缓存
    uint8_t pipe;     // 管道号
} NRF24L01_Pack_t;

#if Status_SYNC
/*芯片状态结构体*/
typedef struct {
#if SYNC_INCLUDE_CONFIG_Reg
    uint8_t Cur_Mode;       // 当前模式
    uint8_t Cur_CRC_Length; // 当前CRC长度
#endif
#if SYNC_INCLUDE_EN_AA_Reg
    uint8_t EN_Pipe_AA; // 通道自动应答使能     哪一位为 1 代表启用
#endif
#if SYNC_INCLUDE_EN_RXADDR_Reg
    uint8_t EN_Pipe_RX; // 接收通道地址启用     哪一位为 1 代表启用
#endif
#if SYNC_INCLUDE_SETUP_AW_Reg
    uint8_t Cur_Addr_Length; // 当前地址长度
#endif
#if SYNC_INCLUDE_SETUP_RETR_Reg
    uint8_t Cur_ARC;  // 当前自动重传次数
    uint16_t Cur_ARD; // 当前自动重传延时
#endif
#if SYNC_INCLUDE_RF_CH_Reg
    uint16_t Cur_Frequency; // 当前2.4G频道
#endif
#if SYNC_INCLUDE_RF_SETUP_Reg
    uint8_t Cur_Air_Data_Rate; // 当前空中传输速率
    uint8_t Cur_PA_Control;    // 当前射频功率
#endif
#if SYNC_INCLUDE_DYNPD_Reg
    uint8_t EN_Pipe_DPL; // 通道动态载荷使能     哪一位为 1 代表启用
#endif
#if SYNC_INCLUDE_FEATURE_Reg
    uint8_t EN_ACK_PAY_FLAG; // ACK附带载荷使能     【0 禁用】【1 启用】
    uint8_t EN_DYN_ACK_FLAG; // 无ACK载荷使能       【0 禁用】【1 启用】
    uint8_t EN_DPL_FLAG;     // 动态载荷使能        【0 禁用】【1 启用】
#endif
} NRF24L01_Status_t;
#endif

/*芯片驱动结构体*/
/*如果挂载了多个芯片，NRF24L01_Driver_t这个结构体实际上决定了使用哪一个NRF24L01芯片*/
typedef struct NRF24L01_Driver_t NRF24L01_Driver_t;
struct NRF24L01_Driver_t {
    // 硬件抽象层
    void (*NRF24L01_W_CE)(GPIO_Status);     // 写CE
    void (*NRF24L01_W_CS)(GPIO_Status);     // 写CS
    uint8_t (*SPI_SwapByte_MODE0)(uint8_t); // SPI交换单字节，此函数内无需处理 CS 信号
    void (*Delay_us)(uint32_t);             // 微秒延迟
#if Status_SYNC
    // 芯片状态监控
    NRF24L01_Status_t NRF_Status; // 芯片状态结构体
#endif
    // 中断回调函数
    void (*MAX_RT_CallBack)(NRF24L01_Driver_t *); // 最大重发中断回调函数
    void (*TX_DS_CallBack)(NRF24L01_Driver_t *);  // 发送中断回调函数
    void (*RX_DR_CallBack)(NRF24L01_Driver_t *);  // 接收中断回调函数
#if Debug_Enable
    // 配置错误相关
    uint8_t Config_Err_FLAG;                          // 配置错误标志位
    void (*My_Printf)(const char *format, ...);       // 串口打印错误信息
    void (*Config_Err_CallBack)(NRF24L01_Driver_t *); // 配置错误回调函数，在实现此函数时注意清除标志位
#endif
};

/*---------------------------------------N R F 2 4 L 0 1 无 线 模 块 函 数 --------------------------------------*/
/*--------- 结 构 体 相 关 ---------*/
#if Status_SYNC
void NRF24L01_SyncStruct_All(NRF24L01_Driver_t *drv);                                    // 芯片状态结构体同步【全部】
void NRF24L01_SyncStruct_Single(NRF24L01_Driver_t *drv, uint8_t Reg, uint8_t Reg_Value); // 芯片状态结构体同步【单个】
void NRF24L01_SyncCheck(NRF24L01_Driver_t *drv, uint8_t Reg_Addr);                       // 检查配置同步
#endif
/*------- 中 断 服 务 函 数 --------*/
void NRF24L01_IRQHandler(NRF24L01_Driver_t *drv); // 中断服务函数，在内部调用结构体内的回调函数
/*--------- 读 写 寄 存 器 ---------*/
uint8_t NRF24L01_R_Reg(NRF24L01_Driver_t *drv, uint8_t Reg_adress);                                // 读寄存器
uint8_t *NRF24L01_R_Regs(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t *Rece, uint8_t size); // 读寄存器多字节
void NRF24L01_W_Reg(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t Data);                     // 写寄存器
void NRF24L01_W_Regs(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t *Data, uint8_t size);     // 写寄存器多字节
/*---------- 功 能 函 数 ----------*/
void NRF24L01_EnterPowerDown(NRF24L01_Driver_t *drv);                                                       // 进入掉电模式
void NRF24L01_EnterTx(NRF24L01_Driver_t *drv);                                                              // 进入发送模式
void NRF24L01_EnterRx(NRF24L01_Driver_t *drv);                                                              // 进入接收模式
void NRF24L01_EnterStandby_1(NRF24L01_Driver_t *drv);                                                       // 进入待机模式1
void NRF24L01_ClearFIFOTx(NRF24L01_Driver_t *drv);                                                          // 清空FIFOTx
void NRF24L01_ClearFIFORx(NRF24L01_Driver_t *drv);                                                          // 清空FIFORx
void NRF24L01_R_RxFIFO(NRF24L01_Driver_t *drv, NRF24L01_Pack_t *Rece);                                      // 读Rx_FIFO【静态载荷】【动态载荷】
void NRF24L01_W_Tx(NRF24L01_Driver_t *drv, W_TX_Command W_TX, uint8_t *Send, uint8_t size, uint8_t Pipe_X); // 写Tx_FIFO
void NRF24L01_SendData(NRF24L01_Driver_t *drv);                                                             // 发送数据
NRF24L01_FLAG_Status NRF24L01_GetITFLAG(NRF24L01_Driver_t *drv, Reg_STATUS IT_FLAG);                        // 获取中断标志位
void NRF24L01_ClearITFLAG(NRF24L01_Driver_t *drv, Reg_STATUS IT_FLAG);                                      // 清除中断标志位
uint8_t NRF24L01_R_RxNum(NRF24L01_Driver_t *drv, Reg_RX_PW RX_PW_Px);                                       // 读取接收通道接收字节数【静态载荷】
uint8_t NRF24L01_GetDataPipe(NRF24L01_Driver_t *drv);                                                       // 获取数据来自哪个管道
/*---------- 配 置 函 数 ----------*/
void NRF24L01_EN_AutoACK(NRF24L01_Driver_t *drv, Reg_EN_AA EN_AA, NRF24L01_Status new_status);                // 使能自动应答
void NRF24L01_EN_RxAddr(NRF24L01_Driver_t *drv, Reg_EN_RXADDR ERX, NRF24L01_Status new_status);               // 使能接收通道
void NRF24L01_Set_Addr_Width(NRF24L01_Driver_t *drv, Reg_SETUP_AW Address_Width);                             // 设置地址长度
void NRF24L01_SET_RETR(NRF24L01_Driver_t *drv, Reg_SETUP_RETR_ARD ARD, Reg_SETUP_RETR_ARC ARC);               // 设置自动重传延时，自动重传次数
void NRF24L01_SetAir_Data_Rate(NRF24L01_Driver_t *drv, Reg_RF_SETUP RF_DR);                                   // 设置空中数据速率
void NRF24L01_PA_Control(NRF24L01_Driver_t *drv, Reg_RF_SETUP RF_PWR);                                        // 设置射频输出功率
void NRF24L01_SetFrequencyMHz(NRF24L01_Driver_t *drv, uint16_t freqMHz);                                      // 设置2.4g通信频段
void NRF24L01_EN_DPL(NRF24L01_Driver_t *drv, Reg_DYNPD EN_DPL, NRF24L01_Status new_status);                   // 使能通道动态载荷
void NRF24L01_EN_FEATURE(NRF24L01_Driver_t *drv, Reg_FEATURE EN_FEATURE, NRF24L01_Status new_status);         // 使能特殊功能
void NRF24L01_Set_CRCByte(NRF24L01_Driver_t *drv, Reg_CONFIG CRC_x);                                          // 设置几字节CRC校验码
void NRF24L01_EN_Interrupt(NRF24L01_Driver_t *drv, Reg_CONFIG MASK_Int, NRF24L01_Status new_status);          // 使能中断
void NRF24L01_Set_RxAddr(NRF24L01_Driver_t *drv, Reg_RX_ADDR RX_ADDR_Px, uint8_t *Addr, uint8_t Addr_length); // 设置接收通道地址
void NRF24L01_Set_TxAddr(NRF24L01_Driver_t *drv, uint8_t *Addr, uint8_t Addr_length);                         // 设置目标地址
void NRF24L01_Set_RxNum(NRF24L01_Driver_t *drv, Reg_RX_PW RX_PW_Px, uint8_t count);                           // 设置接收通道接收字节数

#endif
