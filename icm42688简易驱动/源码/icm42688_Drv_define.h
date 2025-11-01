#ifndef icm42688_DEFINE_H
#define icm42688_DEFINE_H
/*icm42688寄存器地址宏定义*/
/*注意：这里只列举了寄存器组【0】的寄存器地址*/
#define DRIVE_CONFIG_0 0x13      // 设置IIC电平翻转等
#define INTF_CONFIG_1 0x4D       // 设置加速度计时钟
#define icm42688_PWR 0x4E        // 陀螺仪，加速度计工作模式
#define GYRO_CONFIG_0 0x4F       // 陀螺仪相关参数
#define ACCEL_CONFIG_0 0x50      // 加速度计相关参数
#define GYRO_CONFIG_1 0x51       // 温度计滤波，陀螺仪滤波器顺序
#define GYRO_ACCEL_CONFIG_0 0x52 // 陀螺仪和加速度计滤波参数
#define ACCEL_CONFIG_1 0x53      // 加速度计滤波器顺序
#define WHO_AM_I 0x75            // 设备号
#define REG_BANK_SEL 0x76        // 选择寄存器组

/*------------------------------- 数 据 ---------------------------------*/
#define icm42688_TEMP_OUT_H 0x1D // 温度
#define icm42688_TEMP_OUT_L 0x1E

#define icm42688_ACCEL_XOUT_H 0x1F // 加速度
#define icm42688_ACCEL_XOUT_L 0x20
#define icm42688_ACCEL_YOUT_H 0x21
#define icm42688_ACCEL_YOUT_L 0x22
#define icm42688_ACCEL_ZOUT_H 0x23
#define icm42688_ACCEL_ZOUT_L 0x24

#define icm42688_GYRO_XOUT_H 0x25 // 转速陀螺仪
#define icm42688_GYRO_XOUT_L 0x26
#define icm42688_GYRO_YOUT_H 0x27
#define icm42688_GYRO_YOUT_L 0x28
#define icm42688_GYRO_ZOUT_H 0x29
#define icm42688_GYRO_ZOUT_L 0x2A

#endif
