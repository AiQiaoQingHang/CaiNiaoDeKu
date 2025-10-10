#include <stdint.h>
#include "PID.h"

/********************************************************************
 *简    介：PID配置参数无效错误函数
 *参    数：无
 *返 回 值：无
 *说    明：当配置PID参数无效时会被调用，用户自己定义
 *********************************************************************/
__weak void PID_Inval_Err_Proc(void)
{
    
}

/********************************************************************
 *简    介：PID参数初始化
 *参    数：PID结构体，比例系数Kp，积分系数Ki，微分系数Kd,设定值or期望值Sp,
                       执行周期dt，输出上限OUT_MAX，输出下限OUT_MIN
 *返 回 值：无
 *说    明：无
 *********************************************************************/
void PID_Init(PID_Typedef *PID_struct, float Kp, float Ki, float Kd, float Sp,
              float dt, float OUT_MAX, float OUT_MIN)
{
    if (PID_struct == (void *)0)
    {
        PID_Inval_Err_Proc();
        return;
    }
    PID_struct->First_Run_Flag = true;
    /*基本参数*/
    PID_struct->Kp = Kp;
    PID_struct->Ki = Ki;
    PID_struct->Kd = Kd;
    PID_struct->Sp = Sp;
    PID_struct->dt = dt;
    /*状态变量*/
    PID_struct->pre_Err = 0.0f;         // 上一次 PID 误差   0
    PID_struct->pre_I_Value = 0.0f;     // 上一次积分 I 值   0
    PID_struct->pre_Output = 0.0f;      // 上一次输出        0
    /*积分抗饱和*/
    PID_struct->Weak_I_Rate = 0.5f; // 积分饱和时同向衰减系数，默认0.5
    /*积分死区相关*/
#if I_Dead_Enable == PID_ENABLE
    PID_struct->I_Dead_Max = 0.0f; // 积分死区上限
    PID_struct->I_Dead_Min = 0.0f; // 积分死区下限
#if I_Dead_Act == 2
    PID_struct->I_Dead_DecayRate = 0.5f; // 积分死区衰减系数
#endif
#endif
    /*输出限幅相关*/
    PID_struct->OUT_MAX = OUT_MAX; // 位置式输出上限
    PID_struct->OUT_MIN = OUT_MIN; // 位置式输出下限
#if D_Filter_Enable == PID_ENABLE
    PID_struct->D_Filter_Rate = 0.5f; // 微分滤波系数,默认0.5
    PID_struct->pre_D_Value = 0.0f;   // 上次微分值
#endif
#if D_MODE_SELECT == D_MODE_FB
    PID_struct->pre_FB = 0.0f; // 上次反馈值
#endif
}

/********************************************************************
 *简    介：修改PID结构体参数
 *参    数：PID结构体，要修改的参数，修改值
 *返 回 值：无
 *说    明：可以使用PID_Set这个宏
 *********************************************************************/
void PID_Config(PID_Typedef *PID_struct, PID_t PID_PARA_X, float Value)
{
    if (PID_struct == (void *)0)
    {
        PID_Inval_Err_Proc();
        return;
    }
    switch (PID_PARA_X)
    {
    case PID_PARA_Kp:
        PID_struct->Kp = Value;
        break;
    case PID_PARA_Ki:
        PID_struct->Ki = Value;
        break;
    case PID_PARA_Kd:
        PID_struct->Kd = Value;
        break;
    case PID_PARA_Sp:
        PID_struct->Sp = Value;
        break;
    case PID_PARA_dt:
        if (Value > 0.0f)
            PID_struct->dt = Value;
        break;
    case PID_PARA_Weak_I_Rate:
        PID_struct->Weak_I_Rate = Value;
        break;
#if I_Dead_Enable == PID_ENABLE
    case PID_PARA_I_Dead_Max:
        PID_struct->I_Dead_Max = Value;
        break;
    case PID_PARA_I_Dead_Min:
        PID_struct->I_Dead_Min = Value;
        break;
#endif

#if (I_Dead_Act == Decay_I) && (I_Dead_Enable == PID_ENABLE)
    case PID_PARA_I_Dead_DecayRate:
        PID_struct->I_Dead_DecayRate = Value;
        break;
#endif

    case PID_PARA_OUT_MAX:
        PID_struct->OUT_MAX = Value;
        break;
    case PID_PARA_OUT_MIN:
        PID_struct->OUT_MIN = Value;
        break;
#if D_Filter_Enable == PID_ENABLE
    case PID_PARA_D_Filter_Rate:
        if (Value <= 0.0f) // 确保滤波系数在0-1之间
            Value = 0.0f;
        else if (Value > 1.0f)
            Value = 1.0f;
        PID_struct->D_Filter_Rate = Value;
        break;
#endif
    default:
        break;
    }
}

