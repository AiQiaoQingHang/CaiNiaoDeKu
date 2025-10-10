#ifndef NRF24L01_CONFIG_H
#define NRF24L01_CONFIG_H

#define Debug_Enable 1 // Debug模式开关
#define Status_SYNC  1 // 状态同步开关
#if Status_SYNC
/*选择需要同步的寄存器*/
#define SYNC_INCLUDE_CONFIG_Reg     1 // 同步 CONFIG 寄存器         【当前工作模式】【CRC长度】
#define SYNC_INCLUDE_EN_AA_Reg      1 // 同步 EN_AA 寄存器          【通道自动应答使能】
#define SYNC_INCLUDE_EN_RXADDR_Reg  1 // 同步 EN_RXADDR 寄存器      【接收通道使能】
#define SYNC_INCLUDE_SETUP_AW_Reg   1 // 同步 SETUP_AW 寄存器       【接收地址长度】
#define SYNC_INCLUDE_SETUP_RETR_Reg 1 // 同步 SETUP_RETR 寄存器     【自动重传次数】【自动重传延迟】
#define SYNC_INCLUDE_RF_CH_Reg      1 // 同步 RF_CH 寄存器          【2.4G频道】
#define SYNC_INCLUDE_RF_SETUP_Reg   1 // 同步 RF_SETUP 寄存器       【空中传输速率】【射频功率】
#define SYNC_INCLUDE_DYNPD_Reg      1 // 同步 DYNPD 寄存器          【通道动态载荷使能】
#define SYNC_INCLUDE_FEATURE_Reg    1 // 同步 FEATURE 寄存器        【特殊功能开关】

#endif
#endif