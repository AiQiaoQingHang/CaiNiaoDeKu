#include <math.h>
#include "oled.h"
#include "oled_ziku.h"
#include <stdbool.h>
#include <stdint.h>
#include "string.h"

/**
 * @brief 次方函数
 *
 * @param x 底数
 * @param y 次数
 * @return uint32_t 计算结果
 */
static uint32_t oled_Pow(uint8_t x, uint8_t y)
{
    uint32_t result = 1; // 默认为1
    while (y--)
        result *= x;
    return result;
}

/**
 * @brief OLED初始化
 *
 * @param OLED_Drv 驱动结构体
 * @return true 成功
 * @return false 失败
 */
bool oled_Init(OLED_Driver_t *OLED_Drv)
{
    ERR_CHECK(OLED_Drv->IIC_Generate_Start(), Generate_Start_Err);            // 起始位
    ERR_CHECK(OLED_Drv->IIC_Send_Data(OLED_Drv->oled_adress), Send_Data_Err); // 寻址
    ERR_CHECK(OLED_Drv->IIC_Send_Data(OLED_COMMAND_CONTROL), Send_Data_Err);  // 命令控制

    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xAE), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xD5), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x80), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xA8), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x3F), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xD3), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x00), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x40), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xA1), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xC8), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xDA), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x12), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x81), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xCF), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xD9), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xF1), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xDB), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x30), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xA4), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xA6), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x8D), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x14), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xAF), Send_Data_Err);
    ERR_CHECK(OLED_Drv->IIC_Generate_Stop(), Generate_Stop_Err);
    OLED_Drv->Delay_us(100);
    oled_UpData(OLED_Drv);
    return true;
}

/**
 * @brief 设置光标位置
 *
 * @param OLED_Drv 驱动结构体
 * @param x 横坐标【0-127】
 * @param Page 页数【0-7】
 * @return true 成功
 * @return false 失败
 */
bool oled_Pointer(OLED_Driver_t *OLED_Drv, uint16_t x, uint8_t Page)
{
    ERR_CHECK(OLED_Drv->IIC_Generate_Start(), Generate_Start_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(OLED_Drv->oled_adress), Send_Dev_Addr_Err);    // 寻址
    ERR_CHECK(OLED_Drv->IIC_Send_Data(OLED_COMMAND_CONTROL), Send_Data_Control_Err); // 发送命令

    ERR_CHECK(OLED_Drv->IIC_Send_Data(0xB0 | Page), Send_Data_Err);              // 设置页位置
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x10 | ((x & 0xF0) >> 4)), Send_Data_Err); // 设置X位置高4位
    ERR_CHECK(OLED_Drv->IIC_Send_Data(0x00 | (x & 0x0F)), Send_Data_Err);        // 设置X位置低4位
    ERR_CHECK(OLED_Drv->IIC_Generate_Stop(), Generate_Stop_Err);
    return true;
}

/**
 * @brief 发送数据
 *
 * @param OLED_Drv 驱动结构体
 * @param Data 待数据
 * @param size 数据大小
 * @return true 成功
 * @return false 失败
 */
bool oled_SendData(OLED_Driver_t *OLED_Drv, uint8_t *Data, uint16_t size)
{
    ERR_CHECK(OLED_Drv->IIC_Generate_Start(), Generate_Start_Err);
    ERR_CHECK(OLED_Drv->IIC_Send_Data(OLED_Drv->oled_adress), Send_Dev_Addr_Err); // 寻址
    ERR_CHECK(OLED_Drv->IIC_Send_Data(OLED_DATA_CONTROL), Send_Data_Control_Err); // 表明发送数据
    for (uint16_t i = 0; i < size; i++) {
        ERR_CHECK(OLED_Drv->IIC_Send_Data(Data[i]), Send_Data_Err);
    }
    ERR_CHECK(OLED_Drv->IIC_Generate_Stop(), Generate_Stop_Err);
    return true;
}

/**
 * @brief 更新缓存数据到屏幕
 *
 * @param OLED_Drv 驱动结构体
 * @return true 成功
 * @return false 失败
 */
bool oled_UpData(OLED_Driver_t *OLED_Drv)
{
    for (uint8_t i = 0; i < 8; i++) {
        oled_Pointer(OLED_Drv, 0, i);
        oled_SendData(OLED_Drv, OLED_Drv->oled_buffer[i], 128); // 一次更新一页，循环8次
    }
    return true;
}

