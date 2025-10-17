#ifndef PID_H
#define PID_H
#include <stdint.h>
#include <stdbool.h>
#include "PID_Config.h"

/*------------------------------ 配 置 参 数 宏 ------------------------------*/
#define PID_Set(PID_struct, Para, Value)                                \
        do                                                              \
        {                                                               \
            if((PID_struct) == (void*)0)  PID_Inval_Err_Proc();        \
            else PID_Config((PID_struct),(Para),(Value));               \
        }while(0)

/*------------------------------ 驱 动 结 构 体 ------------------------------*/
typedef struct
{
    bool First_Run_Flag;      //首次运行标志位
    /*基本参数*/
    float Kp,Ki,Kd;           //比例系数,积分系数,微分系数
    float Sp;                 //设定值or期望值
    float dt;                 //周期
    
    /*状态变量*/
    float pre_Err;            //上一次PID误差
    float pre_I_Value;        //上一次积分值
    float pre_Output;         //上一次输出值
    
    /*积分抗饱和*/
    float Weak_I_Rate;        //积分饱和时同向衰减系数
    
    /*积分死区相关*/
#if I_Dead_Enable == PID_ENABLE
    float I_Dead_Max;         //积分死区上限
    float I_Dead_Min;         //积分死区下限
#endif
#if (I_Dead_Act == Decay_I) && (I_Dead_Enable == PID_ENABLE)
    float I_Dead_DecayRate;   //积分死区衰减系数
#endif
    /*输出限幅相关*/
    float OUT_MAX;       //位置式输出上限
    float OUT_MIN;       //位置式输出下限
    
    /*微分滤波相关*/
#if D_Filter_Enable == PID_ENABLE
    float D_Filter_Rate;      //微分滤波系数
    float pre_D_Value;        //上次微分值
#endif
    /*微分先行*/
#if D_MODE_SELECT == D_MODE_FB
    float pre_FB;             //上次反馈值
#endif
} PID_Typedef;

/*-------------------------- 枚 举 用 于 参 数 检 查 -------------------------*/
typedef enum
{
    /*基本参数*/
    PID_PARA_Kp = 0,
    PID_PARA_Ki,
    PID_PARA_Kd,
    PID_PARA_Sp,
    PID_PARA_dt,
    /*积分抗饱和*/
    PID_PARA_Weak_I_Rate,        //积分饱和时同向衰减系数
    /*积分死区相关*/
#if I_Dead_Enable == PID_ENABLE
    PID_PARA_I_Dead_Max,         //积分死区上限
    PID_PARA_I_Dead_Min,         //积分死区下限
    #if I_Dead_Act == Decay_I
    PID_PARA_I_Dead_DecayRate,   //积分死区衰减系数
    #endif
#endif
    /*输出限幅相关*/
    PID_PARA_OUT_MAX,       //位置式输出上限
    PID_PARA_OUT_MIN,       //位置式输出下限
#if PID_MODE == PID_MODE_INCR
    PID_PARA_OUT_MAX_INCR,       //增量式输出上限
    PID_PARA_OUT_MIN_INCR,       //增量式输出下限
#endif
    /*微分滤波相关*/
#if D_Filter_Enable == PID_ENABLE
    PID_PARA_D_Filter_Rate,      //微分滤波系数
#endif
}PID_t;

/*---------------------------- P I D 相 关 函 数 --------------------------*/
/*错误处理*/
void PID_Inval_Err_Proc(void);
/*配置函数*/
void PID_Config(PID_Typedef *PID_struct,PID_t PID_PARA_X,float Value);               //配置PID参数
/*初始化函数*/
void PID_Init(PID_Typedef *PID_struct,float Kp,float Ki,float Kd,float Sp,
                                       float dt,float OUT_MAX,float OUT_MIN);        //PID参数初始化
/*PID运算*/
float PID_Compute(PID_Typedef *PID_struct,float Fb);                                 //进行一次PID运算

#endif
