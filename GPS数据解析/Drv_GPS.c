#include "Drv_GPS.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

static uint32_t NMEA_Str2num(uint8_t *buf, uint8_t *dx);                               // 字符串-->数字
static uint32_t NMEA_Pow(uint8_t x, uint8_t y);                                        // x^y次方函数
static bool NMEA_Comma_Pos(uint8_t *Start_Addr, uint8_t **Find_Addr, uint8_t Max_Len); // 查找逗号
static NMEA_Err NMEA_General_CheckSum(uint8_t *GPS_Data);                              // 通用校验和检查
static bool Is_Leap_Year(uint8_t Year);                                                // 判断是否为闰年（用于计算2月的最大天数）
static uint8_t Get_Days_In_Month(uint8_t Month, uint8_t Year);                         // 获取指定月份的最大天数
static void Adjust_UTC2Local_Time(NMEA_UTC_Time_t *utc_time);                          // 调整UTC时间到本地时间

/**
 * @brief 检查校验和
 *
 * @param GPS_Data 报文（不包括'$'）
 * @return NMEA_Err 解析是否成功
 */
NMEA_Err NMEA_General_CheckSum(uint8_t *GPS_Data)
{
    uint16_t i, j;         // 循环计数
    uint16_t CheckSum = 0; // GPS数据实际校验和
    uint16_t CheckData;    // GPS数据理论校验和
    uint8_t ch1, ch2;      // 报文校验码

    /* 空指针检查 */
    if (GPS_Data == NULL)
    {
        return NMEA_SUM_ERR_C; // 传入参数错误
    }
    /* 查找校验结束符*（从索引0开始遍历） */
    for (i = 0; i < NMEA_MAX_LEN; i++)
    {
        if (*(GPS_Data + i) == '*') // 找到*符号，终止遍历
        {
            break;
        }
        // 遇到字符串结束符，直接返回无*的错误
        if (*(GPS_Data + i) == '\0')
        {
            return NMEA_SUM_ERR_A;
        }
    }
    if (i == NMEA_MAX_LEN) // 未找到*符号
        return NMEA_SUM_ERR_A;
    // 检查*后是否有足够的两位校验位（防止越界）
    if ((i + 2) >= NMEA_MAX_LEN || *(GPS_Data + i + 1) == '\0' || *(GPS_Data + i + 2) == '\0')
    {
        return NMEA_SUM_ERR_A;
    }
    /* 计算异或校验和（从索引0开始，到*前一个字符结束） */
    for (j = 0; j < i; j++)
    {
        CheckSum ^= *(GPS_Data + j); // 异或校验
    }
    /* 解析*后的两位十六进制校验码（兼容大小写） */
    ch1 = *(GPS_Data + i + 1);
    ch2 = *(GPS_Data + i + 2);
    // 解析第一位（兼容大写/小写）
    if (ch1 >= '0' && ch1 <= '9')
    {
        CheckData = (ch1 - '0') << 4;
    }
    else if (ch1 >= 'A' && ch1 <= 'F')
    {
        CheckData = (ch1 - 'A' + 10) << 4;
    }
    else if (ch1 >= 'a' && ch1 <= 'f')
    {
        CheckData = (ch1 - 'a' + 10) << 4;
    }
    else
    {
        return NMEA_SUM_ERR_B; // 校验码字符非法
    }
    // 解析第二位（兼容大写/小写）
    CheckData |= (ch2 >= '0' && ch2 <= '9') ? (ch2 - '0') : (ch2 >= 'A' && ch2 <= 'F') ? (ch2 - 'A' + 10)
                                                        : (ch2 >= 'a' && ch2 <= 'f')   ? (ch2 - 'a' + 10)
                                                                                       : 0XFF;
    // 校验第二位是否合法
    if ((ch2 < '0' || ch2 > '9') && (ch2 < 'A' || ch2 > 'F') && (ch2 < 'a' || ch2 > 'f'))
    {
        return NMEA_SUM_ERR_B;
    }
    // 5. 比对校验和
    if (CheckSum == CheckData)
    {
        return NMEA_OK;
    }
    return NMEA_SUM_ERR_B;
}

/**
 * @brief GPRMC报文解析
 *
 * @param gpsx 解析后输出
 * @param GPS_Data GPRMC报文输入
 * @retval 解析是否成功
 */
