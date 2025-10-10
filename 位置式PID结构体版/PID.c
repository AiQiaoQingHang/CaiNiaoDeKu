#include <stdint.h>
#include "PID.h"

/********************************************************************
 *��    �飺PID���ò�����Ч������
 *��    ������
 *�� �� ֵ����
 *˵    ����������PID������Чʱ�ᱻ���ã��û��Լ�����
 *********************************************************************/
__weak void PID_Inval_Err_Proc(void)
{
}

/********************************************************************
 *��    �飺PID������ʼ��
 *��    ����PID�ṹ�壬����ϵ��Kp������ϵ��Ki��΢��ϵ��Kd,�趨ֵor����ֵSp,
                       ִ������dt���������OUT_MAX���������OUT_MIN
 *�� �� ֵ����
 *˵    ������
 *********************************************************************/
void PID_Init(PID_ParamsTypedef *PID_struct, float Kp, float Ki, float Kd, float Sp,
              float dt, float OUT_MAX, float OUT_MIN)
{
    if (PID_struct == (void *)0) {
        PID_Inval_Err_Proc();
        return;
    }
    PID_struct->First_Run_Flag = true;
    /*��������*/
    PID_struct->Kp = Kp;
    PID_struct->Ki = Ki;
    PID_struct->Kd = Kd;
    PID_struct->Sp = Sp;
    PID_struct->dt = dt;
    /*״̬����*/
    PID_struct->pre_Err     = 0.0f; // ��һ�� PID ���   0
    PID_struct->pre_I_Value = 0.0f; // ��һ�λ��� I ֵ   0
    PID_struct->pre_Output  = 0.0f; // ��һ�����        0
    /*���ֿ�����*/
    PID_struct->Weak_I_Rate = 0.5f; // ���ֱ���ʱͬ��˥��ϵ����Ĭ��0.5
    /*�����������*/
    PID_struct->I_Dead_Max       = 0.0f; // ������������
    PID_struct->I_Dead_Min       = 0.0f; // ������������
    PID_struct->I_Dead_DecayRate = 0.5f; // ��������˥��ϵ��
    /*����޷����*/
    PID_struct->OUT_MAX = OUT_MAX; // λ��ʽ�������
    PID_struct->OUT_MIN = OUT_MIN; // λ��ʽ�������
    /*΢���˲����*/
    PID_struct->D_Filter_Rate = 0.5f; // ΢���˲�ϵ��,Ĭ��0.5
    PID_struct->pre_D_Value   = 0.0f; // �ϴ�΢��ֵ
    PID_struct->pre_FB        = 0.0f; // �ϴη���ֵ
}

/********************************************************************
 *��    �飺�޸�PID�ṹ�����
 *��    ����PID�ṹ�壬Ҫ�޸ĵĲ������޸�ֵ
 *�� �� ֵ����
 *˵    ��������ʹ��PID_Set�����
 *********************************************************************/
void PID_ParamsCfg(PID_ParamsTypedef *PID_struct, PID_Params_t PID_PARA_X, float Value)
{
    if (PID_struct == (void *)0) {
        PID_Inval_Err_Proc();
        return;
    }
    switch (PID_PARA_X) {
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
        case PID_PARA_I_Dead_Max:
            PID_struct->I_Dead_Max = Value;
            break;
        case PID_PARA_I_Dead_Min:
            PID_struct->I_Dead_Min = Value;
            break;
        case PID_PARA_I_Dead_DecayRate:
            PID_struct->I_Dead_DecayRate = Value;
            break;
        case PID_PARA_OUT_MAX:
            PID_struct->OUT_MAX = Value;
            break;
        case PID_PARA_OUT_MIN:
            PID_struct->OUT_MIN = Value;
            break;
        case PID_PARA_D_Filter_Rate:
            if (Value <= 0.0f) // ȷ���˲�ϵ����0-1֮��
                Value = 0.0f;
            else if (Value > 1.0f)
                Value = 1.0f;
            PID_struct->D_Filter_Rate = Value;
            break;
        default:
            break;
    }
}

/********************************************************************
 *��    �飺�޸�PID����ѡ��
 *��    ����PID���������壬Ҫ�޸ĵĹ���ѡ��
 *�� �� ֵ����
 *˵    ��������ʹ��PID_FeatureSet�����
 *********************************************************************/
