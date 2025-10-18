#include "ov2640.h"
#include "LCD.h"
#include "main.h"

/**
 * @brief ov2640初始化
 */
void ov2640_Init(void)
{
    ov2640_EnterWorkMode(); // 拉低PWDN进入工作模式
    HAL_Delay(50);
    ov2640_Reset(); // 复位
    HAL_Delay(50);
    // for (uint32_t i = 0; i < (sizeof(ov2640_Init_UXGA_Config) / sizeof(ov2640_Init_UXGA_Config[0])); i++) {
    //     ov2640_W_Reg(ov2640_Init_UXGA_Config[i][0], ov2640_Init_UXGA_Config[i][1]);
    // }
    // for (uint32_t i = 0; i < (sizeof(ov2640_Set_RGB565_Config) / sizeof(ov2640_Set_RGB565_Config[0])); i++) {
    //     ov2640_W_Reg(ov2640_Set_RGB565_Config[i][0], ov2640_Set_RGB565_Config[i][1]);
    // }
    ov2640_RGB565_Mode();                                    // 配置RGB565格式
    ov2640_Set_Output_Size(320, 240);                        // 设置输出大小
    ov2640_Set_Light_Mode(ov2640_LIGHT_MODE_SUNNY);          // 设置灯光模式
    ov2640_Set_Color_Saturation(ov2640_COLOR_SATURATION_0);  // 设置色彩饱和度
    ov2640_Set_Brightness(ov2640_BRIGHTNESS_0);              // 设置亮度
    ov2640_Set_Contrast(ov2640_CONTRAST_0);                  // 设置对比度
    ov2640_Set_Special_Effect(ov2640_SPECIAL_EFFECT_NORMAL); // 正常模式
    // 设置RGB低字节优先，不知道为什么只能放在最后
    ov2640_RegBank_Sel(OV2640_DSP_REGBank);
    uint8_t temp = ov2640_R_Reg(0xDA);
    temp |= (1 << 0);
    ov2640_W_Reg(0xDA, temp);
}

/**
 * @brief ov2640进入掉电状态
 */
void ov2640_EnterPowerDown(void)
{
    HAL_GPIO_WritePin(ov2640_PWDN_GPIO_Port, ov2640_PWDN_Pin, GPIO_PIN_SET);
}

/**
 * @brief ov2640进入工作状态
 */
void ov2640_EnterWorkMode(void)
{
    HAL_GPIO_WritePin(ov2640_PWDN_GPIO_Port, ov2640_PWDN_Pin, GPIO_PIN_RESET);
}

/**
 * @brief ov2640复位
 */
