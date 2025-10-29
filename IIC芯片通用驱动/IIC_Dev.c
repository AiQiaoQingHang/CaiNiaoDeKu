#include <stdint.h>
#include "IIC_Dev.h"

/**
 * @brief дIIC_Dev�Ĵ���
 *
 * @param Driver �����ṹ��
 * @param reg_adress �Ĵ�����ַ
 * @param Data Ҫд�������
 * @return true �ɹ�
 * @return false ʧ��
 */
bool IIC_Dev_W_Reg(IIC_Dev_Driver_t *Driver, uint8_t reg_adress, uint8_t Data)
{
    /*������ʼλ*/
    Err_Check(Driver->IIC_Generate_Start(), Generate_Start_Err);
    /*������λ��ַ+д*/
    Err_Check((Driver->IIC_Send_Dev_Addr((uint8_t)(Driver->Dev_Addr << 1), true)), Send_Dev_Addr_Err); // �Զ�����
    // ���ͼĴ�����ַ
    Err_Check(Driver->IIC_Send_Data(reg_adress), Send_Reg_Addr_Err);
    /*����Ҫд�������*/
    Err_Check(Driver->IIC_Send_Data(Data), Send_Data_Err);
    /*����ֹͣλ*/
    Err_Check(Driver->IIC_Generate_Stop(), Generate_Stop_Err);
    return true;
}

/**
 * @brief ��IIC_Dev�Ĵ���
 *
 * @param Driver �����ṹ��
 * @param reg_adress �Ĵ�����ַ
 * @param Rece ���ջ�����
 * @param size ��ȡ�ֽ���
 * @return true �ɹ�
 * @return false ����
 */
bool IIC_Dev_R_Regs(IIC_Dev_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size)
{
    /*������ʼλ*/
    Err_Check(Driver->IIC_Generate_Start(), Generate_Start_Err);
    /*������λ��ַ+д*/
    Err_Check((Driver->IIC_Send_Dev_Addr((uint8_t)(Driver->Dev_Addr << 1), true)), Send_Dev_Addr_Err); // �Զ�����
    /*���ͼĴ�����ַ*/
    Err_Check(Driver->IIC_Send_Data(reg_adress), Send_Reg_Addr_Err);
    /*�ظ�������ʼλ*/
    Err_Check(Driver->IIC_Generate_Start(), RE_Generate_Start_Err);
    /*������λ��ַ+��*/
    Err_Check(Driver->IIC_Send_Dev_Addr((uint8_t)((Driver->Dev_Addr << 1) | 0x01), false), RE_Send_Dev_Addr_Err);
    /*ѭ����������*/
    for (uint8_t i = 0; i < size - 1; i++) // �ճ����һ�ֽ�
    {
        Err_Check(Driver->IIC_Rece_Data(&Rece[i], false), Rece_Data_Err);
    }
    Err_Check(Driver->IIC_Rece_Data(&Rece[size - 1], true), Rece_Data_Err); // ���һ�ֽڵ�������
    /*����ֹͣλ*/
    Err_Check(Driver->IIC_Generate_Stop(), Generate_Stop_Err);
    return true;
}