NMEA_Err NMEA_GPRMC_Analysis(NMEA_Msg_t *gpsx, uint8_t *GPS_Data)
{
    uint8_t dx = 0;                                            // 小数点个数
    uint8_t *Start_Addr = NULL;                                // 起始地址
    uint8_t *Find_Addr = NULL;                                 // 查找到的逗号地址
    const uint8_t Single_FindLen = 20;                         // 查找逗号函数单次最长查找长度
    uint32_t UTC_Temp = 0, Temp = 0, Degrees = 0, Minutes = 0; // UTC临时变量、临时变量、角、分
    bool Find_Rs = false;                                      // 查找结果
    NMEA_Err Check_Ret = NMEA_Fail;                            // 校验和结果
    /* 参数检查 */
    if (gpsx == NULL || GPS_Data == NULL)
    {
        return NMEA_ANA_ERR_GPRMC;
    }

    /* 查找起始地址 */
    Start_Addr = (uint8_t *)strstr((const char *)GPS_Data, "GPRMC");
    if (Start_Addr == NULL) // 未找到头部
    {
        return NMEA_ANA_ERR_GPRMC_HEAD;
    }
    Check_Ret = NMEA_General_CheckSum(Start_Addr);
    switch (Check_Ret)
    {
    case NMEA_OK: // 解析成功
        break;
    case NMEA_SUM_ERR_A: // 【校验和】在遍历范围内没有找到“ * ”
        gpsx->Is_Valid = 0;
        return NMEA_SUM_ERR_A;
    case NMEA_SUM_ERR_B: // 【校验和】校验和错误
        gpsx->Is_Valid = 0;
        return NMEA_SUM_ERR_B;
    case NMEA_SUM_ERR_C: // 【校验和】传入参数错误
        gpsx->Is_Valid = 0;
        return NMEA_SUM_ERR_C;
    default:
        gpsx->Is_Valid = 0;
        return NMEA_Fail;
    }

    /* 解析UTC时间【时、分、秒】 */
    Find_Rs = NMEA_Comma_Pos(Start_Addr, &Find_Addr, Single_FindLen); // 这里Find_Addr会被替换为第一个逗号位置
    if (Find_Rs != false)
    {
        UTC_Temp = NMEA_Str2num(Find_Addr, &dx) / NMEA_Pow(10, dx); // 得到UTC时间,去掉ms,更新结构体在下面统一更新
    }

    /* 检查定位是否有效 */
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen);
    if (Find_Rs == true) // 查找逗号成功
    {
        if ((*(Find_Addr) == 'A' || *(Find_Addr) == 'a')) // 定位有效
        {
            gpsx->Status = 'A';
        }
        else // 定位无效
        {
            gpsx->Status = 'V';
            gpsx->Is_Valid = 0;
            return NMEA_ANA_ERR_GPRMC_INV;
        }
    }
    else // 查找逗号失败
    {
        gpsx->Is_Valid = 0;
        return NMEA_ANA_ERR_GPRMC_INV;
    }

    /* 解析纬度 */
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen); // 在上一次基础上继续向后查找
    if (Find_Rs != false)
    {
        Temp = NMEA_Str2num(Find_Addr, &dx);
        // 度分格式：ddmm.mmmm → 度 + 分/60
        Degrees = Temp / NMEA_Pow(10, dx + 2); // 提取度
        Minutes = Temp % NMEA_Pow(10, dx + 2); // 提取分（含小数）
        // 转换为度*10^5：degrees*10^5 + (minutes*10^5)/(60*10^dx)
        gpsx->Latitude = Degrees * LATLON_SCALE + (Minutes * LATLON_SCALE) / (60 * NMEA_Pow(10, dx));
    }

    /* 判断南纬还是北纬 */
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen);
    if (Find_Rs != false)
        gpsx->Nshemi = *(Find_Addr);

    /* 解析经度 */
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen);
    if (Find_Rs != false)
    {
        Temp = NMEA_Str2num(Find_Addr, &dx);
        Degrees = Temp / NMEA_Pow(10, dx + 2); // 提取度（dddmm.mmmm）
        Minutes = Temp % NMEA_Pow(10, dx + 2); // 提取分
        gpsx->Longitude = Degrees * LATLON_SCALE + (Minutes * LATLON_SCALE) / (60 * NMEA_Pow(10, dx));
    }

    /* 判断东经还是西经 */
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen);
    if (Find_Rs != false)
        gpsx->Ewhemi = *(Find_Addr);

    /* 解析地面速度 */
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen);
    if (Find_Rs != false)
    {
        Temp = NMEA_Str2num(Find_Addr, &dx);
        // 速度（节），定点放大 SPEED_ANGLE_SCALE 倍（100）
        gpsx->Speed_Knots = (Temp * SPEED_ANGLE_SCALE) / NMEA_Pow(10, dx);
        // 速度（公里/小时），定点放大 SPEED_ANGLE_SCALE 倍（100）
        // 转换因子 1.852 = 1852 / 1000，乘以放大因子 100 得 1852 / 10
        gpsx->Speed_Kmh = (Temp * 1852) / (NMEA_Pow(10, dx) * 10);
    }

    /* 解析地面航向 */
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen);
    if (Find_Rs != false)
    {
        Temp = NMEA_Str2num(Find_Addr, &dx);
        // 直接进行整数运算：Temp * 100 / 10^dx
        gpsx->Course_True = (Temp * SPEED_ANGLE_SCALE) / NMEA_Pow(10, dx);
    }

    /* 解析UTC日期【日、月、年】*/
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen);
    if (Find_Rs != false)
    {
        // 这里UTC_Temp的值没有被改变，为【时、分、秒】
        gpsx->UTC.Hour = UTC_Temp / 10000;
        gpsx->UTC.Min = (UTC_Temp / 100) % 100;
        gpsx->UTC.Sec = UTC_Temp % 100;
        // 这里UTC_Temp的值被修改为【日、月、年】
        UTC_Temp = NMEA_Str2num(Find_Addr, &dx); // 得到UTC日期
        gpsx->UTC.Date = UTC_Temp / 10000;
        gpsx->UTC.Month = (UTC_Temp / 100) % 100;
        gpsx->UTC.Year = UTC_YEAR_BASE + UTC_Temp % 100;
        // 处理时区偏差
        Adjust_UTC2Local_Time(&gpsx->UTC);
    }
    /* 解析磁偏角 */
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen);
    if (Find_Rs != false)
    {
        Temp = NMEA_Str2num(Find_Addr, &dx);
        gpsx->Magnetic_Variation = (Temp * SPEED_ANGLE_SCALE) / NMEA_Pow(10, dx);
    }

    /* 解析磁偏角方向 */
    Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen);
    if (Find_Rs != false)
    {
        gpsx->Magnetic_Dir = *(Find_Addr);
    }

    // /* 解析模式指示（扩展字段） */
    // Find_Rs = NMEA_Comma_Pos(Find_Addr, &Find_Addr, Single_FindLen2);
    // if (Find_Rs != false)
    // {
    //     gpsx->Mode_Indicator = *(Find_Addr);
    // }

    /* 标记解析结果有效 */
    gpsx->Is_Valid = 1;
    return NMEA_OK;
}