void ov2640_Reset(void)
{
    // 硬件复位
    HAL_GPIO_WritePin(ov2640_RES_GPIO_Port, ov2640_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(ov2640_RES_GPIO_Port, ov2640_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
    // 切换到传感器寄存器组
    ov2640_RegBank_Sel(OV2640_SENSOR_REGBank);
    // 软件复位
    ov2640_W_Reg(0x12, 0x80);
    HAL_Delay(50);
}

/**
 * @brief ov2640寄存器【写】
 * @param REG_Addr  寄存器地址
 * @param Data  写入的数据
 */
void ov2640_W_Reg(uint8_t REG_Addr, uint8_t Data)
{
    HAL_I2C_Mem_Write(&hi2c1, ov2640_Addr, REG_Addr, I2C_MEMADD_SIZE_8BIT, &Data, 1, 10);
}

/**
 * @brief ov2640寄存器【读】
 * @param REG_Addr  寄存器地址
 * @return 寄存器数据
 */
uint8_t ov2640_R_Reg(uint8_t Reg_Addr)
{
    uint8_t a = 0;
    HAL_I2C_Master_Transmit(&hi2c1, ov2640_Addr, &Reg_Addr, 1, 10);
    HAL_I2C_Master_Receive(&hi2c1, ov2640_Addr, &a, 1, 10);
    return a;
}
/**
 * @brief 选择寄存器组
 *
 * @param RegBank ：待选寄存器组，DSP和传感器
 */
void ov2640_RegBank_Sel(ov2640_RegBank_Sel_t RegBank)
{
    ov2640_W_Reg(OV2640_REGBankConfig, RegBank);
}

/**
 * @brief       设置ov2640模块输出图像分辨率
 * @param       width : 输出图像宽度，必须是4的倍数
 *              height: 输出图像高度，必须是4的倍数
 * @retval      Set_Output_Size_OK   : 设置ov2640模块输出图像大小成功
 *              Set_Output_Size_EINVAL: 传入参数错误
 */

ov2640_Err_FLAG_t ov2640_Set_Output_Size(uint16_t width, uint16_t height)
{
    uint16_t output_width;
    uint16_t output_height;

    if (((width & (4 - 1)) != 0) || ((height & (4 - 1)) != 0)) {
        return Set_Output_Size_EINVAL;
    }
    output_width  = width >> 2;
    output_height = height >> 2;

    ov2640_W_Reg(OV2640_REGBankConfig, OV2640_DSP_REGBank);
    ov2640_W_Reg(OV2640_DSP_RESET, 0x04);
    ov2640_W_Reg(OV2640_DSP_ZMOW, (uint8_t)(output_width & 0x00FF));
    ov2640_W_Reg(OV2640_DSP_ZMOH, (uint8_t)(output_height & 0x00FF));
    ov2640_W_Reg(OV2640_DSP_ZMHH, ((uint8_t)(output_width >> 8) & 0x03) | ((uint8_t)(output_height >> 6) & 0x04));
    ov2640_W_Reg(OV2640_DSP_RESET, 0x00);
    return Set_Output_Size_OK;
}

/**
 * @brief 设置RGB565格式
 * @note  此函数将图片大小设置为SVGA（800*600），在确定输出图像大小时要注意要小于这个值
 */
void ov2640_RGB565_Mode(void)
{
    for (uint32_t i = 0; i < (sizeof(ov2640_SVGA_RGB565_25fps_Config) / sizeof(ov2640_SVGA_RGB565_25fps_Config[0])); i++) {
        ov2640_W_Reg(ov2640_SVGA_RGB565_25fps_Config[i][0], ov2640_SVGA_RGB565_25fps_Config[i][1]);
    }
}

/**
 * @brief       设置ov2640模块灯光模式
 * @param       mode: ov2640_LIGHT_MOED_AUTO  : 自动
 *                    ov2640_LIGHT_MOED_SUNNY : 晴天
 *                    ov2640_LIGHT_MOED_CLOUDY: 多云
 *                    ov2640_LIGHT_MOED_OFFICE: 办公室
 *                    ov2640_LIGHT_MOED_HOME  : 家
 * @retval      Set_Light_Mode_OK   : 设置ov2640模块灯光模式成功
 *              Set_Light_Mode_EINVAL: 传入参数错误
 */
ov2640_Err_FLAG_t ov2640_Set_Light_Mode(ov2640_Light_Mode_t mode)
{
    switch (mode) {
        case ov2640_LIGHT_MODE_AUTO: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0xC7, 0x00);
            break;
        }
        case ov2640_LIGHT_MODE_SUNNY: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0xC7, 0x40);
            ov2640_W_Reg(0xCC, 0x5E);
            ov2640_W_Reg(0xCD, 0x41);
            ov2640_W_Reg(0xCE, 0x54);
            break;
        }
        case ov2640_LIGHT_MODE_CLOUDY: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0xC7, 0x40);
            ov2640_W_Reg(0xCC, 0x65);
            ov2640_W_Reg(0xCD, 0x41);
            ov2640_W_Reg(0xCE, 0x4F);
            break;
        }
        case ov2640_LIGHT_MODE_OFFICE: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0xC7, 0x40);
            ov2640_W_Reg(0xCC, 0x52);
            ov2640_W_Reg(0xCD, 0x41);
            ov2640_W_Reg(0xCE, 0x66);
            break;
        }
        case ov2640_LIGHT_MODE_HOME: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0xC7, 0x40);
            ov2640_W_Reg(0xCC, 0x42);
            ov2640_W_Reg(0xCD, 0x3F);
            ov2640_W_Reg(0xCE, 0x71);
            break;
        }
        default: {
            return Set_Light_Mode_EINVAL;
        }
    }
    return Set_Light_Mode_OK;
}

/**
 * @brief       设置ov2640模块色彩饱和度
 * @param       saturation: ov2640_COLOR_SATURATION_2: +2
 *                          ov2640_COLOR_SATURATION_1: +1
 *                          ov2640_COLOR_SATURATION_0: 0
 *                          ov2640_COLOR_SATURATION_N1: -1
 *                          ov2640_COLOR_SATURATION_N2: -2
 * @retval      Set_Color_Saturation_OK   : 设置ov2640模块色彩饱和度成功
 *              Set_Color_Saturation_EINVAL: 传入参数错误
 */
