#ifndef icm42688_DRV_H
#define icm42688_DRV_H

#include <stdint.h>
#include <stdbool.h>
#include "icm42688_Drv_define.h" //���üĴ�����ַӳ��궨��

#define ICM_TEMP_DATA_SIZE_8BIT 2                                                                             // �¶ȼ����ݴ�С����λ���ֽڡ�
#define ICM_ACCEL_DATA_SIZE_8BIT 6                                                                            // ���ٶȼ����ݴ�С����λ���ֽڡ�
#define ICM_GYRO_DATA_SIZE_8BIT 6                                                                             // ���������ݴ�С����λ���ֽڡ�
#define ICM_ALL_DATA_SIZE_8BIT (ICM_TEMP_DATA_SIZE_8BIT + ICM_ACCEL_DATA_SIZE_8BIT + ICM_GYRO_DATA_SIZE_8BIT) // ȫ�����ݴ�С

#define ICM_TEMP_DATA_SIZE_16BIT (ICM_TEMP_DATA_SIZE_8BIT / 2)   // �¶ȼ����ݴ�С����λ�����֡�
#define ICM_ACCEL_DATA_SIZE_16BIT (ICM_ACCEL_DATA_SIZE_8BIT / 2) // ���ٶȼ����ݴ�С����λ�����֡�
#define ICM_GYRO_DATA_SIZE_16BIT (ICM_GYRO_DATA_SIZE_8BIT / 2)   // ���������ݴ�С����λ�����֡�
#define ICM_ALL_DATA_SIZE_16BIT (ICM_ALL_DATA_SIZE_8BIT / 2)     // ȫ�����ݴ�С����λ�����֡�

#define ICM_TEMP_DATA_OFFSET_8BIT 0  // �¶ȼ������������ṹ���ڵ���������ƫ�ơ���λ���ֽڡ�
#define ICM_ACCEL_DATA_OFFSET_8BIT 2 // ���ٶȼ������������ṹ���ڵ���������ƫ�ơ���λ���ֽڡ�
#define ICM_GYRO_DATA_OFFSET_8BIT 8  // �����������������ṹ���ڵ���������ƫ�ơ���λ���ֽڡ�

#define ICM_TEMP_DATA_OFFSET_16BIT 0                                 // �¶ȼ������������ṹ���ڵ���������ƫ�ơ���λ�����֡�
#define ICM_ACCEL_DATA_OFFSET_16BIT (ICM_ACCEL_DATA_OFFSET_8BIT / 2) // ���ٶȼ������������ṹ���ڵ���������ƫ�ơ���λ�����֡�
#define ICM_GYRO_DATA_OFFSET_16BIT (ICM_GYRO_DATA_OFFSET_8BIT / 2)   // �����������������ṹ���ڵ���������ƫ�ơ���λ�����֡�

#define RAD_TO_ANGLE 57.2958f   // ����ת�Ƕ�ϵ��

#define ERR_CHECK(Driver_Command, IIC_Err)    \
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

#define BYTESWAP_16BIT(data) ((int16_t)((((data) & 0x00FF) << 8) | (((data) & 0xFF00) >> 8)))
#define BYTESWAP_ARRAY_16BIT(Arr_Ptr_16Bit, Arr_Size_16Bit) \
    do                                                      \
    {                                                       \
        int16_t *Arr = (int16_t *)Arr_Ptr_16Bit;            \
        for (uint8_t i = 0; i < (Arr_Size_16Bit); i++)      \
        {                                                   \
            (Arr)[i] = BYTESWAP_16BIT((Arr)[i]);            \
        }                                                   \
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

/* оƬ���ݽṹ�� */
typedef struct
{
    int16_t Pre_GYRO[3];               // ��һ��������ԭʼ����
    int16_t Pre_ACCEL[3];              // ��һ�μ��ٶȼ�ԭʼ����
    int16_t GYRO_Bias[3];              // ��������ƫУ׼
    float Yaw, Pitch, Roll;            // ŷ����
    volatile uint8_t icm_Data_Buf[14]; // ���ݻ�����
} ICM42688_Value_t;

/* ���ݴ�������ṹ�� */
typedef struct
{
    float Ts;              // ��������
    float Fil_Power;       // �����˲�ϵ��
    float GYRO_Alone_Fil;  // �����ǵ����˲�ϵ��
    float ACCEL_Alone_Fil; // ���ٶȼƵ����˲�ϵ��
} ICM42688_Para_t;

struct ICM42688_Driver;
typedef struct ICM42688_Driver ICM42688_Driver_t;
/* оƬ���ƽṹ�� */
struct ICM42688_Driver
{
    ICM42688_Value_t ICM_Value;                             // ICM����
    ICM42688_Para_t ICM_Para;                               // ICM�˲�ϵ��
    IIC_Err_Flag Err_Flag;                                  // �����־λ
    uint16_t Err_Count;                                     // �������
    uint8_t Dev_Addr;                                       // �豸��ַ��ԭʼ��ַ��
    bool (*IIC_Generate_Start)(void);                       // ������ʼ����
    bool (*IIC_Send_Dev_Addr)(uint8_t Addr, bool IS_Write); // �����豸��ַ
    bool (*IIC_Send_Data)(uint8_t Send_Data);               // ����һ�ֽ�����
    bool (*IIC_Rece_Data)(uint8_t *Rece_Buf, bool IS_NACK); // ����һ�ֽ�����
    bool (*IIC_Generate_Stop)(void);                        // ����ֹͣλ
    void (*DMA_Start)(void);                                // DMA���俪ʼ
    void (*Delay_ms)(uint32_t ms);                          // �����ӳ�
    void (*Err_CallBack)(ICM42688_Driver_t *Drv);           // ����ص�����
};

/*----------------------------------------- I C M 4 2 6 8 8 --------------------------------------*/
void icm42688_Default_Init(ICM42688_Driver_t *Driver);                                            // Ĭ�ϳ�ʼ��
bool icm42688_W_Reg(ICM42688_Driver_t *Driver, uint8_t reg_adress, uint8_t Data);                 // д�Ĵ���
bool icm42688_R_Regs(ICM42688_Driver_t *Driver, uint8_t reg_adress, uint8_t *Rece, uint8_t size); // ���Ĵ���void icm42688_Cal(uint16_t num_samples);                                  // У׼
bool icm42688_GetID(ICM42688_Driver_t *Driver, uint8_t *ID_Buf);                                  // ��ȡоƬID
bool icm42688_UpTEMP(ICM42688_Driver_t *Driver);                                                  // �����¶ȼ�����
bool icm42688_UpACCEL(ICM42688_Driver_t *Driver, bool Fil_EN);                                    // ���¼��ٶȼ�����
bool icm42688_UpGYRO(ICM42688_Driver_t *Driver, bool Fil_EN);                                     // ��������������
bool icm42688_UpAllData(ICM42688_Driver_t *Driver, bool Fil_EN);                                  // ����ȫ������
void icm42688_ACCEL_GYRO_Fil(ICM42688_Driver_t *Driver);                                          // �����Ǻͼ��ٶ������˲�
bool icm42688_DMA_UPData(ICM42688_Driver_t *Driver);                                              // ʹ��DMA����ȫ������
uint16_t icm42688_Cal(ICM42688_Driver_t *Driver, uint16_t num_samples, uint16_t Delay_time_ms);   // ƽ��ֵУ׼
void icm42688_Slove_Eular(ICM42688_Driver_t *Driver);                                             // ����ŷ����
#endif