/**
 * @brief 计算x的y次方（用于数值转换）
 * @param x: 底数
 * @param y: 幂次
 * @retval x^y的结果
 */
uint32_t NMEA_Pow(uint8_t x, uint8_t y)
{
    uint32_t res = 1;
    for (uint8_t i = 0; i < y; i++)
    {
        res *= x;
    }
    return res;
}

/**
 * @brief 查找逗号
 *
 * @param Start_Addr 起始地址
 * @param Find_Addr 找到的逗号后第一个字符的地址
 * @param Max_Len 最大查找长度
 * @return true 查找成功
 * @return false 查找失败
 */
bool NMEA_Comma_Pos(uint8_t *Start_Addr, uint8_t **Find_Addr, uint8_t Max_Len)
{
    /* 参数检查 */
    if (Start_Addr == NULL || Find_Addr == NULL || Max_Len <= 0)
    {
        return false;
    }

    for (uint8_t i = 0; i < Max_Len; i++)
    {
        if (Start_Addr[i] == ',')
        {
            if (Start_Addr[i + 1] == '\0' || Start_Addr[i + 1] == '*')
                return false;                // 确保逗号后值还有效
            *Find_Addr = Start_Addr + i + 1; // 返回逗号后第一个字符的位置
            return true;
        }
        if (Start_Addr[i] == '\0' || Start_Addr[i] == '*')
        {
            return false;
        }
    }
    return false;
}

/**
 * @brief 将字符串转换为数字（提取数字部分，忽略非数字字符）
 * @param buf: 字符串缓冲区
 * @param dx: 输出参数，记录小数点后的位数
 * @retval 转换后的整数（包含小数点后数字，如"123.45" → 12345，dx=2）
 */