void PID_FeatureCfg(PID_FeaturesTypedef *PID_Union, PID_Feature_t Value)
{
    if (PID_Union == (void *)0) {
        PID_Inval_Err_Proc();
        return;
    }
    switch (Value) {
        case PID_Feature_I_Dead_Enable: // ������������ʹ��
            PID_Union->All_Bits |= I_Dead_Enable_Config_BitField;
            break;
        case PID_Feature_I_Dead_Disable: // �����������ܽ���
            PID_Union->All_Bits &= ~I_Dead_Enable_Config_BitField;
            break;
        case PID_Feature_I_Dead_Act_Clear: // ������������Ϊ:�������
            PID_Union->All_Bits &= ~I_Dead_Act_Config_BitField;
            PID_Union->All_Bits |= I_Dead_Act_Clear_I;
            break;
        case PID_Feature_I_Dead_Act_Frozen: // ������������Ϊ:�������
            PID_Union->All_Bits &= ~I_Dead_Act_Config_BitField;
            PID_Union->All_Bits |= I_Dead_Act_Frozen_I;
            break;
        case PID_Feature_I_Dead_Act_Decay: // ������������Ϊ:˥������
            PID_Union->All_Bits &= ~I_Dead_Act_Config_BitField;
            PID_Union->All_Bits |= I_Dead_Act_Decay_I;
            break;
        case PID_Feature_D_Filter_Enable: // ΢���˲�����ʹ��
            PID_Union->All_Bits |= D_Filter_Enable_Config_BitField;
            break;
        case PID_Feature_D_Filter_Disable: // ΢���˲����ܽ���
            PID_Union->All_Bits &= ~D_Filter_Enable_Config_BitField;
            break;
        case PID_Feature_D_MODE_ERR: // ΢�ֹ���ģʽѡ��:���΢��
            PID_Union->All_Bits &= ~D_MODE_SELECT_Config_BitField;
            PID_Union->All_Bits |= D_MODE_Select_D_MODE_ERR;
            break;
        case PID_Feature_D_MODE_FB: // ΢�ֹ���ģʽѡ��:����΢��
            PID_Union->All_Bits &= ~D_MODE_SELECT_Config_BitField;
            PID_Union->All_Bits |= D_MODE_Select_D_MODE_FB;
            break;
        case PID_Feature_Reserved: // ����λ
            break;
        default:
            break;
    }
}

/********************************************************************
 *��    �飺����һ��PID����
 *��    ����PID�ṹ�壬PID��Ϊ�����壬����ֵFb
 *�� �� ֵ�����ر���PID������
 *˵    ������
 *********************************************************************/
