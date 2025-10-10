#ifndef NRF24L01_H
#define NRF24L01_H
#include <stdint.h>
#include "NRF24L01_define.h" //����NRF24L01�Ĵ���ӳ����ָ��궨��
#include "NRF24L01_Config.h"
/*--------------------------------- �� �� �� �� -----------------------------------*/
// ��һ����Ҫ�ǵ���ʱ���NRF24L01_Driver_t������ʹ��뺯��ʱ�Ƿ�Ϊ��ָ��

#if Debug_Enable
extern uint8_t DRIVER_VOID_ERR;    // NRF24L01_Driver_t��ָ���־λ
extern uint8_t PRINTF_VOID_ERR;    // ��ӡ������ָ���־λ
void NRF24L01_ASSERT_Failed(void); // ���������Ҫ�Լ��������
#define NRF24L01_ASSERT(drv)          \
    do {                              \
        if (!(drv)) {                 \
            DRIVER_VOID_ERR = 1;      \
            NRF24L01_ASSERT_Failed(); \
        }                             \
    } while (0)

// ���ڴ�ӡ������Ϣ����оƬ����ʱ������drv���printf�����м���һЩ��ʶ������鿴���ĸ�оƬ������
#define NRF24L01_ERR_PRINT(drv, str)   \
    do {                               \
        if (!(drv))                    \
            DRIVER_VOID_ERR = 1;       \
        else {                         \
            if (!((drv)->My_Printf))   \
                PRINTF_VOID_ERR = 1;   \
            else                       \
                (drv)->My_Printf(str); \
        }                              \
    } while (0)

/*���ô������*/
#define NRF24L01_Config_Err_Proc(drv, Err_Flag, str)                          \
    do {                                                                      \
        (drv)->Config_Err_FLAG = Err_Flag;                                    \
        if ((drv)->My_Printf != (void *)0) {                                  \
            NRF24L01_ERR_PRINT((drv), str);                                   \
            if ((drv)->Config_Err_CallBack != (void *)0) {                    \
                (drv)->Config_Err_CallBack(drv);                              \
            } else                                                            \
                NRF24L01_ERR_PRINT((drv), "Config_Err_CallBack is NULL\r\n"); \
        }                                                                     \
    } while (0)
#else
#define NRF24L01_ASSERT(drv)                         ((void)0)
#define NRF24L01_ERR_PRINT(drv, str)                 ((void)0)
#define NRF24L01_Config_Err_Proc(drv, Err_Flag, str) ((void)0)
#endif
/*---------------------------------- �� �� �� ͬ �� ---------------------------------*/
#if Status_SYNC
#define __NRF24L01_SYNC_Struct(drv, Reg_Addr, Reg_Value)        \
    do {                                                        \
        NRF24L01_SyncStruct_Single((drv), Reg_Addr, Reg_Value); \
        NRF24L01_SyncCheck((drv), Reg_Addr);                    \
    } while (0)
#else
#define __NRF24L01_SYNC_Struct(drv, Reg_Addr, Reg_Value) ((void)0)
#endif

/*---------------------------------- �� �� �� �� �� ---------------------------------*/
/*NRF���ƿ鶨��*/
/*NRF���ƿ鴴����*/
/*������궨�崴�����������ֶ�����״̬ͬ������*/
#define NRF24L01_Creat_CtrlBlock(Name, Delay_func, Spi_func, W_CE_func, W_CS_func) \
    NRF24L01_Driver_t Name = {                                                     \
        .Delay_us           = Delay_func,                                          \
        .SPI_SwapByte_MODE0 = Spi_func,                                            \
        .NRF24L01_W_CE      = W_CE_func,                                           \
        .NRF24L01_W_CS      = W_CS_func,                                           \
        .MAX_RT_CallBack    = ((void *)0),                                         \
        .TX_DS_CallBack     = ((void *)0),                                         \
        .RX_DR_CallBack     = ((void *)0),                                         \
    }

/*�������ṹ��*/
typedef struct {
    uint8_t Data[32]; // ����
    uint8_t pipe;     // �ܵ���
} NRF24L01_Pack_t;

