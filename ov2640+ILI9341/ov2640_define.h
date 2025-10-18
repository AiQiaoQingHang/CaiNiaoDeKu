#ifndef ov2640_DEFINE_H
#define ov2640_DEFINE_H

/*-----------------寄存器------------------*/
// 当选择DSP地址(0XFF=0X00)时,OV2640的DSP寄存器地址映射表
#define OV2640_DSP_R_BYPASS   0x05
#define OV2640_DSP_Qs         0x44
#define OV2640_DSP_CTRL       0x50
#define OV2640_DSP_HSIZE1     0x51
#define OV2640_DSP_VSIZE1     0x52
#define OV2640_DSP_XOFFL      0x53
#define OV2640_DSP_YOFFL      0x54
#define OV2640_DSP_VHYX       0x55
#define OV2640_DSP_DPRP       0x56
#define OV2640_DSP_TEST       0x57
#define OV2640_DSP_ZMOW       0x5A
#define OV2640_DSP_ZMOH       0x5B
#define OV2640_DSP_ZMHH       0x5C
#define OV2640_DSP_BPADDR     0x7C
#define OV2640_DSP_BPDATA     0x7D
#define OV2640_DSP_CTRL2      0x86
#define OV2640_DSP_CTRL3      0x87
#define OV2640_DSP_SIZEL      0x8C
#define OV2640_DSP_HSIZE8     0xC0
#define OV2640_DSP_VSIZE8     0xC1
#define OV2640_DSP_CTRL0      0xC2
#define OV2640_DSP_CTRL1      0xC3
#define OV2640_DSP_R_DVP_SP   0xD3
#define OV2640_DSP_IMAGE_MODE 0xDA
#define OV2640_DSP_RESET      0xE0
#define OV2640_DSP_MS_SP      0xF0
#define OV2640_DSP_SS_ID      0x7F
#define OV2640_DSP_SS_CTRL    0xF8
#define OV2640_DSP_MC_BIST    0xF9
#define OV2640_DSP_MC_AL      0xFA
#define OV2640_DSP_MC_AH      0xFB
#define OV2640_DSP_MC_D       0xFC
#define OV2640_DSP_P_STATUS   0xFE
#define OV2640_DSP_RA_DLMT    0xFF

// 当选择传感器地址(0XFF=0X01)时,OV2640的DSP寄存器地址映射表
#define OV2640_SENSOR_GAIN       0x00
#define OV2640_SENSOR_COM1       0x03
#define OV2640_SENSOR_REG04      0x04
#define OV2640_SENSOR_REG08      0x08
#define OV2640_SENSOR_COM2       0x09
#define OV2640_SENSOR_PIDH       0x0A
#define OV2640_SENSOR_PIDL       0x0B
#define OV2640_SENSOR_COM3       0x0C
#define OV2640_SENSOR_COM4       0x0D
#define OV2640_SENSOR_AEC        0x10
#define OV2640_SENSOR_CLKRC      0x11
#define OV2640_SENSOR_COM7       0x12
#define OV2640_SENSOR_COM8       0x13
#define OV2640_SENSOR_COM9       0x14
#define OV2640_SENSOR_COM10      0x15
#define OV2640_SENSOR_HREFST     0x17
#define OV2640_SENSOR_HREFEND    0x18
#define OV2640_SENSOR_VSTART     0x19
#define OV2640_SENSOR_VEND       0x1A
#define OV2640_SENSOR_MIDH       0x1C
#define OV2640_SENSOR_MIDL       0x1D
#define OV2640_SENSOR_AEW        0x24
#define OV2640_SENSOR_AEB        0x25
#define OV2640_SENSOR_W          0x26
#define OV2640_SENSOR_REG2A      0x2A
#define OV2640_SENSOR_FRARL      0x2B
#define OV2640_SENSOR_ADDVSL     0x2D
#define OV2640_SENSOR_ADDVHS     0x2E
#define OV2640_SENSOR_YAVG       0x2F
#define OV2640_SENSOR_REG32      0x32
#define OV2640_SENSOR_ARCOM2     0x34
#define OV2640_SENSOR_REG45      0x45
#define OV2640_SENSOR_FLL        0x46
#define OV2640_SENSOR_FLH        0x47
#define OV2640_SENSOR_COM19      0x48
#define OV2640_SENSOR_ZOOMS      0x49
#define OV2640_SENSOR_COM22      0x4B
#define OV2640_SENSOR_COM25      0x4E
#define OV2640_SENSOR_BD50       0x4F
#define OV2640_SENSOR_BD60       0x50
#define OV2640_SENSOR_REG5D      0x5D
#define OV2640_SENSOR_REG5E      0x5E
#define OV2640_SENSOR_REG5F      0x5F
#define OV2640_SENSOR_REG60      0x60
#define OV2640_SENSOR_HISTO_LOW  0x61
#define OV2640_SENSOR_HISTO_HIGH 0x62