uint32_t NMEA_Str2num(uint8_t *buf, uint8_t *dx)
{
    uint32_t res = 0;
    uint8_t dec_point = 0; // 标记是否遇到小数点
    *dx = 0;               // 初始化小数位数

    if (buf == NULL || dx == NULL)
    {
        return 0;
    }

    for (uint8_t i = 0; i < 15; i++) // 最大15数字长度
    {
        if (buf[i] >= '0' && buf[i] <= '9') // 数字字符
        {
            res = res * 10 + (buf[i] - '0');
            if (dec_point) // 小数点后数字，记录位数
            {
                (*dx)++;
                // 限制小数位数最多8位，防止溢出
                if (*dx > 8)
                {
                    break;
                }
            }
        }
        else if (buf[i] == '.') // 遇到小数点
        {
            dec_point = 1;
        }
        else // 非数字/小数点，停止转换
        {
            break;
        }
    }
    return res;
}

/**
 * @brief 判断是否为闰年（用于计算2月的最大天数）
 * @param year 年份（后两位，如26=2026）
 * @return true闰年，false平年
 */
static bool Is_Leap_Year(uint8_t Year)
{
    uint16_t full_year = UTC_YEAR_BASE + Year;
    // 闰年规则：能被4整除但不能被100整除，或能被400整除
    if ((full_year % 4 == 0 && full_year % 100 != 0) || (full_year % 400 == 0))
    {
        return true;
    }
    return false;
}

/**
 * @brief 获取指定月份的最大天数
 * @param month 月份（1-12）
 * @param year 年份（后两位，如26=2026）
 * @return 当月最大天数（1月31天，2月28/29天等）
 */
static uint8_t Get_Days_In_Month(uint8_t Month, uint8_t Year)
{
    switch (Month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31; // 大月31天
    case 4:
    case 6:
    case 9:
    case 11:
        return 30; // 小月30天
    case 2:
        // 2月：闰年29天，平年28天
        return Is_Leap_Year(Year) ? 29 : 28;
    default:
        return 31; // 异常月份默认31天
    }
}

/**
 * @brief 调整UTC时间到本地时间（处理时区偏移+日期溢出）
 * @param utc_time 原始UTC时间
 */
static void Adjust_UTC2Local_Time(NMEA_UTC_Time_t *utc_time)
{
    /* 保存原始UTC小时，用于判断日期是否需要增减 */
    uint8_t original_hour = utc_time->Hour;
    /* 计算本地小时（UTC小时 + 时区偏移）*/
    int local_hour = original_hour + TIMEZONE_OFFSET_HOUR;
    int day_offset = 0; // 日期偏移量（+1=加1天，-1=减1天）
    /* 处理小时溢出，确定日期偏移 */
    if (local_hour >= 24)
    {
        // 示例：UTC 23:00 +8 → 31:00 → 本地7:00，日期+1
        local_hour -= 24;
        day_offset = 1;
    }
    else if (local_hour < 0)
    {
        // 示例：UTC 1:00 -5 → -4:00 → 本地20:00，日期-1
        local_hour += 24;
        day_offset = -1;
    }
    // 更新本地小时
    utc_time->Hour = (uint8_t)local_hour;

    // 4. 处理日期偏移（核心：处理月末溢出）
    if (day_offset != 0)
    {
        uint8_t new_day = utc_time->Date + day_offset;
        uint8_t current_month = utc_time->Month;
        uint8_t current_year = utc_time->Year;
        uint8_t max_days = Get_Days_In_Month(current_month, current_year);

        // 情况1：日期+1后超过当月最大天数（如1月31日+1→2月1日）
        if (new_day > max_days)
        {
            utc_time->Date = 1;
            utc_time->Month += 1;
            // 月份溢出（12月+1→1月，年份+1）
            if (utc_time->Month > 12)
            {
                utc_time->Month = 1;
                utc_time->Year += 1;
            }
        }
        // 情况2：日期-1后小于1（如1月1日-1→12月31日）
        else if (new_day < 1)
        {
            utc_time->Month -= 1;
            // 月份小于1（1月-1→12月，年份-1）
            if (utc_time->Month < 1)
            {
                utc_time->Month = 12;
                utc_time->Year -= 1;
            }
            // 取新月份的最大天数（如1月1日-1→12月31日）
            utc_time->Date = Get_Days_In_Month(utc_time->Month, current_year);
        }
        // 情况3：日期在正常范围内（如2月15日+1→2月16日）
        else
        {
            utc_time->Date = new_day;
        }
    }
}