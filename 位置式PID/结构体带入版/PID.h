#ifndef PID_H
#define PID_H
#include <stdint.h>
#include <stdbool.h>
#include "PID_define.h"

/*------------------------------- 参 数 检 查 --------------------------------*/
#define BitCheck(Value, Bit)                    ((Value) & (Bit)) // 位检查
#define PID_FeatureCheck(PID_FeatureUnion, Bit) BitCheck((PID_FeatureUnion)->All_Bits, (Bit))
/*------------------------------ 配 置 参 数 宏 ------------------------------*/
#define PID_ParaSet(PID_Paramsstruct, PID_PARA, Value)              \
    do {                                                            \
        if ((PID_Paramsstruct) == (void *)0)                        \
            PID_Inval_Err_Proc();                                   \
        else                                                        \
            PID_ParamsCfg((PID_Paramsstruct), (PID_PARA), (Value)); \
    } while (0)

#define PID_FeatureSet(PID_FeatureUnion, PID_Feature)          \
    do {                                                       \
        if ((PID_FeatureUnion) == (void *)0)                   \
            PID_Inval_Err_Proc();                              \
        else                                                   \
            PID_FeatureCfg((PID_FeatureUnion), (PID_Feature)); \
    } while (0)

/*------------------------------ 驱 动 结 构 体 ------------------------------*/
/*基础参数*/
typedef struct
{
    bool First_Run_Flag; // 首次运行标志位
    /*基本参数*/
    float Kp, Ki, Kd; // 比例系数,积分系数,微分系数
    float Sp;         // 设定值or期望值
    float dt;         // 周期
    /*状态变量*/
    float pre_Err;     // 上一次PID误差
    float pre_I_Value; // 上一次积分值
    float pre_Output;  // 上一次输出值
    /*积分抗饱和*/
    float Weak_I_Rate; // 积分饱和时同向衰减系数
    /*积分死区相关*/
    float I_Dead_Max;       // 积分死区上限
    float I_Dead_Min;       // 积分死区下限
    float I_Dead_DecayRate; // 积分死区衰减系数
    /*输出限幅相关*/
    float OUT_MAX; // 位置式输出上限
    float OUT_MIN; // 位置式输出下限
    /*微分滤波相关*/
    float D_Filter_Rate; // 微分滤波系数
    float pre_D_Value;   // 上次微分值
    /*微分先行*/
    float pre_FB; // 上次反馈值
} PID_ParamsTypedef;

/*位域*/
typedef struct
{
    uint8_t I_Dead_Enable_Bits : 1;   // 积分死区功能使能
    uint8_t I_Dead_Act_Bits : 2;      // 积分死区内行为
    uint8_t D_Filter_Enable_Bits : 1; // 微分滤波功能使能
    uint8_t D_MODE_SELECT_Bits : 1;   // 微分工作模式选择
    uint8_t Reserved_Bits : 3;        // 保留位
} PID_Config_BitField;

/*PID行为*/
typedef union {
    uint8_t All_Bits;
    PID_Config_BitField bit;
} PID_FeaturesTypedef, PID_Feature_Union;

/*---------------------------- P I D 相 关 函 数 --------------------------*/
/*错误处理*/
void PID_Inval_Err_Proc(void);
/*配置函数*/
void PID_ParamsCfg(PID_ParamsTypedef *PID_struct, PID_Params_t PID_PARA_X, float Value); // 配置PID参数
void PID_FeatureCfg(PID_FeaturesTypedef *PID_Union, PID_Feature_t Value);                // 配置PID功能选项
/*初始化函数*/
void PID_Init(PID_ParamsTypedef *PID_struct, float Kp, float Ki, float Kd, float Sp,
              float dt, float OUT_MAX, float OUT_MIN); // PID参数初始化
/*PID运算*/
float PID_Compute(PID_ParamsTypedef *PID_struct, PID_FeaturesTypedef *PID_Union, float Fb); // 进行一次PID运算

#endif