ov2640_Err_FLAG_t ov2640_Set_Color_Saturation(ov2640_Color_Saturation_t saturation)
{
    switch (saturation) {
        case ov2640_COLOR_SATURATION_2: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x02);
            ov2640_W_Reg(0x7C, 0x03);
            ov2640_W_Reg(0x7D, 0x68);
            ov2640_W_Reg(0x7D, 0x68);
            break;
        }
        case ov2640_COLOR_SATURATION_1: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x02);
            ov2640_W_Reg(0x7C, 0x03);
            ov2640_W_Reg(0x7D, 0x58);
            ov2640_W_Reg(0x7D, 0x58);
            break;
        }
        case ov2640_COLOR_SATURATION_0: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x02);
            ov2640_W_Reg(0x7C, 0x03);
            ov2640_W_Reg(0x7D, 0x48);
            ov2640_W_Reg(0x7D, 0x48);
            break;
        }
        case ov2640_COLOR_SATURATION_N1: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x02);
            ov2640_W_Reg(0x7C, 0x03);
            ov2640_W_Reg(0x7D, 0x38);
            ov2640_W_Reg(0x7D, 0x38);
            break;
        }
        case ov2640_COLOR_SATURATION_N2: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x02);
            ov2640_W_Reg(0x7C, 0x03);
            ov2640_W_Reg(0x7D, 0x28);
            ov2640_W_Reg(0x7D, 0x28);
            break;
        }
        default: {
            return Set_Color_Saturation_EINVAL;
        }
    }
    return Set_Color_Saturation_OK;
}

/**
 * @brief       设置ATK-MC2640模块亮度
 * @param       brightness: ov2640_BRIGHTNESS_2: +2
 *                          ov2640_BRIGHTNESS_1: +1
 *                          ov2640_BRIGHTNESS_0: 0
 *                          ov2640_BRIGHTNESS_N1: -1
 *                          ov2640_BRIGHTNESS_N2: -2
 * @retval      Set_Brightness_OK   : 设置ov2640模块亮度成功
 *              Set_Brightness_EINVAL: 传入参数错误
 */
ov2640_Err_FLAG_t ov2640_Set_Brightness(ov2640_Brightness_t brightness)
{
    switch (brightness) {
        case ov2640_BRIGHTNESS_2: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x09);
            ov2640_W_Reg(0x7D, 0x40);
            ov2640_W_Reg(0x7D, 0x00);
            break;
        }
        case ov2640_BRIGHTNESS_1: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x09);
            ov2640_W_Reg(0x7D, 0x30);
            ov2640_W_Reg(0x7D, 0x00);
            break;
        }
        case ov2640_BRIGHTNESS_0: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x09);
            ov2640_W_Reg(0x7D, 0x20);
            ov2640_W_Reg(0x7D, 0x00);
            break;
        }
        case ov2640_BRIGHTNESS_N1: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x09);
            ov2640_W_Reg(0x7D, 0x10);
            ov2640_W_Reg(0x7D, 0x00);
            break;
        }
        case ov2640_BRIGHTNESS_N2: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x09);
            ov2640_W_Reg(0x7D, 0x00);
            ov2640_W_Reg(0x7D, 0x00);
            break;
        }
        default: {
            return Set_Brightness_EINVAL;
        }
    }
    return Set_Brightness_OK;
}

/**
 * @brief       设置ov2640模块对比度
 * @param       contrast: ov2640_CONTRAST_2: +2
 *                        ov2640_CONTRAST_1: +1
 *                        ov2640_CONTRAST_0: 0
 *                        ov2640_CONTRAST_N1: -1
 *                        ov2640_CONTRAST_N2: -2
 * @retval      Set_Contrast_OK   : 设置ov2640模块对比度成功
 *              Set_Contrast_EINVAL: 传入参数错误
 */