/**
 * @brief 清屏
 *
 * @param OLED_Drv 驱动结构体
 * @return true 成功
 * @return false 失败
 */
bool oled_clear(OLED_Driver_t *OLED_Drv)
{
    memset(OLED_Drv->oled_buffer, 0, sizeof(OLED_Drv->oled_buffer));
    oled_UpData(OLED_Drv); // 更新屏幕
    return true;
}

/**
 * @brief 显示字符
 *
 * @param OLED_Drv 驱动结构体
 * @param x 横坐标【0-127】
 * @param y 纵坐标【0-63】
 * @param data 待显示字符
 * @return true 成功
 * @return false 失败
 */
bool oled_showchar(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, char data)
{
    for (uint8_t j = 0; j < 2; j++) {
        for (uint8_t i = 0; i < 8; i++) {
            OLED_Drv->oled_buffer[y / 8 + j][x + i] &= ~(0xFF << (y % 8)); // 和下面一行操作的是同一片像素，实现方式是将1挪到突出下一页的部分，然后取反在与，舍弃低位
            OLED_Drv->oled_buffer[y / 8 + j][x + i] |= ASCLL8x16[16 * (data - ' ') + i + 8 * j] << (y % 8);
            /*y%8计算出距离上一页差多少格，对于字库数据来说，在这一页要写的数据就是字库数据的低位，左移将低位舍弃*/

            OLED_Drv->oled_buffer[y / 8 + 1 + j][x + i] &= ~(0xFF >> (8 - (y % 8))); // 和下面一行操作的是同一片像素，实现方式是将1挪到占此页的部分，然后取反在与，舍弃高位
            OLED_Drv->oled_buffer[y / 8 + 1 + j][x + i] |= ASCLL8x16[16 * (data - ' ') + i + 8 * j] >> (8 - (y % 8));
            /*(8-(y%8))计算出在这一页占了多少格，对于字库数据来说，在这一页要写的数据就是字库数据的高位，右移将低位位舍弃	，实现方式是将0挪到占用此页的部分，然后与*/
            /*要注意字库数据和缓存区数据的对应关系！！！*/
        }
    }
    return true;
}

/**
 * @brief 显示字符串\0结尾
 *
 * @param OLED_Drv 驱动结构体
 * @param x 横坐标【0-127】
 * @param y 纵坐标【0-63】
 * @param data 待显示字符串
 * @return true 成功
 * @return false 失败
 */
bool oled_showstr(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, char *data)
{
    uint8_t i = 0;
    while (*(data + i) != '\0') {
        oled_showchar(OLED_Drv, x + 8 * i, y, *(data + i));
        i++;
    }
    return true;
}

/**
 * @brief 显示图像
 *
 * @param OLED_Drv 驱动结构体
 * @param x 横坐标【0-127】
 * @param y 纵坐标【0-63】
 * @param height 图像高度
 * @param width 图像宽度
 * @param image 图像数组
 * @return true 成功
 * @return false 失败
 */
bool oled_showimage(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, uint8_t height, uint8_t width, const uint8_t *image)
{
    if (x < 128 & y <= 64) {
        for (uint8_t j = 0; j < (height - 1) / 8 + 1; j++) {
            for (uint8_t i = 0; i < width; i++) {
                OLED_Drv->oled_buffer[y / 8 + j][x + i] &= ~(0xFF << (y % 8));                // 具体看oled_showchar
                OLED_Drv->oled_buffer[y / 8 + j][x + i] |= (image[j * width + i] << (y % 8)); // 除8取余可以得到距离上一页的距离，或运算不改变其他位的值

                OLED_Drv->oled_buffer[y / 8 + 1 + j][x + i] &= ~(0xFF >> (8 - (y % 8)));                // 具体看oled_showchar
                OLED_Drv->oled_buffer[y / 8 + 1 + j][x + i] |= (image[j * width + i] >> (8 - (y % 8))); // 得到超出上一页多少
            }
        }
    }
    return true;
}

/**
 * @brief 显示无符号十进制
 *
 * @param OLED_Drv 驱动结构体
 * @param x 横坐标【0-127】
 * @param y 纵坐标【0-63】
 * @param Num 待显示数字
 * @param length 长度
 * @return true 成功
 * @return false 失败
 */
