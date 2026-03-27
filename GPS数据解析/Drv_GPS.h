#ifndef DRV_GPS_H
#define DRV_GPS_H
#include <stdint.h>

#define TIMEZONE_OFFSET_HOUR 8 // 时区偏差
#define UTC_YEAR_BASE 2000     // UTC基准年份
#define NMEA_MAX_LEN 210       // 单语句最大长度

#define KNOT_TO_KMH 1.852f    // 速度转换系数：1节(knot) = 1.852 公里/小时
#define LATLON_SCALE 100000   // 经纬度放大系数（度*10^5，如30.12345° → 3012345）
#define SPEED_ANGLE_SCALE 100 // 速度/角度放大倍数（10^2）

/**
 * @brief 错误码
 */
typedef enum
{
    NMEA_Fail = 0,                // 错误
    NMEA_OK = 1,                  // 成功
    NMEA_SUM_ERR_A = -1,          // 【校验和】在遍历范围内没有找到“ * ”
    NMEA_SUM_ERR_B = -2,          // 【校验和】校验和错误
    NMEA_SUM_ERR_C = -3,          // 【校验和】传入参数错误空指针
    NMEA_ANA_ERR_GPRMC = -4,      // 【解析】GPRMC报文解析错误
    NMEA_ANA_ERR_GPRMC_INV = -5,  // 【解析】GPRMC报文定位无效
    NMEA_ANA_ERR_GPRMC_HEAD = -6, // 【解析】GPRMC报文未找到头部
} NMEA_Err;

/**
 * @brief UTC时间结构体
 */
typedef struct
{
    uint8_t Hour;  // 小时 (0-23)
    uint8_t Min;   // 分钟 (0-59)
    uint8_t Sec;   // 秒   (0-59)
    uint8_t Date;  // 日期 (1-31)
    uint8_t Month; // 月份 (1-12)
    uint16_t Year; // 年份 (如2024)
} NMEA_UTC_Time_t;

/**
 * @brief NMEA消息解析结构体
 */
typedef struct
{
    NMEA_UTC_Time_t UTC;         // UTC时间/日期
    uint32_t Latitude;           // 纬度 (度*10^5，如30.12345° → 3012345)
    uint32_t Longitude;          // 经度 (度*10^5，如120.12345° → 12012345)
    uint8_t Nshemi;              // 南北半球 (N/S)
    uint8_t Ewhemi;              // 东西半球 (E/W)
    uint8_t Status;              // 定位状态 (A=有效，V=无效)
    uint32_t Speed_Knots;        // 地面速度 (节，*10^2，如10.5节 → 1050)
    uint32_t Speed_Kmh;          // 地面速度 (公里/小时，*10^2，如19.45km/h → 1945)
    uint32_t Course_True;        // 地面航向 (真北，度*10^2，如360.0° → 36000)
    uint32_t Magnetic_Variation; // 磁偏角 (度*10^2，如11.3° → 1130)
    uint8_t Magnetic_Dir;        // 磁偏角方向 (E/W)
    // uint8_t Mode_Indicator;      // 模式指示 (A/D/E/N)
    uint8_t Is_Valid; // 解析结果是否有效 (0=无效，1=有效),非报文内容，人为设定
} NMEA_Msg_t;

NMEA_Err NMEA_GPRMC_Analysis(NMEA_Msg_t *gpsx, uint8_t *GPS_Data); // GPRMC报文解析

#endif