ov2640_Err_FLAG_t ov2640_Set_Contrast(ov2640_Contrast_t contrast)
{
    switch (contrast) {
        case ov2640_CONTRAST_2: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x07);
            ov2640_W_Reg(0x7D, 0x20);
            ov2640_W_Reg(0x7D, 0x28);
            ov2640_W_Reg(0x7D, 0x0C);
            ov2640_W_Reg(0x7D, 0x06);
            break;
        }
        case ov2640_CONTRAST_1: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x07);
            ov2640_W_Reg(0x7D, 0x20);
            ov2640_W_Reg(0x7D, 0x24);
            ov2640_W_Reg(0x7D, 0x16);
            ov2640_W_Reg(0x7D, 0x06);
            break;
        }
        case ov2640_CONTRAST_0: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x07);
            ov2640_W_Reg(0x7D, 0x20);
            ov2640_W_Reg(0x7D, 0x20);
            ov2640_W_Reg(0x7D, 0x20);
            ov2640_W_Reg(0x7D, 0x06);
            break;
        }
        case ov2640_CONTRAST_N1: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x07);
            ov2640_W_Reg(0x7D, 0x20);
            ov2640_W_Reg(0x7D, 0x1C);
            ov2640_W_Reg(0x7D, 0x2A);
            ov2640_W_Reg(0x7D, 0x06);
            break;
        }
        case ov2640_CONTRAST_N2: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x04);
            ov2640_W_Reg(0x7C, 0x07);
            ov2640_W_Reg(0x7D, 0x20);
            ov2640_W_Reg(0x7D, 0x18);
            ov2640_W_Reg(0x7D, 0x34);
            ov2640_W_Reg(0x7D, 0x06);
            break;
        }
        default: {
            return Set_Contrast_EINVAL;
        }
    }
    return Set_Contrast_OK;
}

/**
 * @brief       设置ov2640模块特殊效果
 * @param       contrast: ov2640_SPECIAL_EFFECT_ANTIQUE    : 复古模式
 *                        ov2640_SPECIAL_EFFECT_BLUISH     : 偏蓝模式
 *                        ov2640_SPECIAL_EFFECT_GREENISH   : 偏绿模式
 *                        ov2640_SPECIAL_EFFECT_REDISH     : 偏红模式
 *                        ov2640_SPECIAL_EFFECT_BW         : 黑白模式
 *                        ov2640_SPECIAL_EFFECT_NEGATIVE   : 负片模式
 *                        ov2640_SPECIAL_EFFECT_BW_NEGATIVE: 黑白负片模式
 *                        ov2640_SPECIAL_EFFECT_NORMAL     : 正常模式
 * @retval      Set_Special_Effect_OK   : 设置ov2640模块特殊效果成功
 *              Set_Special_Effect_EINVAL: 传入参数错误
 */
ov2640_Err_FLAG_t ov2640_Set_Special_Effect(ov2640_Special_Effect_t effect)
{
    switch (effect) {
        case ov2640_SPECIAL_EFFECT_ANTIQUE: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x18);
            ov2640_W_Reg(0x7C, 0x05);
            ov2640_W_Reg(0x7D, 0x40);
            ov2640_W_Reg(0x7D, 0xA6);
            break;
        }
        case ov2640_SPECIAL_EFFECT_BLUISH: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x18);
            ov2640_W_Reg(0x7C, 0x05);
            ov2640_W_Reg(0x7D, 0xA0);
            ov2640_W_Reg(0x7D, 0x40);
            break;
        }
        case ov2640_SPECIAL_EFFECT_GREENISH: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x18);
            ov2640_W_Reg(0x7C, 0x05);
            ov2640_W_Reg(0x7D, 0x40);
            ov2640_W_Reg(0x7D, 0x40);
            break;
        }
        case ov2640_SPECIAL_EFFECT_REDISH: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x18);
            ov2640_W_Reg(0x7C, 0x05);
            ov2640_W_Reg(0x7D, 0x40);
            ov2640_W_Reg(0x7D, 0xC0);
            break;
        }
        case ov2640_SPECIAL_EFFECT_BW: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x18);
            ov2640_W_Reg(0x7C, 0x05);
            ov2640_W_Reg(0x7D, 0x80);
            ov2640_W_Reg(0x7D, 0x80);
            break;
        }
        case ov2640_SPECIAL_EFFECT_NEGATIVE: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x40);
            ov2640_W_Reg(0x7C, 0x05);
            ov2640_W_Reg(0x7D, 0x80);
            ov2640_W_Reg(0x7D, 0x80);
            break;
        }
        case ov2640_SPECIAL_EFFECT_BW_NEGATIVE: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x58);
            ov2640_W_Reg(0x7C, 0x05);
            ov2640_W_Reg(0x7D, 0x80);
            ov2640_W_Reg(0x7D, 0x80);
            break;
        }
        case ov2640_SPECIAL_EFFECT_NORMAL: {
            ov2640_RegBank_Sel(OV2640_DSP_REGBank);
            ov2640_W_Reg(0x7C, 0x00);
            ov2640_W_Reg(0x7D, 0x00);
            ov2640_W_Reg(0x7C, 0x05);
            ov2640_W_Reg(0x7D, 0x80);
            ov2640_W_Reg(0x7D, 0x80);
            break;
        }
        default: {
            return Set_Special_Effect_EINVAL;
        }
    }
    return Set_Special_Effect_OK;
}