bool oled_showNum(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, uint16_t Num, uint8_t length)
{
    /* Number / OLED_Pow(10, Length - i - 1) % 10 可以十进制提取数字的每一位*/
    /* + '0' 可将数字转换为字符格式*/
    for (uint8_t i = 0; i < length; i++)
        oled_showchar(OLED_Drv, x + 8 * i, y, Num / oled_Pow(10, (length - i - 1)) % 10 + '0');
    return true;
}

/**
 * @brief 显示十六进制
 *
 * @param OLED_Drv 驱动结构体
 * @param x 横坐标【0-127】
 * @param y 纵坐标【0-63】
 * @param HexNum 待显示十六进制数
 * @param length 长度
 * @return true 成功
 * @return false 失败
 */
bool oled_showHexNum(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, uint16_t HexNum, uint8_t length)
{
    uint8_t SingleNumber;
    for (uint8_t i = 0; i < length; i++) {
        /*提取单个数字*/
        SingleNumber = HexNum / (oled_Pow(16, (length - i - 1))) % 16;
        /*判断是否属于0-9，
            +'0'转字符格式*/
        if (SingleNumber < 10) {
            oled_showchar(OLED_Drv, x + 8 * i, y, SingleNumber + '0');
        }
        /*属于10-16，
            +'A'转字符格式*/
        else {
            oled_showchar(OLED_Drv, x + 8 * i, y, SingleNumber - 10 + 'A');
        }
    }
    return true;
}

/**
 * @brief 显示有符号十进制
 *
 * @param OLED_Drv 驱动结构体
 * @param x 横坐标【0-127】
 * @param y 纵坐标【0-63】
 * @param Num 待显示10进制数
 * @param length 长度
 * @return true 成功
 * @return false 失败
 */
bool oled_showSignNum(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, int16_t Num, uint8_t length)
{
    uint16_t Num1;
    /*数字大于0*/
    if (Num > 0) {
        Num1 = Num;
        oled_showchar(OLED_Drv, x, y, '+'); // 显示‘+’号，x坐标会多8位
    }
    /*数字小于0*/
    else {
        Num1 = -Num;                        // 取补码
        oled_showchar(OLED_Drv, x, y, '-'); // 显示‘-’号，x坐标会多8位
    }
    /*在oled_showNum中完成单独取某一位*/
    oled_showNum(OLED_Drv, x + 8, y, Num1, length);
    return true;
}

/**
 * @brief 显示浮点数
 *
 * @param OLED_Drv 驱动结构体
 * @param x 横坐标【0-127】
 * @param y 纵坐标【0-63】
 * @param Number 待显示浮点数
 * @param IntLength 显示整数长度
 * @param FraLength 显示小数长度
 * @return true 成功
 * @return false 失败
 */
bool oled_showFloatNum(OLED_Driver_t *OLED_Drv, uint8_t x, uint8_t y, float Number, uint8_t IntLength, uint8_t FraLength)
{
    /*显示正负号*/
    if (Number > 0) {
        oled_showchar(OLED_Drv, x, y, '+');
    } else {
        Number = -Number;
        oled_showchar(OLED_Drv, x, y, '-');
    }
    /*直接赋值提取整数部分*/
    uint32_t Int = Number;
    /*将小数位乘到整数位*/
    uint32_t Power = oled_Pow(10, FraLength); // 获取10^n次方，用32位，不然最大只能65536
    Number -= (uint32_t)Number;               // 整数部分置0
    uint32_t Fra = round(Number * Power);     // 将小数乘到整数,得到小数部分,用标准库round四舍五入，避免浮点误差
    // 为什么需要四舍五入：1.23存储可能是1.2299999...，经过去整数后变为0.229999...,在将小数移入整数就变为22.999...不符合
    /*处理四舍五入的进位问题*/
    Int += Fra / Power;
    // 假设要显示的数为123.999...，去整数后为0.999...，正常四舍五入后应该为124.00，而此时整数部分已经被提取出去，所以需要看小数最高位（假设为n位），
    // 那就需要小数部分对10^n次方整除，例如小数移入整数后为99.999...,四舍五入为100.000，此时已经等于10^2次方，整除后就是1，实现进位
    /*目前Int里为整数部分，Fra为小数部分*/
    /*显示*/
    oled_showNum(OLED_Drv, x + 8, y, Int, IntLength);                     // 显示整数部分，‘+8’由于多了一个符号
    oled_showchar(OLED_Drv, IntLength * 8 + 8 + x, y, '.');               // 显示小数点
    oled_showNum(OLED_Drv, IntLength * 8 + 8 + 8 + x, y, Fra, FraLength); // 显示小数
    return true;
}