float PID_Compute(PID_ParamsTypedef *PID_struct, PID_FeaturesTypedef *PID_Union, float Fb)
{
    /*���㵱ǰ���*/
    float cur_Err = PID_struct->Sp - Fb; // �趨ֵ - ����ֵ
    /*----------------------------------------- �� �� �� �� �� I ----------------------------------------------*/
    float Integral_increments = 0.0f; // �����������������
    float cur_integral        = 0.0f; // ����ֵ
    /*-------------------------- �� �� �� �� �� �� ------------------------------*/
    if ((PID_struct->Ki != 0.0f) && (PID_struct->Ki > 1e-6f)) {
        /*��ǰ������      = �ϴλ���ֵ +�����ϴ���� + ������*���ʱ�䣩/2
                         = �ϴλ���ֵ + ��������
                         = �ϴλ���ֵ + ����������
        */
        /*���ε�Ч����������������������� = �����ϵ� + �µף� * �ߣ�/2��*/
        if (PID_FeatureCheck(PID_Union, I_Dead_Enable)) // ������������ʹ��
        {
            /*�жϵ�ǰ�Ƿ���������*/
            if (cur_Err >= PID_struct->I_Dead_Min && cur_Err <= PID_struct->I_Dead_Max) // ��������
            {
                /*�ж�������Ϊ*/
                if (PID_FeatureCheck(PID_Union, I_Dead_Act_Clear_I)) // �������������������
                {
                    Integral_increments     = 0.0f;
                    PID_struct->pre_I_Value = 0.0f;                                                                                 // �����ʷ����ֵ
                } else if (PID_FeatureCheck(PID_Union, I_Dead_Act_Frozen_I))                                                        // ���������ڶ���
                    Integral_increments = 0.0f;                                                                                     // ��������Ϊ 0��������һ�λ���ֵ
                else if (PID_FeatureCheck(PID_Union, I_Dead_Act_Decay_I))                                                           // ����������˥��
                    Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt * PID_struct->I_Dead_DecayRate; // ���һ��˥��ϵ��
                else                                                                                                                // I_Dead_Act�����������������
                    Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt;
            } else // ��������,���������������
            {
                Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt;
            }
        } else // �����������ܹر�
        {
            Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt;
        }
        cur_integral = PID_struct->pre_I_Value + Integral_increments; // ��ǰ������ = �ϴλ���ֵ + ����������
        /*---------------------��---------------��-------------------��----------------*/

        /*----------��̬�����޷����˴�������Ϊ����ֵ���� COi / Ki ��---------------*/
        float I_MAX = PID_struct->OUT_MAX / PID_struct->Ki;
        float I_MIN = PID_struct->OUT_MIN / PID_struct->Ki;
        /*   ��   �����ڱ���״̬ʱ�����ڿ��ؿ����������ӽ�һ���̶�ֵ����ʱ�� P ���������͡� D ������������ʱ������ռ����Ի��ֻ�Խ��ԽС�������ۼ�����Խ��Խ��
            ��   ���յ��¡� I ������������������λ������˵��� ��PID�ܿ�������������� I �������������ռ�ܴ����
            ��   ��ʽ CO = Cp + Ci + Cd��  Cp �� Cd ���ؽ�С��Ĩȥ�õ� CO = Ci = ��Ki * I����Ϊ��ȷ���� I ����������������Ҳ����
            ʽ   ���� C0 ����ô Imax = ��COmax / Ki����Imin = ��COmin / Ki��
        */
        if (cur_integral > I_MAX) // ����
            cur_integral = I_MAX;
        else if (cur_integral < I_MIN) // С��
            cur_integral = I_MIN;
        /*---------------------��---------------��-------------------��----------------*/
    } else // KiΪ0
    {
        cur_integral = 0.0f; // ����ϵ��Ϊ 0��������Ϊ 0
    }

    /*------------------------------------------ �� �� ΢ �� �� D --------------------------------------*/
    float cur_differ = 0.0f;
    if ((PID_struct->Kd != 0.0f) && (PID_struct->Kd > 1e-6f)) {
        if (PID_struct->First_Run_Flag == true) // ��һ������
        {
            if (PID_FeatureCheck(PID_Union, D_MODE_Select_D_MODE_ERR)) // ���΢��
                cur_differ = 0.0f;                                     // ��ֹ��һ��ִ������
            else                                                       // ����΢��
                cur_differ = 0.0f;                                     // ��ֹ��һ��ִ������
        } else                                                         // ��������
        {
            if (PID_FeatureCheck(PID_Union, D_MODE_Select_D_MODE_ERR)) // ���΢��
                /*��ǰ΢���� = ����ǰ��� - �ϴ��� / ���ʱ��  ��d��ERR��/dt��*/
                /*tan������ֵ��б��*/
                cur_differ = (cur_Err - PID_struct->pre_Err) / PID_struct->dt;
            else // ����΢��
                /*��ǰ΢���� = -����ǰ����ֵ - �ϴη���ֵ�� / ���ʱ��  ��-d��FB��/dt��*/
                cur_differ = -(Fb - PID_struct->pre_FB) / PID_struct->dt;
        }
        /*΢���˲�*/
        if (PID_FeatureCheck(PID_Union, D_Filter_Enable)) {
            // �����˲�
            if (PID_struct->First_Run_Flag == false) // �ܿ���һ��ִ��
                cur_differ = (cur_differ * PID_struct->D_Filter_Rate) + ((1 - PID_struct->D_Filter_Rate) * PID_struct->pre_D_Value);
        }
    } else {               // KdΪ0
        cur_differ = 0.0f; // ΢��ϵ��Ϊ 0��΢����Ϊ 0
    }

    /*------------------------------------------ �� �� �� �� �� �� �� -----------------------------------*/
    float COp = (PID_struct->Kp * cur_Err);      // P���������
    float COi = (PID_struct->Ki * cur_integral); // I���������
    float COd = (PID_struct->Kd * cur_differ);   // D���������
    float CO  = COp + COi + COd;                 // PID�����������

    /*--------------------------------------------- �� �� �� �� -----------------------------------------*/
    if (CO >= PID_struct->OUT_MAX) // �������ֵ
    {
        if (cur_Err < 0.0f) // ��������֣����С��0
        {
            PID_struct->pre_I_Value = cur_integral; // ���»���ֵ
        } else {
            PID_struct->pre_I_Value *= PID_struct->Weak_I_Rate; // ͬ����������
        }
        CO = PID_struct->OUT_MAX;         // ����������
    } else if (CO <= PID_struct->OUT_MIN) // ������Сֵ
    {
        if (cur_Err > 0.0f) // ��������֣�������0
        {
            PID_struct->pre_I_Value = cur_integral; // ���»���ֵ
        } else {
            PID_struct->pre_I_Value *= PID_struct->Weak_I_Rate; // ͬ����������
        }
        CO = PID_struct->OUT_MIN; // ������С���
    } else                        // ������ֵ��
    {
        PID_struct->pre_I_Value = cur_integral; // �������»���ֵ
    }

    if (PID_struct->First_Run_Flag == true) // �����һ�����б�־λ
    {
        PID_struct->First_Run_Flag = false;
    }

    /*-------------------------------------- �� �� �� �� �� �� �� -----------------------------------------*/
    PID_struct->pre_Err     = cur_Err;    // ���汾�����
    PID_struct->pre_Output  = CO;         // ���汾�����ֵ
    PID_struct->pre_D_Value = cur_differ; // ���汾��΢��ֵ
    PID_struct->pre_FB      = Fb;         // ���汾�η���ֵ
    return CO;
}
