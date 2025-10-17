#ifndef PID_H
#define PID_H
#include <stdint.h>
#include <stdbool.h>
#include "PID_define.h"

/*------------------------------- �� �� �� �� --------------------------------*/
#define BitCheck(Value, Bit)                    ((Value) & (Bit)) // λ���
#define PID_FeatureCheck(PID_FeatureUnion, Bit) BitCheck((PID_FeatureUnion)->All_Bits, (Bit))
/*------------------------------ �� �� �� �� �� ------------------------------*/
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

/*------------------------------ �� �� �� �� �� ------------------------------*/
/*��������*/
typedef struct
{
    bool First_Run_Flag; // �״����б�־λ
    /*��������*/
    float Kp, Ki, Kd; // ����ϵ��,����ϵ��,΢��ϵ��
    float Sp;         // �趨ֵor����ֵ
    float dt;         // ����
    /*״̬����*/
    float pre_Err;     // ��һ��PID���
    float pre_I_Value; // ��һ�λ���ֵ
    float pre_Output;  // ��һ�����ֵ
    /*���ֿ�����*/
    float Weak_I_Rate; // ���ֱ���ʱͬ��˥��ϵ��
    /*�����������*/
    float I_Dead_Max;       // ������������
    float I_Dead_Min;       // ������������
    float I_Dead_DecayRate; // ��������˥��ϵ��
    /*����޷����*/
    float OUT_MAX; // λ��ʽ�������
    float OUT_MIN; // λ��ʽ�������
    /*΢���˲����*/
    float D_Filter_Rate; // ΢���˲�ϵ��
    float pre_D_Value;   // �ϴ�΢��ֵ
    /*΢������*/
    float pre_FB; // �ϴη���ֵ
} PID_ParamsTypedef;

/*λ��*/
typedef struct
{
    uint8_t I_Dead_Enable_Bits : 1;   // ������������ʹ��
    uint8_t I_Dead_Act_Bits : 2;      // ������������Ϊ
    uint8_t D_Filter_Enable_Bits : 1; // ΢���˲�����ʹ��
    uint8_t D_MODE_SELECT_Bits : 1;   // ΢�ֹ���ģʽѡ��
    uint8_t Reserved_Bits : 3;        // ����λ
} PID_Config_BitField;

/*PID��Ϊ*/
typedef union {
    uint8_t All_Bits;
    PID_Config_BitField bit;
} PID_FeaturesTypedef, PID_Feature_Union;

/*---------------------------- P I D �� �� �� �� --------------------------*/
/*������*/
void PID_Inval_Err_Proc(void);
/*���ú���*/
void PID_ParamsCfg(PID_ParamsTypedef *PID_struct, PID_Params_t PID_PARA_X, float Value); // ����PID����
void PID_FeatureCfg(PID_FeaturesTypedef *PID_Union, PID_Feature_t Value);                // ����PID����ѡ��
/*��ʼ������*/
void PID_Init(PID_ParamsTypedef *PID_struct, float Kp, float Ki, float Kd, float Sp,
              float dt, float OUT_MAX, float OUT_MIN); // PID������ʼ��
/*PID����*/
float PID_Compute(PID_ParamsTypedef *PID_struct, PID_FeaturesTypedef *PID_Union, float Fb); // ����һ��PID����

#endif