#if Status_SYNC
/*оƬ״̬�ṹ��*/
typedef struct {
#if SYNC_INCLUDE_CONFIG_Reg
    uint8_t Cur_Mode;       // ��ǰģʽ
    uint8_t Cur_CRC_Length; // ��ǰCRC����
#endif
#if SYNC_INCLUDE_EN_AA_Reg
    uint8_t EN_Pipe_AA; // ͨ���Զ�Ӧ��ʹ��     ��һλΪ 1 ��������
#endif
#if SYNC_INCLUDE_EN_RXADDR_Reg
    uint8_t EN_Pipe_RX; // ����ͨ����ַ����     ��һλΪ 1 ��������
#endif
#if SYNC_INCLUDE_SETUP_AW_Reg
    uint8_t Cur_Addr_Length; // ��ǰ��ַ����
#endif
#if SYNC_INCLUDE_SETUP_RETR_Reg
    uint8_t Cur_ARC;  // ��ǰ�Զ��ش�����
    uint16_t Cur_ARD; // ��ǰ�Զ��ش���ʱ
#endif
#if SYNC_INCLUDE_RF_CH_Reg
    uint16_t Cur_Frequency; // ��ǰ2.4GƵ��
#endif
#if SYNC_INCLUDE_RF_SETUP_Reg
    uint8_t Cur_Air_Data_Rate; // ��ǰ���д�������
    uint8_t Cur_PA_Control;    // ��ǰ��Ƶ����
#endif
#if SYNC_INCLUDE_DYNPD_Reg
    uint8_t EN_Pipe_DPL; // ͨ����̬�غ�ʹ��     ��һλΪ 1 ��������
#endif
#if SYNC_INCLUDE_FEATURE_Reg
    uint8_t EN_ACK_PAY_FLAG; // ACK�����غ�ʹ��     ��0 ���á���1 ���á�
    uint8_t EN_DYN_ACK_FLAG; // ��ACK�غ�ʹ��       ��0 ���á���1 ���á�
    uint8_t EN_DPL_FLAG;     // ��̬�غ�ʹ��        ��0 ���á���1 ���á�
#endif
} NRF24L01_Status_t;
#endif

/*оƬ�����ṹ��*/
/*��������˶��оƬ��NRF24L01_Driver_t����ṹ��ʵ���Ͼ�����ʹ����һ��NRF24L01оƬ*/
typedef struct NRF24L01_Driver_t NRF24L01_Driver_t;
struct NRF24L01_Driver_t {
    // Ӳ�������
    void (*NRF24L01_W_CE)(GPIO_Status);     // дCE
    void (*NRF24L01_W_CS)(GPIO_Status);     // дCS
    uint8_t (*SPI_SwapByte_MODE0)(uint8_t); // SPI�������ֽڣ��˺��������账�� CS �ź�
    void (*Delay_us)(uint32_t);             // ΢���ӳ�
#if Status_SYNC
    // оƬ״̬���
    NRF24L01_Status_t NRF_Status; // оƬ״̬�ṹ��
#endif
    // �жϻص�����
    void (*MAX_RT_CallBack)(NRF24L01_Driver_t *); // ����ط��жϻص�����
    void (*TX_DS_CallBack)(NRF24L01_Driver_t *);  // �����жϻص�����
    void (*RX_DR_CallBack)(NRF24L01_Driver_t *);  // �����жϻص�����
#if Debug_Enable
    // ���ô������
    uint8_t Config_Err_FLAG;                          // ���ô����־λ
    void (*My_Printf)(const char *format, ...);       // ���ڴ�ӡ������Ϣ
    void (*Config_Err_CallBack)(NRF24L01_Driver_t *); // ���ô���ص���������ʵ�ִ˺���ʱע�������־λ
#endif
};

