#ifndef PID_CONFIG_H
#define PID_CONFIG_H


/*-------------------------------- 配 置 选 项 -------------------------------*/
/*注：这部分只是为了后面的宏定义更清晰*/
/*基础配置*/
#define PID_ENABLE      1
#define PID_DISABLE     0
/*积分死区动作*/
#define Clear_I         0   //清零积分项
#define Frozen_I        1   //冻结积分项
#define Decay_I         2   //衰减积分项
/*微分工作模式*/
#define D_MODE_FB       0   //微分先行，使用反馈值微分
#define D_MODE_ERR      1   //正常微分，使用误差值微分





/*----------------------------- P I D 行 为 选 择 -----------------------------*/
/*注：这后面都是配置PID行为的*/
/*---------------------------- 积 分 死 区 内 行 为 ---------------------------*/
#define I_Dead_Enable               PID_ENABLE          //是否开启积分死区功能【0 关闭】【1 开启】

#if I_Dead_Enable
#define I_Dead_Act                  Decay_I         //处于积分死区内，积分项行为【0 清零积分项】【1 冻结积分】【2 积分衰减】其他数字正常积分
#endif

/*-------------------------------- 微 分 相 关 -------------------------------*/
#define D_Filter_Enable             PID_ENABLE      //微分滤波
#define D_MODE_SELECT               D_MODE_FB      //微分工作模式
#endif
