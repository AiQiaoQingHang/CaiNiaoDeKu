#ifndef PID_DEFINE_H
#define PID_DEFINE_H
#include <stdint.h>

/*����������λ*/
#define I_Dead_Enable_Config_BitField   (1 << 0)
#define I_Dead_Act_Config_BitField      ((1 << 1) | (1 << 2))
#define D_Filter_Enable_Config_BitField (1 << 3)
#define D_MODE_SELECT_Config_BitField   (1 << 4)
/*��������λ����*/
#define I_Dead_Enable            (1 << 0)              // �Ƿ��������������ܡ�0 �رա���1 ������
#define I_Dead_Act_Clear_I       ((0 << 2) | (1 << 1)) // ������������Ϊ��������֡�0 1��
#define I_Dead_Act_Frozen_I      ((1 << 2) | (0 << 1)) // ������������Ϊ��������֡�1 0��
#define I_Dead_Act_Decay_I       ((1 << 2) | (1 << 1)) // ������������Ϊ��˥�����֡�1 1��
#define D_Filter_Enable          (1 << 3)              // �Ƿ���΢���˲����ܡ�0 �رա���1 ������
#define D_MODE_Select_D_MODE_FB  (0 << 4)              // ΢�ֹ���ģʽ������΢��
#define D_MODE_Select_D_MODE_ERR (1 << 4)              // ΢�ֹ���ģʽ�����΢��

/*-------------------------- ö �� �� �� �� �� �� �� -------------------------*/
typedef enum {
    /*��������*/
    PID_PARA_Kp = 0,
    PID_PARA_Ki,
    PID_PARA_Kd,
    PID_PARA_Sp,
    PID_PARA_dt,
    /*���ֿ�����*/
    PID_PARA_Weak_I_Rate, // ���ֱ���ʱͬ��˥��ϵ��
    /*�����������*/
    PID_PARA_I_Dead_Max,       // ������������
    PID_PARA_I_Dead_Min,       // ������������
    PID_PARA_I_Dead_DecayRate, // ��������˥��ϵ��
    /*����޷����*/
    PID_PARA_OUT_MAX,      // λ��ʽ�������
    PID_PARA_OUT_MIN,      // λ��ʽ�������
    PID_PARA_OUT_MAX_INCR, // ����ʽ�������
    PID_PARA_OUT_MIN_INCR, // ����ʽ�������
    /*΢���˲����*/
    PID_PARA_D_Filter_Rate, // ΢���˲�ϵ��
} PID_Params_t;

typedef enum {
    PID_Feature_I_Dead_Enable = 0, // ������������ʹ��
    PID_Feature_I_Dead_Disable,    // �����������ܽ���
    PID_Feature_I_Dead_Act_Clear,  // ������������Ϊ:�������
    PID_Feature_I_Dead_Act_Frozen, // ������������Ϊ:�������
    PID_Feature_I_Dead_Act_Decay,  // ������������Ϊ:˥������
    PID_Feature_D_Filter_Enable,   // ΢���˲�����ʹ��
    PID_Feature_D_Filter_Disable,  // ΢���˲����ܽ���
    PID_Feature_D_MODE_ERR,        // ΢�ֹ���ģʽѡ��:���΢��
    PID_Feature_D_MODE_FB,         // ΢�ֹ���ģʽѡ��:����΢��
    PID_Feature_Reserved,          // ����λ
} PID_Feature_t;

#endif