/*---------------------------------------N R F 2 4 L 0 1 �� �� ģ �� �� �� --------------------------------------*/
/*--------- �� �� �� �� �� ---------*/
#if Status_SYNC
void NRF24L01_SyncStruct_All(NRF24L01_Driver_t *drv);                                    // оƬ״̬�ṹ��ͬ����ȫ����
void NRF24L01_SyncStruct_Single(NRF24L01_Driver_t *drv, uint8_t Reg, uint8_t Reg_Value); // оƬ״̬�ṹ��ͬ����������
void NRF24L01_SyncCheck(NRF24L01_Driver_t *drv, uint8_t Reg_Addr);                       // �������ͬ��
#endif
/*------- �� �� �� �� �� �� --------*/
void NRF24L01_IRQHandler(NRF24L01_Driver_t *drv); // �жϷ����������ڲ����ýṹ���ڵĻص�����
/*--------- �� д �� �� �� ---------*/
uint8_t NRF24L01_R_Reg(NRF24L01_Driver_t *drv, uint8_t Reg_adress);                                // ���Ĵ���
uint8_t *NRF24L01_R_Regs(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t *Rece, uint8_t size); // ���Ĵ������ֽ�
void NRF24L01_W_Reg(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t Data);                     // д�Ĵ���
void NRF24L01_W_Regs(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t *Data, uint8_t size);     // д�Ĵ������ֽ�
/*---------- �� �� �� �� ----------*/
void NRF24L01_EnterPowerDown(NRF24L01_Driver_t *drv);                                                       // �������ģʽ
void NRF24L01_EnterTx(NRF24L01_Driver_t *drv);                                                              // ���뷢��ģʽ
void NRF24L01_EnterRx(NRF24L01_Driver_t *drv);                                                              // �������ģʽ
void NRF24L01_EnterStandby_1(NRF24L01_Driver_t *drv);                                                       // �������ģʽ1
void NRF24L01_ClearFIFOTx(NRF24L01_Driver_t *drv);                                                          // ���FIFOTx
void NRF24L01_ClearFIFORx(NRF24L01_Driver_t *drv);                                                          // ���FIFORx
void NRF24L01_R_RxFIFO(NRF24L01_Driver_t *drv, NRF24L01_Pack_t *Rece);                                      // ��Rx_FIFO����̬�غɡ�����̬�غɡ�
void NRF24L01_W_Tx(NRF24L01_Driver_t *drv, W_TX_Command W_TX, uint8_t *Send, uint8_t size, uint8_t Pipe_X); // дTx_FIFO
void NRF24L01_SendData(NRF24L01_Driver_t *drv);                                                             // ��������
NRF24L01_FLAG_Status NRF24L01_GetITFLAG(NRF24L01_Driver_t *drv, Reg_STATUS IT_FLAG);                        // ��ȡ�жϱ�־λ
void NRF24L01_ClearITFLAG(NRF24L01_Driver_t *drv, Reg_STATUS IT_FLAG);                                      // ����жϱ�־λ
uint8_t NRF24L01_R_RxNum(NRF24L01_Driver_t *drv, Reg_RX_PW RX_PW_Px);                                       // ��ȡ����ͨ�������ֽ�������̬�غɡ�
uint8_t NRF24L01_GetDataPipe(NRF24L01_Driver_t *drv);                                                       // ��ȡ���������ĸ��ܵ�
/*---------- �� �� �� �� ----------*/
void NRF24L01_EN_AutoACK(NRF24L01_Driver_t *drv, Reg_EN_AA EN_AA, NRF24L01_Status new_status);                // ʹ���Զ�Ӧ��
void NRF24L01_EN_RxAddr(NRF24L01_Driver_t *drv, Reg_EN_RXADDR ERX, NRF24L01_Status new_status);               // ʹ�ܽ���ͨ��
void NRF24L01_Set_Addr_Width(NRF24L01_Driver_t *drv, Reg_SETUP_AW Address_Width);                             // ���õ�ַ����
void NRF24L01_SET_RETR(NRF24L01_Driver_t *drv, Reg_SETUP_RETR_ARD ARD, Reg_SETUP_RETR_ARC ARC);               // �����Զ��ش���ʱ���Զ��ش�����
void NRF24L01_SetAir_Data_Rate(NRF24L01_Driver_t *drv, Reg_RF_SETUP RF_DR);                                   // ���ÿ�����������
void NRF24L01_PA_Control(NRF24L01_Driver_t *drv, Reg_RF_SETUP RF_PWR);                                        // ������Ƶ�������
void NRF24L01_SetFrequencyMHz(NRF24L01_Driver_t *drv, uint16_t freqMHz);                                      // ����2.4gͨ��Ƶ��
void NRF24L01_EN_DPL(NRF24L01_Driver_t *drv, Reg_DYNPD EN_DPL, NRF24L01_Status new_status);                   // ʹ��ͨ����̬�غ�
void NRF24L01_EN_FEATURE(NRF24L01_Driver_t *drv, Reg_FEATURE EN_FEATURE, NRF24L01_Status new_status);         // ʹ�����⹦��
void NRF24L01_Set_CRCByte(NRF24L01_Driver_t *drv, Reg_CONFIG CRC_x);                                          // ���ü��ֽ�CRCУ����
void NRF24L01_EN_Interrupt(NRF24L01_Driver_t *drv, Reg_CONFIG MASK_Int, NRF24L01_Status new_status);          // ʹ���ж�
void NRF24L01_Set_RxAddr(NRF24L01_Driver_t *drv, Reg_RX_ADDR RX_ADDR_Px, uint8_t *Addr, uint8_t Addr_length); // ���ý���ͨ����ַ
void NRF24L01_Set_TxAddr(NRF24L01_Driver_t *drv, uint8_t *Addr, uint8_t Addr_length);                         // ����Ŀ���ַ
void NRF24L01_Set_RxNum(NRF24L01_Driver_t *drv, Reg_RX_PW RX_PW_Px, uint8_t count);                           // ���ý���ͨ�������ֽ���

#endif
