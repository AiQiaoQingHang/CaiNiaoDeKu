#ifndef DRV_AK09911C_DEFINE_H
#define DRV_AK09911C_DEFINE_H

/* 寄存器地址定义 */
#define WIA_1  0x00 // AKM公司ID
#define WIA_2  0x01 // 设备ID
#define INFO_1 0x02 // 设备信息 1
#define INFO_2 0x03 // 设备信息 2
#define ST_1   0x10 // 状态 1
#define HXL    0x11 // X轴测量数据低位
#define HXH    0x12 // X轴测量数据高位
#define HYL    0x13 // Y轴测量数据低位
#define HYH    0x14 // Y轴测量数据高位
#define HZL    0x15 // Z轴测量数据低位
#define HZH    0x16 // Z轴测量数据高位
#define TMPS   0x17 // 空寄存器，没用
#define ST_2   0x18 // 状态 2，判断磁感器溢出
#define CNTL_1 0x30 // 控制寄存器 1，没用
#define CNTL_2 0x31 // 控制寄存器 2，切换工作模式
#define CNTL_3 0x32 // 控制寄存器 3，软复位
#define TSI    0x33 // 测试寄存器，没用
#define ASAX   0x60 // X轴灵敏度调整值，只在Fuse ROM下访问
#define ASAY   0x61 // Y轴灵敏度调整值，只在Fuse ROM下访问
#define ASAZ   0x62 // Z轴灵敏度调整值，只在Fuse ROM下访问

/* 寄存器位定义 */
/* ST_1 */
#define DRDY (1 << 0)   // 数据就绪
#define DOR  (1 << 1) // 数据溢出
#define HSM  (1 << 7) // 高速IIC模式
/* ST_2 */
#define HOFL (1 << 3)
/* CNTL_2 */
#define MODE0 (1 << 0) // 模式选择位
#define MODE1 (1 << 1) // 模式选择位
#define MODE2 (1 << 2) // 模式选择位
#define MODE3 (1 << 3) // 模式选择位
#define MODE4 (1 << 4) // 模式选择位
/* CNTL_3 */
#define SRST (1 << 0) // 软复位

typedef enum {
    PD_Mode = 0,    // 掉电模式
    Single_Mode,    // 单次采集模式
    Con_1_Mode,     // 连续模式1 10Hz
    Con_2_Mode,     // 连续模式2 20Hz
    Con_3_Mode,     // 连续模式3 50Hz
    Con_4_Mode,     // 连续模式4 100Hz
    Self_test_Mode, // 自测模式
    Fuse_Rom_mode,  // FuseRom模式
} MODE_SEL;
#endif