#ifndef PID_H
#define PID_H
#include <stdint.h>
#include <stdbool.h>
#include "PID_Config.h"

/*------------------------------ �� �� �� �� �� ------------------------------*/
#define PID_Set(PID_struct, Para, Value)                                \
        do                                                              \
        {                                                               \
            if((PID_struct) == (void*)0)  PID_Inval_Err_Proc();        \
            else PID_Config((PID_struct),(Para),(Value));               \
        }while(0)

/*------------------------------ �� �� �� �� �� ------------------------------*/
typedef struct
{
    bool First_Run_Flag;      //�״����б�־λ
    /*��������*/
    float Kp,Ki,Kd;           //����ϵ��,����ϵ��,΢��ϵ��
    float Sp;                 //�趨ֵor����ֵ
    float dt;                 //����
    
    /*״̬����*/
    float pre_Err;            //��һ��PID���
    float pre_I_Value;        //��һ�λ���ֵ
    float pre_Output;         //��һ�����ֵ
    
    /*���ֿ�����*/
    float Weak_I_Rate;        //���ֱ���ʱͬ��˥��ϵ��
    
    /*�����������*/
#if I_Dead_Enable == PID_ENABLE
    float I_Dead_Max;         //������������
    float I_Dead_Min;         //������������
#endif
#if (I_Dead_Act == Decay_I) && (I_Dead_Enable == PID_ENABLE)
    float I_Dead_DecayRate;   //��������˥��ϵ��
#endif
    /*����޷����*/
    float OUT_MAX;       //λ��ʽ�������
    float OUT_MIN;       //λ��ʽ�������
    
    /*΢���˲����*/
#if D_Filter_Enable == PID_ENABLE
    float D_Filter_Rate;      //΢���˲�ϵ��
    float pre_D_Value;        //�ϴ�΢��ֵ
#endif
    /*΢������*/
#if D_MODE_SELECT == D_MODE_FB
    float pre_FB;             //�ϴη���ֵ
#endif
} PID_Typedef;

/*-------------------------- ö �� �� �� �� �� �� �� -------------------------*/
typedef enum
{
    /*��������*/
    PID_PARA_Kp = 0,
    PID_PARA_Ki,
    PID_PARA_Kd,
    PID_PARA_Sp,
    PID_PARA_dt,
    /*���ֿ�����*/
    PID_PARA_Weak_I_Rate,        //���ֱ���ʱͬ��˥��ϵ��
    /*�����������*/
#if I_Dead_Enable == PID_ENABLE
    PID_PARA_I_Dead_Max,         //������������
    PID_PARA_I_Dead_Min,         //������������
    #if I_Dead_Act == Decay_I
    PID_PARA_I_Dead_DecayRate,   //��������˥��ϵ��
    #endif
#endif
    /*����޷����*/
    PID_PARA_OUT_MAX,       //λ��ʽ�������
    PID_PARA_OUT_MIN,       //λ��ʽ�������
#if PID_MODE == PID_MODE_INCR
    PID_PARA_OUT_MAX_INCR,       //����ʽ�������
    PID_PARA_OUT_MIN_INCR,       //����ʽ�������
#endif
    /*΢���˲����*/
#if D_Filter_Enable == PID_ENABLE
    PID_PARA_D_Filter_Rate,      //΢���˲�ϵ��
#endif
}PID_t;

/*---------------------------- P I D �� �� �� �� --------------------------*/
/*������*/
void PID_Inval_Err_Proc(void);
/*���ú���*/
void PID_Config(PID_Typedef *PID_struct,PID_t PID_PARA_X,float Value);               //����PID����
/*��ʼ������*/
void PID_Init(PID_Typedef *PID_struct,float Kp,float Ki,float Kd,float Sp,
                                       float dt,float OUT_MAX,float OUT_MIN);        //PID������ʼ��
/*PID����*/
float PID_Compute(PID_Typedef *PID_struct,float Fb);                                 //����һ��PID����

#endif
