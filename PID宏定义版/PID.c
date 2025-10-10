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
void PID_Init(PID_Typedef *PID_struct, float Kp, float Ki, float Kd, float Sp,
              float dt, float OUT_MAX, float OUT_MIN)
{
    if (PID_struct == (void *)0)
    {
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
    PID_struct->pre_Err = 0.0f;         // ��һ�� PID ���   0
    PID_struct->pre_I_Value = 0.0f;     // ��һ�λ��� I ֵ   0
    PID_struct->pre_Output = 0.0f;      // ��һ�����        0
    /*���ֿ�����*/
    PID_struct->Weak_I_Rate = 0.5f; // ���ֱ���ʱͬ��˥��ϵ����Ĭ��0.5
    /*�����������*/
#if I_Dead_Enable == PID_ENABLE
    PID_struct->I_Dead_Max = 0.0f; // ������������
    PID_struct->I_Dead_Min = 0.0f; // ������������
#if I_Dead_Act == 2
    PID_struct->I_Dead_DecayRate = 0.5f; // ��������˥��ϵ��
#endif
#endif
    /*����޷����*/
    PID_struct->OUT_MAX = OUT_MAX; // λ��ʽ�������
    PID_struct->OUT_MIN = OUT_MIN; // λ��ʽ�������
#if D_Filter_Enable == PID_ENABLE
    PID_struct->D_Filter_Rate = 0.5f; // ΢���˲�ϵ��,Ĭ��0.5
    PID_struct->pre_D_Value = 0.0f;   // �ϴ�΢��ֵ
#endif
#if D_MODE_SELECT == D_MODE_FB
    PID_struct->pre_FB = 0.0f; // �ϴη���ֵ
#endif
}

/********************************************************************
 *��    �飺�޸�PID�ṹ�����
 *��    ����PID�ṹ�壬Ҫ�޸ĵĲ������޸�ֵ
 *�� �� ֵ����
 *˵    ��������ʹ��PID_Set�����
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
        if (Value <= 0.0f) // ȷ���˲�ϵ����0-1֮��
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
 *��    �飺ִ��һ��PID����
 *��    ����PID�ṹ�壬����ֵFb
 *�� �� ֵ����
 *˵    ����������
 *********************************************************************/
float PID_Compute(PID_Typedef *PID_struct, float Fb)
{
    /*���㵱ǰ���*/
    float cur_Err = PID_struct->Sp - Fb; // �趨ֵ - ����ֵ

    /*----------------------------------------- �� �� �� �� �� I ----------------------------------------------*/
    float Integral_increments = 0; // �����������������
    float cur_integral = 0;        // ����ֵ
    /*-------------------------- �� �� �� �� �� �� ------------------------------*/
    if ((PID_struct->Ki != 0.0f) && (PID_struct->Ki > 1e-6f))
    {
        /*��ǰ������      = �ϴλ���ֵ +�����ϴ���� + ������*���ʱ�䣩/2
                         = �ϴλ���ֵ + ��������
                         = �ϴλ���ֵ + ����������
        */
        /*���ε�Ч����������������������� = �����ϵ� + �µף� * �ߣ�/2��*/
#if I_Dead_Enable == PID_DISABLE // �رջ����������ܣ���������
        Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt;

#else                        // ����������������
        /*�жϵ�ǰ�Ƿ���������*/
        if (cur_Err >= PID_struct->I_Dead_Min && cur_Err <= PID_struct->I_Dead_Max) // ��������
        {
#if I_Dead_Act == Clear_I    // �������������������
            PID_struct->pre_I_Value = 0;
            Integral_increments = 0;
#elif I_Dead_Act == Frozen_I // ���������ڶ���
            Integral_increments = 0; // ��������Ϊ 0��������һ�λ���ֵ
#elif I_Dead_Act == Decay_I  // ����������˥��
            Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt * PID_struct->I_Dead_DecayRate; // ���һ��˥��ϵ��
#else                        // I_Dead_Act�����������������
            Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt;
#endif
        }
        else // ��������,���������������
        {
            Integral_increments = ((PID_struct->pre_Err + cur_Err) / 2.0f) * PID_struct->dt;
        }
#endif
        cur_integral = PID_struct->pre_I_Value + Integral_increments; // ��ǰ������ = �ϴλ���ֵ + ����������
        // ����ʱ�жϡ�Integral_increments���Ƿ��򣬼����˳�����״̬
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
    }
    else // KiΪ0
    {
        cur_integral = 0.0f; // ����ϵ��Ϊ 0��������Ϊ 0
    }

    /*------------------------------------------ �� �� ΢ �� �� D --------------------------------------*/
    float cur_differ = 0;
    if ((PID_struct->Kd != 0.0f) && (PID_struct->Kd > 1e-6f))
    {
        if (PID_struct->First_Run_Flag == true) // ��һ������
        {
#if D_MODE_SELECT == D_MODE_ERR
            /*��ǰ΢���� = ����ǰ��� - �ϴ��� / ���ʱ��  ��d��ERR��/dt��*/
            /*tan������ֵ��б��*/
            cur_differ = 0.0f;
#elif D_MODE_SELECT == D_MODE_FB
            /*��ǰ΢���� = -����ǰ����ֵ - �ϴη���ֵ�� / ���ʱ��  ��-d��FB��/dt��*/
            cur_differ = 0.0f;
#endif
        }
        else // ��������
        {
#if D_MODE_SELECT == D_MODE_ERR
            cur_differ = (cur_Err - PID_struct->pre_Err) / PID_struct->dt;
#elif D_MODE_SELECT == D_MODE_FB
            cur_differ = -(Fb - PID_struct->pre_FB) / PID_struct->dt;
#endif
        }

#if D_Filter_Enable == PID_ENABLE
        // �����˲�
        if (PID_struct->First_Run_Flag == false) // �ܿ���һ��ִ��
            cur_differ = (cur_differ * PID_struct->D_Filter_Rate) + ((1 - PID_struct->D_Filter_Rate) * PID_struct->pre_D_Value);
#endif
    }
    else
    {
        cur_differ = 0.0f; // ΢��ϵ��Ϊ 0��΢����Ϊ 0
    }

    /*------------------------------------------ �� �� �� �� �� �� �� -----------------------------------*/
    float COp = (PID_struct->Kp * cur_Err);      // P���������
    float COi = (PID_struct->Ki * cur_integral); // I���������
    float COd = (PID_struct->Kd * cur_differ);   // D���������
    float CO = COp + COi + COd;                  // PID�����������

    /*--------------------------------------------- �� �� �� �� -----------------------------------------*/
    if (CO >= PID_struct->OUT_MAX) // �������ֵ
    {
        if (Integral_increments < 0.0f) // ��������֣���������С��0
        {
            PID_struct->pre_I_Value = cur_integral; // ���»���ֵ
        }
        else
        {
            PID_struct->pre_I_Value *= PID_struct->Weak_I_Rate; // ͬ����������
        }
        CO = PID_struct->OUT_MAX; // ����������
    }
    else if (CO <= PID_struct->OUT_MIN) // ������Сֵ
    {
        if (Integral_increments > 0.0f) // ��������֣�������������0
        {
            PID_struct->pre_I_Value = cur_integral; // ���»���ֵ
        }
        else
        {
            PID_struct->pre_I_Value *= PID_struct->Weak_I_Rate; // ͬ����������
        }
        CO = PID_struct->OUT_MIN; // ������С���
    }
    else // ������ֵ��
    {
        PID_struct->pre_I_Value = cur_integral; // �������»���ֵ
    }

    if (PID_struct->First_Run_Flag == true) // �����һ�����б�־λ
    {
        PID_struct->First_Run_Flag = false;
    }

    /*-------------------------------------- �� �� �� �� �� �� �� -----------------------------------------*/
    PID_struct->pre_Err = cur_Err; // ���汾�����
    PID_struct->pre_Output = CO; // ���汾�����ֵ
#if D_Filter_Enable == PID_ENABLE
    PID_struct->pre_D_Value = cur_differ; // ���汾��΢��ֵ
#endif
#if D_MODE_SELECT == D_MODE_FB
    PID_struct->pre_FB = Fb; // ���汾�η���ֵ
#endif
    return CO;
}