/********************************************************************
 *简    介：执行一次PID计算
 *参    数：PID结构体，反馈值Fb
 *返 回 值：无
 *说    明：控制量
 *********************************************************************/
float PID_Compute(PID_Typedef *PID_struct, float Fb)
{
    /*计算当前误差*/
    float cur_Err = PID_struct->Sp - Fb; // 设定值 - 反馈值

    /*----------------------------------------- 计 算 积 分 项 I ----------------------------------------------*/
    float Integral_increments = 0; // 梯形面积，积分增量
    float cur_integral = 0;        // 积分值
    /*-------------------------- 计 算 积 分 增 量 ------------------------------*/
    if ((PID_struct->Ki != 0.0f) && (PID_struct->Ki > 1e-6f))
    {
        /*当前积分项      = 上次积分值 +（（上次误差 + 本次误差）*误差时间）/2
                         = 上次积分值 + 积分增量
                         = 上次积分值 + 梯形面积面积
        */
        /*梯形等效，计算面积，【梯形面积面积 = （（上底 + 下底） * 高）/2】*/
#if I_Dead_Enable == PID_DISABLE // 关闭积分死区功能，正常积分
        Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt;

#else                        // 开启积分死区功能
        /*判断当前是否处于死区内*/
        if (cur_Err >= PID_struct->I_Dead_Min && cur_Err <= PID_struct->I_Dead_Max) // 在死区内
        {
#if I_Dead_Act == Clear_I    // 积分死区内清零积分项
            PID_struct->pre_I_Value = 0;
            Integral_increments = 0;
#elif I_Dead_Act == Frozen_I // 积分死区内冻结
            Integral_increments = 0; // 积分增量为 0，保持上一次积分值
#elif I_Dead_Act == Decay_I  // 积分死区内衰减
            Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt * PID_struct->I_Dead_DecayRate; // 多乘一个衰减系数
#else                        // I_Dead_Act错误参数，正常积分
            Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt;
#endif
        }
        else // 在死区外,正常计算积分增量
        {
            Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt;
        }
#endif
        cur_integral = PID_struct->pre_I_Value + Integral_increments; // 当前积分项 = 上次积分值 + 梯形面积面积
        // 饱和时判断【Integral_increments】是否反向，加速退出饱和状态
        /*---------------------分---------------隔-------------------线----------------*/

        /*----------动态积分限幅，此处的限制为积分值【即 COi / Ki 】---------------*/
        float I_MAX = PID_struct->OUT_MAX / PID_struct->Ki;
        float I_MIN = PID_struct->OUT_MIN / PID_struct->Ki;
        /*   解   当处于饱和状态时，由于开关开到最大，输出接近一个固定值，此时【 P 控制器】和【 D 控制器】随着时间推移占比相对积分会越来越小，积分累计误差会越来越大
            释   最终导致【 I 控制器】处于主导地位，就是说相对 【PID总控制器输出】，【 I 控制器】输出会占很大比重
            公   公式 CO = Cp + Ci + Cd，  Cp 与 Cd 比重较小，抹去得到 CO = Ci = （Ki * I），为了确保【 I 控制器】单独作用也不会
            式   超过 C0 ，那么 Imax = （COmax / Ki），Imin = （COmin / Ki）
        */
        if (cur_integral > I_MAX) // 大于
            cur_integral = I_MAX;
        else if (cur_integral < I_MIN) // 小于
            cur_integral = I_MIN;
        /*---------------------分---------------隔-------------------线----------------*/
    }
    else // Ki为0
    {
        cur_integral = 0.0f; // 积分系数为 0，积分项为 0
    }

    /*------------------------------------------ 计 算 微 分 项 D --------------------------------------*/
    float cur_differ = 0;
    if ((PID_struct->Kd != 0.0f) && (PID_struct->Kd > 1e-6f))
    {
        if (PID_struct->First_Run_Flag == true) // 第一次运行
        {
#if D_MODE_SELECT == D_MODE_ERR
            /*当前微分项 = （当前误差 - 上次误差） / 误差时间  【d（ERR）/dt】*/
            /*tan，正切值，斜率*/
            cur_differ = 0.0f;
#elif D_MODE_SELECT == D_MODE_FB
            /*当前微分项 = -（当前反馈值 - 上次反馈值） / 误差时间  【-d（FB）/dt】*/
            cur_differ = 0.0f;
#endif
        }
        else // 后续运行
        {
#if D_MODE_SELECT == D_MODE_ERR
            cur_differ = (cur_Err - PID_struct->pre_Err) / PID_struct->dt;
#elif D_MODE_SELECT == D_MODE_FB
            cur_differ = -(Fb - PID_struct->pre_FB) / PID_struct->dt;
#endif
        }

#if D_Filter_Enable == PID_ENABLE
        // 互补滤波
        if (PID_struct->First_Run_Flag == false) // 避开第一次执行
            cur_differ = (cur_differ * PID_struct->D_Filter_Rate) + ((1 - PID_struct->D_Filter_Rate) * PID_struct->pre_D_Value);
#endif
    }
    else
    {
        cur_differ = 0.0f; // 微分系数为 0，微分项为 0
    }

    /*------------------------------------------ 计 算 控 制 器 输 出 -----------------------------------*/
    float COp = (PID_struct->Kp * cur_Err);      // P控制器输出
    float COi = (PID_struct->Ki * cur_integral); // I控制器输出
    float COd = (PID_struct->Kd * cur_differ);   // D控制器输出
    float CO = COp + COi + COd;                  // PID控制器总输出

    /*--------------------------------------------- 输 出 限 幅 -----------------------------------------*/
    if (CO >= PID_struct->OUT_MAX) // 超过最大值
    {
        if (Integral_increments < 0.0f) // 允许反向积分，积分增量小于0
        {
            PID_struct->pre_I_Value = cur_integral; // 更新积分值
        }
        else
        {
            PID_struct->pre_I_Value *= PID_struct->Weak_I_Rate; // 同向削弱积分
        }
        CO = PID_struct->OUT_MAX; // 限制最大输出
    }
    else if (CO <= PID_struct->OUT_MIN) // 低于最小值
    {
        if (Integral_increments > 0.0f) // 允许反向积分，积分增量大于0
        {
            PID_struct->pre_I_Value = cur_integral; // 更新积分值
        }
        else
        {
            PID_struct->pre_I_Value *= PID_struct->Weak_I_Rate; // 同向削弱积分
        }
        CO = PID_struct->OUT_MIN; // 限制最小输出
    }
    else // 处于阈值内
    {
        PID_struct->pre_I_Value = cur_integral; // 正常更新积分值
    }

    if (PID_struct->First_Run_Flag == true) // 处理第一次运行标志位
    {
        PID_struct->First_Run_Flag = false;
    }

    /*-------------------------------------- 更 新 结 构 体 内 容 -----------------------------------------*/
    PID_struct->pre_Err = cur_Err; // 保存本次误差
    PID_struct->pre_Output = CO; // 保存本次输出值
#if D_Filter_Enable == PID_ENABLE
    PID_struct->pre_D_Value = cur_differ; // 保存本次微分值
#endif
#if D_MODE_SELECT == D_MODE_FB
    PID_struct->pre_FB = Fb; // 保存本次反馈值
#endif
    return CO;
}
