#ifndef IIC_Dev_H
#define IIC_Dev_H

#include <stdint.h>
#include <stdbool.h>

#define Err_Check(Driver_Command, IIC_Err)    \
    do                                        \
    {                                         \
        if (!(Driver_Command))                \
        {                                     \
            Driver->Err_Flag = (IIC_Err);     \
            Driver->IIC_Generate_Stop();      \
            if (Driver->Err_CallBack)         \
            {                                 \
                Driver->Err_CallBack(Driver); \
            }                                 \
            return false;                     \
        }                                     \
    } while (0)

/* IIC����ö�� */
typedef enum
{
    None_Err,              // û�д���
    Generate_Start_Err,    // ��ʼλ��������
    RE_Generate_Start_Err, // �ظ�������ʼλ����
    Send_Dev_Addr_Err,     // �豸��ַ���ʹ���
    RE_Send_Dev_Addr_Err,  // �ڶ���Ѱַ����
    Send_Reg_Addr_Err,     // �Ĵ�����ַ���ʹ���
    Send_Data_Err,         // �������ݴ���
    Rece_Data_Err,         // �������ݴ���
    Generate_Stop_Err,     // ֹͣλ��������
} IIC_Err_Flag;

/* оƬ���ƽṹ�� */
typedef struct
{
    IIC_Err_Flag Err_Flag;                                  // �����־λ
    uint16_t Err_Count;                                     // �������
    uint8_t Dev_Addr;                                       // �豸��ַ,δ���� 1 λ
    bool (*IIC_Generate_Start)(void);                       // ������ʼ����
    bool (*IIC_Send_Dev_Addr)(uint8_t Addr, bool IS_Write); // �����豸��ַ
    bool (*IIC_Send_Data)(uint8_t Send_Data);               // ����һ�ֽ�����
    bool (*IIC_Rece_Data)(uint8_t *Rece_Buf, bool IS_NACK); // ����һ�ֽ�����
    bool (*IIC_Generate_Stop)(void);                        // ����ֹͣλ
    void (*Err_CallBack)(IIC_Err_Flag Err);                 // ����ص�����
} IIC_Dev_Driver_t;

bool IIC_Dev_W_Reg(IIC_Dev_Driver_t *Driver, uint8_t reg_adress, uint8_t Data);                 // д�Ĵ���
bool IIC_Dev_R_Regs(IIC_Dev_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size); // ���Ĵ���
#endif