/*-------------------------配置选项枚举--------------------*/
/*寄存器组选择*/
typedef enum ov2640_RegBank_Sel {
    OV2640_DSP_REGBank    = 0x00,
    OV2640_SENSOR_REGBank = 0x01,
    OV2640_REGBankConfig  = 0xFF,
} ov2640_RegBank_Sel_t;

/*灯光模式选择*/
typedef enum ov2640_Light_Mode {
    ov2640_LIGHT_MODE_AUTO,   // 自动
    ov2640_LIGHT_MODE_SUNNY,  // 晴天
    ov2640_LIGHT_MODE_CLOUDY, // 多云
    ov2640_LIGHT_MODE_OFFICE, // 办公室
    ov2640_LIGHT_MODE_HOME,   // 家
} ov2640_Light_Mode_t;

/*色彩饱和度*/
typedef enum ov2640_Color_Saturation {
    ov2640_COLOR_SATURATION_2,
    ov2640_COLOR_SATURATION_1,
    ov2640_COLOR_SATURATION_0,
    ov2640_COLOR_SATURATION_N1,
    ov2640_COLOR_SATURATION_N2,
} ov2640_Color_Saturation_t;

/*亮度*/
typedef enum ov2640_Brightness {
    ov2640_BRIGHTNESS_2,
    ov2640_BRIGHTNESS_1,
    ov2640_BRIGHTNESS_0,
    ov2640_BRIGHTNESS_N1,
    ov2640_BRIGHTNESS_N2,
} ov2640_Brightness_t;

/*对比度*/
typedef enum ov2640_Contrast {
    ov2640_CONTRAST_2,
    ov2640_CONTRAST_1,
    ov2640_CONTRAST_0,
    ov2640_CONTRAST_N1,
    ov2640_CONTRAST_N2,
} ov2640_Contrast_t;

/*特殊效果*/
typedef enum ov2640_Special_Effect {
    ov2640_SPECIAL_EFFECT_ANTIQUE,     // 复古模式
    ov2640_SPECIAL_EFFECT_BLUISH,      // 偏蓝模式
    ov2640_SPECIAL_EFFECT_GREENISH,    // 偏绿模式
    ov2640_SPECIAL_EFFECT_REDISH,      // 偏红模式
    ov2640_SPECIAL_EFFECT_BW,          // 黑白模式
    ov2640_SPECIAL_EFFECT_NEGATIVE,    // 负片模式
    ov2640_SPECIAL_EFFECT_BW_NEGATIVE, // 黑白负片模式
    ov2640_SPECIAL_EFFECT_NORMAL,      // 正常模式
} ov2640_Special_Effect_t;

/*配置状态*/
typedef enum ov2640_Status_FLAG {
    //配置成功
    Set_Output_Size_OK,
    Set_Light_Mode_OK,
    Set_Color_Saturation_OK,
    Set_Brightness_OK,
    Set_Contrast_OK,
    Set_Special_Effect_OK,
    //参数无效
    Set_Output_Size_EINVAL,
    Set_Light_Mode_EINVAL,
    Set_Color_Saturation_EINVAL,
    Set_Brightness_EINVAL,
    Set_Contrast_EINVAL,
    Set_Special_Effect_EINVAL,
} ov2640_Err_FLAG_t;

#endif