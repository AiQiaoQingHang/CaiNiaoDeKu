#ifndef MMC5983MA_DEFINE_H
#define MMC5983MA_DEFINE_H
/* 寄存器定义 */
#define MMC_REGADDR_XOUT_0         0x00 // X轴
#define MMC_REGADDR_XOUT_1         0x01
#define MMC_REGADDR_YOUT_0         0x02 // Y轴
#define MMC_REGADDR_YOUT_1         0x03
#define MMC_REGADDR_ZOUT_0         0x04 // Z轴
#define MMC_REGADDR_ZOUT_1         0x05
#define MMC_REGADDR_XYZOUT_2       0x06 // 18位模式下各轴多的2位
#define MMC_REGADDR_TOUT           0x07 // 温度
#define MMC_REGADDR_STATUS         0x08
#define MMC_REGADDR_INTERAL_CTRL_0 0x09
#define MMC_REGADDR_INTERAL_CTRL_1 0x0A
#define MMC_REGADDR_INTERAL_CTRL_2 0x0B
#define MMC_REGADDR_INTERAL_CTRL_3 0x0C
#define MMC_REGADDR_PRODUCT_ID     0x2F
/* 设备厂商ID */
#define MMC_PRODUCT_ID 0x30
/* 设备地址 */
#define MMC_IIC_ADDR 0x30
/* 寄存器位定义 */
/* Status */
typedef enum {

    Meas_M_Done   = (1 << 0), // 磁场测量完毕，读数据前检查， 1 有效，写 1 清除相关中断
    Meas_T_Done   = (1 << 1), // 温度测量完成，读数据前检查， 1 有效，写 1 清除相关中断
    OTP_Read_Done = (1 << 4), // 表示芯片能够成功读取其内存
} MMC_Reg_Status;

/* Internal Control 0 */
typedef enum {
    TM_M             = (1 << 0), // 进行磁场测量，设置为 1 开始测量，结束后自动清 0
    TM_T             = (1 << 1), // 进行温度测量，设置为 1 开始测量，结束后自动清 0，与【TM_M】不可同时高
    INT_meas_done_en = (1 << 2), // 写 1 使能测量完成中断，不区分温度与磁场
    Set              = (1 << 3), // 写 1 执行Set操作（500ns），结束后自我清除
    Reset            = (1 << 4), // 写 1 执行Reset操作（500ns），结束后自我清除
    Auto_SR_en       = (1 << 5), // 写 1 使能自动Set/Reset功能
    OTPRead          = (1 << 6), // 写 1 让设备再次读取OTP数据，自动清0
} MMC_Reg_Internal_Ctrl_0;

/* Internal Control 1 */
typedef enum {
    BW0        = (1 << 0), // （BW1，BW0）*测量时间*频率 【（0，0）*8ms*100Hz】
    BW1        = (1 << 1), // 【（0，1）*4ms*200Hz】【（1，0）*2ms*400Hz】【（1，1）*0.5ms*800Hz】
    X_inhibit  = (1 << 2), // 写 1 禁用 X通道
    YZ_inhibit = (1 << 3), // 写 1 禁用 YZ通道
    SW_RST     = (1 << 7), // 写 1 重置传感器，清除所有寄存器，在启动中重新读取OTP，开机提升10ms
} MMC_Reg_Internal_Ctrl_1;

/* Internal Control 2 */
typedef enum {
    Cm_freq_0  = (1 << 0), // 决定芯片在连续测量模式下的频率，基于BW[1:0] = 00
    Cm_freq_1  = (1 << 1), // CM_Freq[2:0] *【(000) 连续关闭】【(001) 1Hz】【(010) 10Hz】【(011) 20Hz】【(100) 50Hz】
    Cm_freq_2  = (1 << 2), // 【(101) 100Hz】【(110,BW = 01) 200Hz】【(111,BW = 11) 1000Hz】
    Cmm_en     = (1 << 3), // 写 1 进入连续模式，CM_Freq[2:0]不能是 000
    Prd_set_0  = (1 << 4), // 决定Set的固定频率
    Prd_set_1  = (1 << 5), // Prd_set [2:0] * 【(000) 1】【(001) 25】【(010) 75】【(011) 100】【(100) 250】
    Prd_set_2  = (1 << 6), // 【(101) 500】【(110) 1000】【(111) 2000】
    En_prd_set = (1 << 7), // 写 1 使能周期Set，需要Auto_SR_en和Cmm_en为 1
} MMC_Reg_Internal_Ctrl_2;

/* Internal Control 3 */
typedef enum {
    St_enp = (1 << 1), // 写 1 会从【正极向负极】施加额外电流，产生额外磁场，可用于检查是否过饱和
    St_enm = (1 << 2), // 写 1 会从【负极向正极】施加额外电流，产生额外磁场，可用于检查是否过饱和
    Spi_3W = (1 << 6), // 写 1 进入三线SPI
} MMC_Reg_Internal_Ctrl_3;

/* 配置 */
typedef enum{
    DataLenMode_16Bit,
    DataLenMode_18Bit,
}MMC_DataLen_Mode;

typedef enum {
    MeasTime_8ms   = (0x00), // 00   BW1，BW0   偏移 0 位
    MeasTime_4ms   = (0x01), // 01
    MeasTime_2ms   = (0x02), // 10
    MeasTime_0p5ms = (0x03), // 11
} BWCfg;

typedef enum {
    Cm_freq_off    = (0x00), // 000   CM_Freq[2:0]  偏移 0 位
    Cm_freq_1Hz    = (0x01), // 001
    Cm_freq_10Hz   = (0x02), // 010
    Cm_freq_20Hz   = (0x03), // 011
    Cm_freq_50Hz   = (0x04), // 100
    Cm_freq_100Hz  = (0x05), // 101
    Cm_freq_200Hz  = (0x06), // 110
    Cm_freq_1000Hz = (0x07), // 111
} Cm_freqCfg;

typedef enum {
    Prd_set_1Cnt    = (0x00 << 4), // 000   Prd_set [2:0] 偏移 4 位
    Prd_set_25Cnt   = (0x01 << 4), // 001
    Prd_set_75Cnt   = (0x02 << 4), // 010
    Prd_set_100Cnt  = (0x03 << 4), // 011
    Prd_set_250Cnt  = (0x04 << 4), // 100
    Prd_set_500Cnt  = (0x05 << 4), // 101
    Prd_set_1000Cnt = (0x06 << 4), // 110
    Prd_set_2000Cnt = (0x07 << 4), // 111
} Prd_setCfg;
#endif