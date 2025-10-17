#ifndef PID_DEFINE_H
#define PID_DEFINE_H
#include <stdint.h>

/*各功能配置位*/
#define I_Dead_Enable_Config_BitField   (1 << 0)
#define I_Dead_Act_Config_BitField      ((1 << 1) | (1 << 2))
#define D_Filter_Enable_Config_BitField (1 << 3)
#define D_MODE_SELECT_Config_BitField   (1 << 4)
/*功能配置位掩码*/
#define I_Dead_Enable            (1 << 0)              // 是否开启积分死区功能【0 关闭】【1 开启】
#define I_Dead_Act_Clear_I       ((0 << 2) | (1 << 1)) // 积分死区内行为：清除积分【0 1】
#define I_Dead_Act_Frozen_I      ((1 << 2) | (0 << 1)) // 积分死区内行为：冻结积分【1 0】
#define I_Dead_Act_Decay_I       ((1 << 2) | (1 << 1)) // 积分死区内行为：衰减积分【1 1】
#define D_Filter_Enable          (1 << 3)              // 是否开启微分滤波功能【0 关闭】【1 开启】
#define D_MODE_Select_D_MODE_FB  (0 << 4)              // 微分工作模式：反馈微分
#define D_MODE_Select_D_MODE_ERR (1 << 4)              // 微分工作模式：误差微分

/*-------------------------- 枚 举 用 于 参 数 检 查 -------------------------*/
typedef enum {
    /*基本参数*/
    PID_PARA_Kp = 0,
    PID_PARA_Ki,
    PID_PARA_Kd,
    PID_PARA_Sp,
    PID_PARA_dt,
    /*积分抗饱和*/
    PID_PARA_Weak_I_Rate, // 积分饱和时同向衰减系数
    /*积分死区相关*/
    PID_PARA_I_Dead_Max,       // 积分死区上限
    PID_PARA_I_Dead_Min,       // 积分死区下限
    PID_PARA_I_Dead_DecayRate, // 积分死区衰减系数
    /*输出限幅相关*/
    PID_PARA_OUT_MAX,      // 位置式输出上限
    PID_PARA_OUT_MIN,      // 位置式输出下限
    PID_PARA_OUT_MAX_INCR, // 增量式输出上限
    PID_PARA_OUT_MIN_INCR, // 增量式输出下限
    /*微分滤波相关*/
    PID_PARA_D_Filter_Rate, // 微分滤波系数
} PID_Params_t;

typedef enum {
    PID_Feature_I_Dead_Enable = 0, // 积分死区功能使能
    PID_Feature_I_Dead_Disable,    // 积分死区功能禁用
    PID_Feature_I_Dead_Act_Clear,  // 积分死区内行为:清除积分
    PID_Feature_I_Dead_Act_Frozen, // 积分死区内行为:冻结积分
    PID_Feature_I_Dead_Act_Decay,  // 积分死区内行为:衰减积分
    PID_Feature_D_Filter_Enable,   // 微分滤波功能使能
    PID_Feature_D_Filter_Disable,  // 微分滤波功能禁用
    PID_Feature_D_MODE_ERR,        // 微分工作模式选择:误差微分
    PID_Feature_D_MODE_FB,         // 微分工作模式选择:反馈微分
    PID_Feature_Reserved,          // 保留位
} PID_Feature_t;

#endif
