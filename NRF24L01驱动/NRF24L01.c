#include <stdint.h>
#include "NRF24L01.h"
#include "NRF24L01_define.h"
#include <string.h>

#if Debug_Enable
uint8_t DRIVER_VOID_ERR = 0; // NRF24L01_Driver_t��ָ���־λ,�������ÿ�ָ��������
uint8_t PRINTF_VOID_ERR = 0; // ��ӡ������ָ���־λ

/********************************************************************
 *��	�飺����������ص�����
 *��	������
 *�� �� ֵ����
 *˵	�������NRF24L01_Driver_t�Ƿ�Ϊ��ָ�룬Ϊ��ָ�����ô˺���,�Լ���������ʵ��
 *********************************************************************/
__weak void NRF24L01_ASSERT_Failed(void)
{
    while(1)
    {

    };
}
#endif

/********************************************************************
 *��	�飺�жϷ�����
 *��	�����ײ������ṹ��
 *�� �� ֵ����
 *˵	�����жϷ�������������жϻص�����
 *********************************************************************/
void NRF24L01_IRQHandler(NRF24L01_Driver_t *drv)
{
    uint8_t status = NRF24L01_R_Reg(drv, NRF24L01_STATUS);
    if (((status & MAX_RT) != 0) && (drv->MAX_RT_CallBack)) {
        NRF24L01_ClearITFLAG(drv, MAX_RT);
        drv->MAX_RT_CallBack(drv);
    }
    if (((status & TX_DS) != 0) && (drv->TX_DS_CallBack)) {
        NRF24L01_ClearITFLAG(drv, TX_DS);
        drv->TX_DS_CallBack(drv);
    }
    if (((status & RX_DR) != 0) && (drv->RX_DR_CallBack)) {
        NRF24L01_ClearITFLAG(drv, RX_DR);
        drv->RX_DR_CallBack(drv);
    }
}

/********************************************************************
 *��	�飺��NRF24L01�Ĵ���
 *��	�����Ĵ�����ַ
 *����ֵ���Ĵ���״̬
 *˵	�����ϲ㺯������
 *********************************************************************/
uint8_t NRF24L01_R_Reg(NRF24L01_Driver_t *drv, uint8_t Reg_adress)
{
    NRF24L01_ASSERT(drv);
    uint8_t status;
    /*����Ƭѡ����ʼͨ��*/
    drv->NRF24L01_W_CS(LOW);
    /*����������ͼĴ�����ַ*/
    drv->SPI_SwapByte_MODE0(NRF24L01_R_REGISTER | Reg_adress);
    /*���ֽ�ά��ʱ�ӣ������Ĵ�����Ϣ*/
    status = drv->SPI_SwapByte_MODE0(dummy);
    /*����Ƭѡ������ͨ��*/
    drv->NRF24L01_W_CS(HIGH);
    return status;
}

/********************************************************************
 *��	�飺��NRF24L01�Ĵ������ֽ�
 *��	�����Ĵ�����ַ�����������������ֽ�
 *����ֵ���Ĵ���״̬
 *˵	�����˺�����Ҫ���ڲ�����Ҫ���ֽڶ���ļĴ���������յ�ַ�����͵�ַ
 *ע	�⣺�������ֽ�ӦС�ڻ�������С
 *********************************************************************/
uint8_t *NRF24L01_R_Regs(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t *Rece, uint8_t size)
{
    NRF24L01_ASSERT(drv);
    /*����Ƭѡ����ʼͨ��*/
    drv->NRF24L01_W_CS(LOW);
    /*����������ͼĴ�����ַ*/
    drv->SPI_SwapByte_MODE0(NRF24L01_R_REGISTER | Reg_adress);
    /*д��Ĵ�������*/
    for (uint8_t i = 0; i < size; i++) {
        (*(Rece + i)) = drv->SPI_SwapByte_MODE0(dummy);
    }
    /*����Ƭѡ������ͨ��*/
    drv->NRF24L01_W_CS(HIGH);
    return Rece;
}

/********************************************************************
 *��	�飺дNRF24L01�Ĵ���
 *��	�����Ĵ�����ַ��д�������
 *�� �� ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_W_Reg(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t Data)
{
    NRF24L01_ASSERT(drv);
    /*����Ƭѡ����ʼͨ��*/
    drv->NRF24L01_W_CS(LOW);
    /*����������ͼĴ�����ַ*/
    drv->SPI_SwapByte_MODE0(NRF24L01_W_REGISTER | Reg_adress);
    /*д��Ĵ�������*/
    drv->SPI_SwapByte_MODE0(Data);
    /*����Ƭѡ������ͨ��*/
    drv->NRF24L01_W_CS(HIGH);
}

/********************************************************************
 *��	�飺дNRF24L01�Ĵ������ֽ�
 *��	�����Ĵ�����ַ��Ҫд������ݣ����ݸ���
 *����ֵ����
 *˵	�����˺�����Ҫ���ڲ�����Ҫ���ֽ�д��ļĴ���������յ�ַ�����͵�ַ
 *********************************************************************/
void NRF24L01_W_Regs(NRF24L01_Driver_t *drv, uint8_t Reg_adress, uint8_t *Data, uint8_t size)
{
    NRF24L01_ASSERT(drv);
    /*����Ƭѡ����ʼͨ��*/
    drv->NRF24L01_W_CS(LOW);
    /*����������ͼĴ�����ַ*/
    drv->SPI_SwapByte_MODE0(NRF24L01_W_REGISTER | Reg_adress);
    /*д��Ĵ�������*/
    for (uint8_t i = 0; i < size; i++) {
        drv->SPI_SwapByte_MODE0(*(Data + i));
    }
    /*����Ƭѡ������ͨ��*/
    drv->NRF24L01_W_CS(HIGH);
}

/********************************************************************
 *��	�飺�޸�2.4G�ź�Ƶ��
 *��	����2.4gƵ�Ρ�2400-2525��Mhz
 *����ֵ����
 *˵	�������㹫ʽF= 2400 + RF_CH [MHz]
 *********************************************************************/
void NRF24L01_SetFrequencyMHz(NRF24L01_Driver_t *drv, uint16_t freqMHz)
{
    NRF24L01_ASSERT(drv);
    // ���Ʒ�Χ 2400 MHz ~ 2525 MHz
    if (freqMHz < 2400) freqMHz = 2400;
    if (freqMHz > 2525) freqMHz = 2525;
    uint8_t channel = (uint8_t)(freqMHz - 2400);
    NRF24L01_W_Reg(drv, NRF24L01_RF_CH, channel);
    /*���½ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_RF_CH, channel);
}

/********************************************************************
 *��	�飺�޸Ŀ�����������
 *��	�����鿴�궨���С�RF_SETUP��
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_SetAir_Data_Rate(NRF24L01_Driver_t *drv, Reg_RF_SETUP RF_DR)
{
    NRF24L01_ASSERT(drv);
    uint8_t RF_SETUP_Reg = 0;
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    RF_SETUP_Reg = NRF24L01_R_Reg(drv, NRF24L01_RF_SETUP);
    /*λ����*/
    RF_SETUP_Reg &= ~(RF_DR_Config_Bit); //	(RF_SETUP_Reg & (1 1 0 1		 0 1 1 0)) = (x x 0 x 		0 x x 0)����λ3��5,���λ����Ϊ0
    /*д�������������*/
    RF_SETUP_Reg |= RF_DR;
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(drv, NRF24L01_RF_SETUP, RF_SETUP_Reg);
    /*���½ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_RF_SETUP, RF_SETUP_Reg);
}

/*********************************************************************
 *��	�飺�޸���Ƶ�������
 *��	�����鿴�궨���С�RF_SETUP��
 *����ֵ����
 *˵	������
 **********************************************************************/
void NRF24L01_PA_Control(NRF24L01_Driver_t *drv, Reg_RF_SETUP RF_PWR)
{
    NRF24L01_ASSERT(drv);
    uint8_t RF_SETUP_Reg = 0;
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    RF_SETUP_Reg = NRF24L01_R_Reg(drv, NRF24L01_RF_SETUP);
    /*λ����*/
    RF_SETUP_Reg &= ~(RF_PWR_Config_Bit); //	(RF_SETUP_Reg & (1 1 1 1		1 0 0 0)) = (x x x x 		x 0 0 0)����λ1��2�����λ����Ϊ0
    /*д����Ƶ�������*/
    RF_SETUP_Reg |= RF_PWR;
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(drv, NRF24L01_RF_SETUP, RF_SETUP_Reg);
    /*���½ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_RF_SETUP, RF_SETUP_Reg);
}

/********************************************************************
 *��	�飺���FIFOTx
 *��	������
 *����ֵ����
 *˵	����FIFOTX��������ֻ��һ�����
 *********************************************************************/
void NRF24L01_ClearFIFOTx(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    /*����CS����ʼͨ��*/
    drv->NRF24L01_W_CS(LOW);
    /*���͡����FIFOTx�������ֽ�*/
    drv->SPI_SwapByte_MODE0(NRF24L01_FLUSH_TX);
    /*����CS������ͨ��*/
    drv->NRF24L01_W_CS(HIGH);
    /*�ȴ������ɣ�����ʱ�����*/
    drv->Delay_us(20);
}

/********************************************************************
 *��	�飺���FIFORx
 *��	������
 *����ֵ����
 *˵	����FIFORX��������ֻ��һ�����
 *********************************************************************/
void NRF24L01_ClearFIFORx(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    /*����CS����ʼͨ��*/
    drv->NRF24L01_W_CS(LOW);
    /*���͡����FIFORx�������ֽ�*/
    drv->SPI_SwapByte_MODE0(NRF24L01_FLUSH_RX);
    /*����CS������ͨ��*/
    drv->NRF24L01_W_CS(HIGH);
    /*�ȴ������ɣ�����ʱ�����*/
    drv->Delay_us(20);
}

/********************************************************************
 *��	�飺��RXFIFO
 *��	�����������ṹ��
 *����ֵ����
 *˵	�����˺������ڶ�ȡRX
 *ע	�⣺��������СҪ >= ����
 *********************************************************************/
void NRF24L01_R_RxFIFO(NRF24L01_Driver_t *drv, NRF24L01_Pack_t *Rece)
{
    NRF24L01_ASSERT(drv);
    uint8_t length = 0; // ���ݳ���
    /*��ȡ��һ�����������ĸ��ܵ�*/
    uint8_t Pipe = NRF24L01_GetDataPipe(drv);
    if (Pipe > 5) return; // RxFIFO��������
/*�жϴ˹ܵ��Ƕ�̬�غɻ��Ǿ�̬�غ�*/
#if (Status_SYNC && SYNC_INCLUDE_FEATURE_Reg && SYNC_INCLUDE_DYNPD_Reg)
    if ((drv->NRF_Status.EN_DPL_FLAG) && ((drv->NRF_Status.EN_Pipe_DPL) & (0x01 << Pipe)))
#else
    if ((NRF24L01_R_Reg(drv, NRF24L01_FEATURE) & EN_DPL_ENABLE) && (NRF24L01_R_Reg(drv, NRF24L01_DYNPD) & (0x01 << Pipe))) // ������������λ��ifֻ�ж��ǲ���0
#endif
    // ͨ�����붯̬�غɵ�ʹ�ܸպ�һһ��Ӧ�����Ƹպõõ���Ӧͨ���Ķ�̬�غ�ʹ�������ֻ�ж�̬�غɵ��ܿ��غ�ͨ���Ķ�̬�غ�ͬʱʹ�ܲ���ʹ�ܶ�̬�غ�
    {
        /*��̬�غɣ��������ȡ����*/
        drv->NRF24L01_W_CS(LOW);                       // ��ʼͨ��
        drv->SPI_SwapByte_MODE0(NRF24L01_R_RX_PL_WID); // ���������ֽ�
        length = drv->SPI_SwapByte_MODE0(dummy);       // ��Ԫ�ֽ�ά��ʱ�ӣ��õ�����
        drv->NRF24L01_W_CS(HIGH);                      // ����ͨ��
    } else                                             // ��̬�غɣ���ȡ��Ӧ�Ĵ���
    {
        length = NRF24L01_R_Reg(drv, NRF24L01_RX_PW_P0 + Pipe); // ��ַ����������Pipeƫ�Ƽ���
    }
    /*�жϲ����Ϸ�*/
    if (length > 0 && length < 32)
        return;
    /*��������*/
    drv->NRF24L01_W_CS(LOW);
    /*���������ֽ�*/
    drv->SPI_SwapByte_MODE0(NRF24L01_R_RX_PAYLOAD);
    /*��ȡ����*/
    for (uint8_t i = 0; i < length; i++) {
        Rece->Data[i] = drv->SPI_SwapByte_MODE0(dummy); // ��Ԫ�ֽ�ά��ʱ��
    }
    /*����ͨ��*/
    drv->NRF24L01_W_CS(HIGH);
    /*����ͨ����Ϣ*/
    Rece->pipe = Pipe;
}

/********************************************************************
 *��	�飺дTX
 *��	����ָ�Ҫд�����ݣ����ݴ�С���ܵ���
 *����ֵ����
 *˵	����������дTXָ�����дACK�����غ���Ҫָ���ܵ��ţ�Pipe_X����ֻ�ڴ�ʱ����
 *********************************************************************/
void NRF24L01_W_Tx(NRF24L01_Driver_t *drv, W_TX_Command W_TX, uint8_t *Send, uint8_t size, uint8_t Pipe_X)
{
    NRF24L01_ASSERT(drv);
    /*ȷ������32�ֽ�*/
    if (size > 32) {
        size = 32;
        NRF24L01_ERR_PRINT(drv, "NRF24L01_W_Tx Size Inval,use 32 Size");
    }
    /*����CS����ʼͨ��*/
    drv->NRF24L01_W_CS(LOW);
    /*����������ɲ�ͬ����*/
    if (W_TX == W_TX_ACK_PAYLOAD) {
        if (Pipe_X > 5) Pipe_X = 5;             // ��ֹ����0-5����Ĳ���
        drv->SPI_SwapByte_MODE0(W_TX | Pipe_X); // ACK������Ч�غ���ָ���ܵ���
    } else {
        drv->SPI_SwapByte_MODE0(W_TX); // ʣ������ֱ��д�뼴��
    }
    /*SPI���ַ���*/
    for (uint8_t i = 0; i < size; i++) {
        drv->SPI_SwapByte_MODE0(Send[i]); // ��TX��д����
    }
    /*����CS������ͨ��*/
    drv->NRF24L01_W_CS(HIGH);
}

/********************************************************************
 *��	�飺����TX
 *��	������
 *����ֵ����
 *˵	��������ʵ���ǽ��롾����ģʽ1��������������ҪCE����
 *********************************************************************/
void NRF24L01_EnterTx(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t config_reg = 0;
    /*����CE*/
    drv->NRF24L01_W_CE(LOW);
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    config_reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*�����0��1λ*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*��PWR_UP��1,PRIM_Rx��0*/
    config_reg |= PWR_UP; // x x x x		x x 1 0��PRIM_RX�������Ѿ���0
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, config_reg);
    /*���½ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, config_reg);
}

/********************************************************************
 *��	�飺����RX
 *��	������
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_EnterRx(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t config_reg = 0;
    /*����CE���޸ļĴ���*/
    drv->NRF24L01_W_CE(LOW);
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    config_reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*�����0��1λ*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*��PWR_UP��PRIM_Rx��1*/
    config_reg |= (PWR_UP | PRIM_RX); // x x x x		x x 1 1
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, config_reg);
    /*���½ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, config_reg);
    /*����CE����������*/
    drv->NRF24L01_W_CE(HIGH); // ���ͬ���мĴ�����д���ȼ��ͬ��
}

/********************************************************************
 *��	�飺�������ģʽ1
 *��	������
 *����ֵ����
 *˵	���������޸ļĴ���ֵ
 *********************************************************************/
void NRF24L01_EnterStandby_1(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t config_reg = 0;
    /*����CE*/
    drv->NRF24L01_W_CE(LOW);
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    config_reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*��PWR_UP��1*/
    config_reg |= PWR_UP;
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, config_reg);
    /*���½ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, config_reg);
}

/********************************************************************
 *��	�飺�������ģʽ
 *��	������
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_EnterPowerDown(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t config_reg = 0;
    /*����CE*/
    drv->NRF24L01_W_CE(LOW);
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    config_reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*���PWR_UPλ*/
    config_reg &= ~(PWR_UP); // x x x x		x x 0 x
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, config_reg);
    /*���½ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, config_reg);
}

/********************************************************************
 *��	�飺��������
 *��	������
 *����ֵ����
 *˵	��������CE����
 *********************************************************************/
void NRF24L01_SendData(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    /*����TX����ʱCEû������*/
    NRF24L01_EnterTx(drv);
    /*��������*/
    drv->NRF24L01_W_CE(HIGH);
    drv->Delay_us(20);
    drv->NRF24L01_W_CE(LOW);
    /*ʣ���������ж��д���*/
    /*���ж������½������ģʽ*/
}

/********************************************************************
 *��	�飺ʹ��ͨ���Զ�Ӧ��
 *��	����ͨ����ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	�����������ֻ�ܲ���һ��ͨ��
 *********************************************************************/
void NRF24L01_EN_AutoACK(NRF24L01_Driver_t *drv, Reg_EN_AA EN_AA, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_EN_AA);
    /*��0��Ӧλ��*/
    Reg &= ~EN_AA; // ����ͨ��������λ�պ�һһ��Ӧ
    /*�ж�ʹ�ܻ���ʧ��*/
    if (new_status) // ʹ����λ��ʧ�ܲ���
    {
        Reg |= EN_AA;
    }
    /*����д������*/
    NRF24L01_W_Reg(drv, NRF24L01_EN_AA, Reg);
    /*���½ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_EN_AA, Reg);
}

/********************************************************************
 *��	�飺ʹ�ܽ���ͨ��
 *��	��������ͨ����ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_EN_RxAddr(NRF24L01_Driver_t *drv, Reg_EN_RXADDR EN_RXADDR, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_EN_RXADDR);
    /*��0��Ӧλ��*/
    Reg &= ~EN_RXADDR; // ����ͨ��������λ�պ�һһ��Ӧ
    /*�ж�ʹ�ܻ���ʧ��*/
    if (new_status) // ʹ����λ��ʧ�ܲ���
    {
        Reg |= EN_RXADDR;
    }
    NRF24L01_W_Reg(drv, NRF24L01_EN_RXADDR, Reg);
    /*����״̬�ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_EN_RXADDR, Reg);
}

/********************************************************************
 *��	�飺���õ�ַ���
 *��	������ַ���
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_Set_Addr_Width(NRF24L01_Driver_t *drv, Reg_SETUP_AW Address_Width)
{
    NRF24L01_ASSERT(drv);
    /*�������Ϸ�*/
    if (Address_Width < Address_Width_3 || Address_Width > Address_Width_5) {
        Address_Width = Address_Width_5;
        NRF24L01_ERR_PRINT(drv, "NRF24L01_Set_Addr_Width INval,Use 5 AddrWith");
    }
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_SETUP_AW);
    /*��0λ���1λ��0*/
    Reg &= ~(Address_Width_Config_Bit);
    /*д�����*/
    Reg |= Address_Width;
    /*����д������*/
    NRF24L01_W_Reg(drv, NRF24L01_SETUP_AW, Reg);
    /*����״̬�ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_SETUP_AW, Reg);
}

/********************************************************************
 *��	�飺�����Զ��ش�
 *��	�����ش��ӳ٣��ش�����
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_SET_RETR(NRF24L01_Driver_t *drv, Reg_SETUP_RETR_ARD ARD, Reg_SETUP_RETR_ARC ARC)
{
    NRF24L01_ASSERT(drv);
    /*ֱ��д�뼴��*/
    NRF24L01_W_Reg(drv, NRF24L01_SETUP_RETR, (ARD | ARC));
    /*����״̬�ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_SETUP_RETR, (ARD | ARC));
}

/********************************************************************
 *��	�飺ʹ�ܶ�̬�غ�
 *��	����ͨ����ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_EN_DPL(NRF24L01_Driver_t *drv, Reg_DYNPD EN_DPL, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_DYNPD);
    /*��0��Ӧλ��*/
    Reg &= ~EN_DPL; // ����ͨ��������λ�պ�һһ��Ӧ
    /*�ж�ʹ�ܻ���ʧ��*/
    if (new_status) // ʹ����λ��ʧ�ܲ���
    {
        Reg |= EN_DPL;
    }
    /*����д������*/
    NRF24L01_W_Reg(drv, NRF24L01_DYNPD, Reg);
    /*����״̬�ṹ��,�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_DYNPD, Reg);
}

/********************************************************************
 *��	�飺ʹ�����⹦��
 *��	�������⹦�ܣ�ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	������̬�غ��ܿ��أ�������ACK�����ݰ�����ACK
 *********************************************************************/
void NRF24L01_EN_FEATURE(NRF24L01_Driver_t *drv, Reg_FEATURE EN_FEATURE, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_FEATURE);
    /*��0��Ӧλ��*/
    Reg &= ~EN_FEATURE; // �������⹦��������λһһ��Ӧ
    /*�ж�ʹ�ܻ���ʧ��*/
    if (new_status) // ʹ����λ��ʧ�ܲ���
    {
        Reg |= EN_FEATURE;
    }
    /*����д������*/
    NRF24L01_W_Reg(drv, NRF24L01_FEATURE, Reg);
    /*����״̬�ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_FEATURE, Reg);
}

/********************************************************************
 *��	�飺����CRCУ�����ֽ���
 *��	����CRC�ֽ���
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_Set_CRCByte(NRF24L01_Driver_t *drv, Reg_CONFIG CRC_x)
{
    NRF24L01_ASSERT(drv);
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*��0��Ӧλ��*/
    Reg &= ~(1 << 2);
    /*д�����*/
    Reg |= CRC_x; // �����1�ֽڣ���2λ����0����������ֽڵ�2λ����1
    /*����д������*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, Reg);
    /*����״̬�ṹ�壬�����ͬ��*/
    __NRF24L01_SYNC_Struct(drv, NRF24L01_CONFIG, Reg);
}

/********************************************************************
 *��	�飺ʹ���ж�
 *��	�����ж�λ��ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	��������ش��жϣ������жϣ������ж�
 *********************************************************************/
void NRF24L01_EN_Interrupt(NRF24L01_Driver_t *drv, Reg_CONFIG MASK_Int, NRF24L01_Status new_status)
{
    NRF24L01_ASSERT(drv);
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    /*��0��Ӧλ��*/
    Reg &= ~MASK_Int; // д 0 ʹ���ж�
    /*�ж�ʹ�ܻ���ʧ��*/
    if (new_status == NRF24L01_DISABLE) // ʹ��д0���ܣ�ʧ��д1��оƬ�涨 0 ʹ�ܣ�1ʧ��
        Reg |= MASK_Int;                // д 1 ʧ���ж�
    /*����д������*/
    NRF24L01_W_Reg(drv, NRF24L01_CONFIG, Reg);
}

/********************************************************************
 *��	�飺���ý���ͨ����ַ
 *��	����ͨ��X��Ҫ���õĵ�ַ���飬���鳤��
 *����ֵ����
 *˵	�������鳤��Ҫ����Ҫ��
 *********************************************************************/
void NRF24L01_Set_RxAddr(NRF24L01_Driver_t *drv, Reg_RX_ADDR RX_ADDR_Px, uint8_t *Addr, uint8_t Addr_length)
{
    NRF24L01_ASSERT(drv);
    /*��ָ����*/
    if (!Addr)
        return;
    /*ȷ��д��ͨ������������ַ�ֽ���*/
    if ((RX_ADDR_Px == RX_ADDR_P0) || (RX_ADDR_Px == RX_ADDR_P1)) // ͨ�� 0 ��ͨ�� 1 ������д��5�ֽ�
    {
        /*��ȡоƬ���õĵ�ַ����*/
        uint8_t Reg_length = (NRF24L01_R_Reg(drv, NRF24L01_SETUP_AW) & 0x03) + 2; // ��2����Ϊ�����3�ֽڵ�ַ���Ĵ�����ֵ��0x01�����ֽڵ�ַ����0x02����2�����Ǽ��ֽڵ�ַ,&0x03ȷ��ȡ��2λ
        /*ȷ��д�����鳤�ȣ���ֹ�Ƿ�����*/
        uint8_t count = (Addr_length >= Reg_length) ? Reg_length : Addr_length;
        NRF24L01_W_Regs(drv, RX_ADDR_Px, Addr, count);
    } else // ͨ�� 2 ��ͨ�� 5 ֻ��д��1�ֽڵ�ַ
    {
        if (Addr_length >= 1) // ȷ��������һ������
            NRF24L01_W_Reg(drv, RX_ADDR_Px, Addr[0]);
    }
}

/********************************************************************
 *��	�飺����Ŀ���ַ
 *��	����Ŀ���ַ���飬���鳤��
 *����ֵ����
 *˵	�������鳤��Ҫ����Ҫ��
 *********************************************************************/
void NRF24L01_Set_TxAddr(NRF24L01_Driver_t *drv, uint8_t *Addr, uint8_t Addr_length)
{
    NRF24L01_ASSERT(drv);
    /*�������Ϸ�*/
    if (Addr_length >= 5) {
        Addr_length = 5;
        NRF24L01_ERR_PRINT(drv, "NRF24L01_Set_TxAddr use 5 ByteLength");
    }
    NRF24L01_W_Regs(drv, NRF24L01_TX_ADDR, Addr, Addr_length);
}

/********************************************************************
 *��	�飺��ȡ�жϱ�־λ
 *��	�����ж�λ
 *����ֵ����
 *˵	������
 *********************************************************************/
NRF24L01_FLAG_Status NRF24L01_GetITFLAG(NRF24L01_Driver_t *drv, Reg_STATUS IT_FLAG)
{
    NRF24L01_ASSERT(drv);
    /*��������Ч*/
    if ((IT_FLAG == MAX_RT) || (IT_FLAG == TX_DS) || (IT_FLAG == RX_DR)) {
        if ((NRF24L01_R_Reg(drv, NRF24L01_STATUS) & IT_FLAG)) // ����־λ�������0��&��������0����λ��if������
            return NRF24L01_SET;
        else
            return NRF24L01_RESET;
    } else // ��������
        return NRF24L01_RESET;
}

/********************************************************************
 *��	�飺����жϱ�־λ
 *��	�����ж�λ
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_ClearITFLAG(NRF24L01_Driver_t *drv, Reg_STATUS IT_FLAG)
{
    NRF24L01_ASSERT(drv);
    /*��������Ч*/
    if ((IT_FLAG == MAX_RT) || (IT_FLAG == TX_DS) || (IT_FLAG == RX_DR)) {
        /*��*/
        uint8_t status = NRF24L01_R_Reg(drv, NRF24L01_STATUS);
        /*�����־λ*/
        status |= IT_FLAG; // д 1 �����־λ
        /*д�ؼĴ���*/
        NRF24L01_W_Reg(drv, NRF24L01_STATUS, status);
    }
}

/********************************************************************
 *��	�飺���ý���ͨ�������ֽ�������̬�غɡ�
 *��	����ͨ�����ֽ���
 *����ֵ����
 *˵	���������ͨ�����Ƕ�Ӧ�Ĵ����ĵ�ַ
 *********************************************************************/
void NRF24L01_Set_RxNum(NRF24L01_Driver_t *drv, Reg_RX_PW RX_PW_Px, uint8_t count)
{
    NRF24L01_ASSERT(drv);
    /*�������Ϸ���*/
    if (count > 32) {
        count = 32;
        NRF24L01_ERR_PRINT(drv, "NRF24L01_Set_RxNum use 32 count");
    }
    NRF24L01_W_Reg(drv, RX_PW_Px, count);
}

/********************************************************************
 *��	�飺��ȡ����ͨ�������ֽ�������̬�غɡ�
 *��	����ͨ��
 *����ֵ����
 *˵	���������ͨ�����Ƕ�Ӧ�Ĵ����ĵ�ַ
 *********************************************************************/
uint8_t NRF24L01_R_RxNum(NRF24L01_Driver_t *drv, Reg_RX_PW RX_PW_Px)
{
    NRF24L01_ASSERT(drv);
    /*�������Ϸ���*/
    if ((RX_PW_Px >= RX_PW_P0) && (RX_PW_Px <= RX_PW_P5)) // ����Ĵ�����ַ�������ģ�ֻ��Ҫ�жϵ�ַ�Ĵ�С����
        return NRF24L01_R_Reg(drv, RX_PW_Px);
    else
        NRF24L01_ERR_PRINT(drv, "NRF24L01_R_RxNum Inval");
    return 0;
}

/********************************************************************
 *��	�飺��ȡ���������ĸ��ܵ�
 *��	������
 *����ֵ�����ݹܵ�
 *˵	������
 *********************************************************************/
uint8_t NRF24L01_GetDataPipe(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
    uint8_t Reg = ((NRF24L01_R_Reg(drv, NRF24L01_STATUS) & RX_P_NO_Config_Bit) >> 1); // ����һλ������λ���뵽��0λ�����þ��Ƕ�Ӧͨ��,&����ȡ������λ
    if (Reg <= 5)
        return Reg;
    else
        NRF24L01_ERR_PRINT(drv, "NRF24L01_GetDataPipe Inval");
    return 7; // ����һ������ͨ��ֵ
}

#if Status_SYNC
/********************************************************************
 *��	�飺оƬ״̬�ṹ��ͬ����ȫ����
 *��	���������½ṹ��
 *�� �� ֵ����
 *˵	������Ҫ���ڽ�оƬ��״̬ʵʱͬ����������
 *********************************************************************/
void NRF24L01_SyncStruct_All(NRF24L01_Driver_t *drv)
{
    NRF24L01_ASSERT(drv);
#if SYNC_INCLUDE_CONFIG_Reg // ����ǰ����ģʽ����CRC���ȡ�
    uint8_t Config_temp = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
    // ����״̬
    if (!(Config_temp & 0x02))
        drv->NRF_Status.Cur_Mode = Power_Down; // ����ģʽ
    else if (Config_temp & (1 << 0))
        drv->NRF_Status.Cur_Mode = RX_Mode; // ����ģʽ
    else
        drv->NRF_Status.Cur_Mode = TX_Mode; // ����ģʽ
    // CRCУ�鳤��
    drv->NRF_Status.Cur_CRC_Length = (Config_temp & (1 << 2)) ? 2 : 1;
#endif

#if SYNC_INCLUDE_EN_AA_Reg // ��ͨ���Զ�Ӧ��ʹ�ܡ�
    uint8_t EN_AA_temp = NRF24L01_R_Reg(drv, NRF24L01_EN_AA);
    EN_AA_temp &= 0x3F; // ȡ��6λ
    drv->NRF_Status.EN_Pipe_AA = EN_AA_temp;
#endif

#if SYNC_INCLUDE_EN_RXADDR_Reg // ������ͨ��ʹ�ܡ�
    uint8_t EN_RX_ADDR_temp = NRF24L01_R_Reg(drv, NRF24L01_EN_RXADDR);
    EN_RX_ADDR_temp &= 0x3F; // ȡ��6λ
    drv->NRF_Status.EN_Pipe_RX = EN_RX_ADDR_temp;
#endif

#if SYNC_INCLUDE_SETUP_AW_Reg // �����յ�ַ���ȡ�
    uint8_t SETUP_AW_temp = NRF24L01_R_Reg(drv, NRF24L01_SETUP_AW);
    SETUP_AW_temp &= 0x03; // ȡ��2λ
    if (SETUP_AW_temp)
        drv->NRF_Status.Cur_Addr_Length = SETUP_AW_temp + 2; // ����ƫ��
    else
        drv->NRF_Status.Cur_Addr_Length = 0;
#endif

#if SYNC_INCLUDE_SETUP_RETR_Reg // ���Զ��ش����������Զ��ش��ӳ١�
    uint8_t SETUP_RETR_temp = NRF24L01_R_Reg(drv, NRF24L01_SETUP_RETR);
    drv->NRF_Status.Cur_ARC = SETUP_RETR_temp & 0x0F;                        // ȡ����λ
    drv->NRF_Status.Cur_ARD = (((SETUP_RETR_temp >> 4) & 0x0F) * 250 + 250); // ȡ����λ
#endif

#if SYNC_INCLUDE_RF_CH_Reg // ��2.4GƵ����
    uint8_t RF_CH_temp = NRF24L01_R_Reg(drv, NRF24L01_RF_CH);
    // 2.4GƵ��
    RF_CH_temp &= 0x7F; // ȡ��7λ
    drv->NRF_Status.Cur_Frequency = (uint16_t)(2400 + RF_CH_temp);
#endif

#if SYNC_INCLUDE_RF_SETUP_Reg // �����д������ʡ�����Ƶ���ʡ�
    uint8_t RF_SETUP_temp = NRF24L01_R_Reg(drv, NRF24L01_RF_SETUP);
    // ���д�������
    if (RF_SETUP_temp & RF_DR_1Mbps)
        drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_1Mbps; // ����ֱ��ʹ�üĴ�������λ
    else if (RF_SETUP_temp & RF_DR_2Mbps)
        drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_2Mbps;
    else if (RF_SETUP_temp & RF_DR_250Kbps)
        drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_250Kbps;
    // ��Ƶ����
    if (RF_SETUP_temp & RF_PWR_18dBm)
        drv->NRF_Status.Cur_PA_Control = RF_PWR_18dBm;
    else if (RF_SETUP_temp & RF_PWR_12dBm)
        drv->NRF_Status.Cur_PA_Control = RF_PWR_12dBm;
    else if (RF_SETUP_temp & RF_PWR_6dBm)
        drv->NRF_Status.Cur_PA_Control = RF_PWR_6dBm;
    else if (RF_SETUP_temp & RF_PWR_0dBm)
        drv->NRF_Status.Cur_PA_Control = RF_PWR_0dBm;
#endif

#if SYNC_INCLUDE_DYNPD_Reg // ��ͨ����̬�غ�ʹ�ܡ�
    uint8_t DYNPD_temp = NRF24L01_R_Reg(drv, NRF24L01_DYNPD);
    DYNPD_temp &= 0x3F; // ȡ��6λ
    drv->NRF_Status.EN_Pipe_DPL = DYNPD_temp;
#endif

#if SYNC_INCLUDE_FEATURE_Reg // �����⹦�ܿ��ء�
    uint8_t FEATURE_temp            = NRF24L01_R_Reg(drv, NRF24L01_FEATURE);
    drv->NRF_Status.EN_DYN_ACK_FLAG = (FEATURE_temp & (1 << 0)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
    drv->NRF_Status.EN_ACK_PAY_FLAG = (FEATURE_temp & (1 << 1)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
    drv->NRF_Status.EN_DPL_FLAG     = (FEATURE_temp & (1 << 2)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
#endif
}
#endif

#if Status_SYNC
/********************************************************************
 *��	�飺״̬�ṹ����
 *��	���������ṹ�壬оƬ�Ĵ�����ַ
 *�� �� ֵ����
 *˵	����ֱ�Ӵ���Ĵ�����ַ���������Զ����Ĵ�����ͬ��������
 *********************************************************************/
void NRF24L01_SyncCheck(NRF24L01_Driver_t *drv, uint8_t Reg_Addr)
{
    NRF24L01_ASSERT(drv);
    drv->Delay_us(20); // �ӳٸ�оƬʱ����λ
    uint8_t temp = 0;
    switch (Reg_Addr) {
#if SYNC_INCLUDE_CONFIG_Reg
        case NRF24L01_CONFIG: // ��鹤��ģʽ��CRC����
            temp = NRF24L01_R_Reg(drv, NRF24L01_CONFIG);
            // ���CRC����
            if ((((temp & (1 << 2)) >> 2) + 1) != drv->NRF_Status.Cur_CRC_Length) // ��ȡ���� 2 λ���ƶ����� 0 λ������ 1 ��ƫ�ƣ��պþ���CRC����
            {
                NRF24L01_Config_Err_Proc(drv, Cur_CRC_Length_Config_Err, "Err: Config CRC Length Failed\r\n");
            }
            // ��鵱ǰ����ģʽ
            temp &= 0x03;        // ȡ�� 0 ��1 λ
            if (temp & (1 << 1)) // оƬ�����ϵ�ģʽ
            {
                if (temp & (1 << 0)) // оƬ���ڽ���ģʽ
                {
                    if (drv->NRF_Status.Cur_Mode != RX_Mode) // �ṹ��״̬��оƬ״̬��һ��
                    {
                        NRF24L01_Config_Err_Proc(drv, Cur_Mode_Config_Err, "Err: Config Mode Config Failed\r\n");
                    }
                } else // оƬ���ڷ���ģʽ
                {
                    if (drv->NRF_Status.Cur_Mode != TX_Mode) // �ṹ��״̬��оƬ״̬��һ��
                    {
                        NRF24L01_Config_Err_Proc(drv, Cur_Mode_Config_Err, "Err: Config Mode Config Failed\r\n");
                    }
                }
            } else // оƬ���ڵ���ģʽ
            {
                if (drv->NRF_Status.Cur_Mode != Power_Down) // �ṹ��״̬��оƬ״̬��һ��
                {
                    NRF24L01_Config_Err_Proc(drv, Cur_Mode_Config_Err, "Err: Config Mode Config Failed\r\n");
                }
            }
            break;
#endif
#if SYNC_INCLUDE_EN_AA_Reg
        case NRF24L01_EN_AA: // ���ͨ���Զ�Ӧ��ʹ��
            temp = NRF24L01_R_Reg(drv, NRF24L01_EN_AA);
            if ((temp & 0x3F) != drv->NRF_Status.EN_Pipe_AA) {
                NRF24L01_Config_Err_Proc(drv, EN_Pipe_AA_Config_Err, "Err: EN Pipe_AA Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_EN_RXADDR_Reg
        case NRF24L01_EN_RXADDR: // ������ͨ��ʹ��
            temp = NRF24L01_R_Reg(drv, NRF24L01_EN_RXADDR);
            if ((temp & 0x3F) != drv->NRF_Status.EN_Pipe_RX) {
                NRF24L01_Config_Err_Proc(drv, EN_Pipe_RX_Config_Err, "Err: EN Pipe_RX Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_SETUP_AW_Reg
        case NRF24L01_SETUP_AW: // ������ͨ����ַ����
            temp = NRF24L01_R_Reg(drv, NRF24L01_SETUP_AW);
            if (((temp & 0x03) + 2) != drv->NRF_Status.Cur_Addr_Length) {
                NRF24L01_Config_Err_Proc(drv, Cur_Addr_Length_Config_Err, "Err: Cur_Addr_Length Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_SETUP_RETR_Reg
        case NRF24L01_SETUP_RETR: // ����Զ��ش��������Զ��ش���ʱ
            temp = NRF24L01_R_Reg(drv, NRF24L01_SETUP_RETR);
            // �Զ��ش�
            if ((temp & 0x0F) != drv->NRF_Status.Cur_ARC) {
                NRF24L01_Config_Err_Proc(drv, Cur_ARC_Config_Err, "Err: Cur ARC Config Failed\r\n");
            }
            // �Զ��ش���ʱ
            if (((temp >> 4) & 0x0F) != ((drv->NRF_Status.Cur_ARD - 250) / 4)) {
                NRF24L01_Config_Err_Proc(drv, Cur_ARD_Config_Err, "Err: Cur ARD Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_RF_CH_Reg
        case NRF24L01_RF_CH: // ���2.4GƵ��
            temp = NRF24L01_R_Reg(drv, NRF24L01_RF_CH);
            if ((temp & 0x7F) != ((uint8_t)(drv->NRF_Status.Cur_Frequency - 2400))) {
                NRF24L01_Config_Err_Proc(drv, Cur_Frequency_Config_Err, "Err: Cur Frequency Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_RF_SETUP_Reg
        case NRF24L01_RF_SETUP: // �����д������ʺ���Ƶ����,����ֱ��ʹ�üĴ�������λ
            temp = NRF24L01_R_Reg(drv, NRF24L01_RF_SETUP);
            // ���д�������
            if ((temp & RF_DR_Config_Bit) != drv->NRF_Status.Cur_Air_Data_Rate) // ȡ������λ����Ϊ���ṹ�帳ֵʱֱ���õĺͼĴ���һ��������λ������ֱ���ж��ǲ���һ������
            {
                NRF24L01_Config_Err_Proc(drv, Cur_Air_Data_Rate_Config_Err, "Err: Cur Air_Data_Rate Config Failed\r\n");
            }
            // ��Ƶ����
            if ((temp & RF_PWR_Config_Bit) != drv->NRF_Status.Cur_PA_Control) {
                NRF24L01_Config_Err_Proc(drv, Cur_PA_Control_Config_Err, "Err: Cur PA_Control Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_DYNPD_Reg
        case NRF24L01_DYNPD: // ͨ����̬����ʹ��
            temp = NRF24L01_R_Reg(drv, NRF24L01_DYNPD);
            if ((temp & 0x3F) != drv->NRF_Status.EN_Pipe_DPL) {
                NRF24L01_Config_Err_Proc(drv, EN_Pipe_DPL_Config_Err, "Err: EN Pipe_DPL Config Failed\r\n");
            }
            break;
#endif
#if SYNC_INCLUDE_FEATURE_Reg
        case NRF24L01_FEATURE: // ���⹦��ʹ��
            temp = NRF24L01_R_Reg(drv, NRF24L01_FEATURE);
            if ((temp & (1 << 0)) != drv->NRF_Status.EN_DYN_ACK_FLAG) { // ��ACK�غ�ʹ��
                NRF24L01_Config_Err_Proc(drv, EN_DYN_ACK_Config_Err, "Err: EN DYN_ACK FEATURE Config Failed\r\n");
            }
            if ((temp & (1 << 1)) != drv->NRF_Status.EN_ACK_PAY_FLAG) { // ACK�����غ�ʹ��
                NRF24L01_Config_Err_Proc(drv, EN_ACK_PAY_Config_Err, "Err: EN ACK_PAY FEATURE Config Failed\r\n");
            }
            if ((temp & (1 << 2)) != drv->NRF_Status.EN_DPL_FLAG) { // ��̬�غ�ʹ��
                NRF24L01_Config_Err_Proc(drv, EN_DPL_Config_Err, "Err: EN DPL Config FEATURE Failed\r\n");
            }
            break;
#endif
        default:
            break;
    }
}
#endif

/********************************************************************
 *��	�飺оƬ״̬�ṹ��ͬ����������
 *��	���������ṹ�壬оƬ�Ĵ�����ַ���Ĵ���ֵ
 *�� �� ֵ����
 *˵	����ֱ�Ӵ���Ĵ�����ַ���������Զ����Ĵ�����ͬ��������
 *********************************************************************/
void NRF24L01_SyncStruct_Single(NRF24L01_Driver_t *drv, uint8_t Reg, uint8_t Reg_Value)
{
    NRF24L01_ASSERT(drv);
    switch (Reg) {
#if SYNC_INCLUDE_CONFIG_Reg // ����ǰ����ģʽ����CRC���ȡ�
        case NRF24L01_CONFIG:
            // ����״̬
            if (!(Reg_Value & 0x02))
                drv->NRF_Status.Cur_Mode = Power_Down; // ����ģʽ
            else if (Reg_Value & (1 << 0))
                drv->NRF_Status.Cur_Mode = RX_Mode; // ����ģʽ
            else
                drv->NRF_Status.Cur_Mode = TX_Mode; // ����ģʽ
            // CRCУ�鳤��
            drv->NRF_Status.Cur_CRC_Length = (Reg_Value & (1 << 2)) ? 2 : 1;
            break;
#endif

#if SYNC_INCLUDE_EN_AA_Reg // ��ͨ���Զ�Ӧ��ʹ�ܡ�
        case NRF24L01_EN_AA:
            Reg_Value &= 0x3F; // ȡ��6λ
            drv->NRF_Status.EN_Pipe_AA = Reg_Value;
            break;
#endif

#if SYNC_INCLUDE_EN_RXADDR_Reg // ������ͨ��ʹ�ܡ�
        case NRF24L01_EN_RXADDR:
            Reg_Value &= 0x3F; // ȡ��6λ
            drv->NRF_Status.EN_Pipe_RX = Reg_Value;
            break;
#endif

#if SYNC_INCLUDE_SETUP_AW_Reg // �����յ�ַ���ȡ�
        case NRF24L01_SETUP_AW:
            Reg_Value &= 0x03; // ȡ��2λ
            if (Reg_Value)
                drv->NRF_Status.Cur_Addr_Length = Reg_Value + 2; // ����ƫ��
            else
                drv->NRF_Status.Cur_Addr_Length = 0;
            break;
#endif

#if SYNC_INCLUDE_SETUP_RETR_Reg // ���Զ��ش����������Զ��ش��ӳ١�
        case NRF24L01_SETUP_RETR:
            drv->NRF_Status.Cur_ARC = Reg_Value & 0x0F;                        // ȡ����λ
            drv->NRF_Status.Cur_ARD = (((Reg_Value >> 4) & 0x0F) * 250 + 250); // ȡ����λ
            break;
#endif

#if SYNC_INCLUDE_RF_CH_Reg // ��2.4GƵ����
        case NRF24L01_RF_CH:
            // 2.4GƵ��
            Reg_Value &= 0x7F; // ȡ��7λ
            drv->NRF_Status.Cur_Frequency = (uint16_t)(2400 + Reg_Value);
            break;
#endif

#if SYNC_INCLUDE_RF_SETUP_Reg // �����д������ʡ�����Ƶ���ʡ�
        case NRF24L01_RF_SETUP:
            // ���д�������
            if (Reg_Value & RF_DR_1Mbps)
                drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_1Mbps; // ����ֱ��ʹ�üĴ�������λ
            else if (Reg_Value & RF_DR_2Mbps)
                drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_2Mbps;
            else if (Reg_Value & RF_DR_250Kbps)
                drv->NRF_Status.Cur_Air_Data_Rate = RF_DR_250Kbps;
            // ��Ƶ����
            if (Reg_Value & RF_PWR_18dBm)
                drv->NRF_Status.Cur_PA_Control = RF_PWR_18dBm;
            else if (Reg_Value & RF_PWR_12dBm)
                drv->NRF_Status.Cur_PA_Control = RF_PWR_12dBm;
            else if (Reg_Value & RF_PWR_6dBm)
                drv->NRF_Status.Cur_PA_Control = RF_PWR_6dBm;
            else if (Reg_Value & RF_PWR_0dBm)
                drv->NRF_Status.Cur_PA_Control = RF_PWR_0dBm;
            break;
#endif

#if SYNC_INCLUDE_DYNPD_Reg // ��ͨ����̬�غ�ʹ�ܡ�
        case NRF24L01_DYNPD:
            Reg_Value &= 0x3F; // ȡ��6λ
            drv->NRF_Status.EN_Pipe_DPL = Reg_Value;
            break;
#endif

#if SYNC_INCLUDE_FEATURE_Reg // �����⹦�ܿ��ء�
        case NRF24L01_FEATURE:
            drv->NRF_Status.EN_DYN_ACK_FLAG = (Reg_Value & (1 << 0)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
            drv->NRF_Status.EN_ACK_PAY_FLAG = (Reg_Value & (1 << 1)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
            drv->NRF_Status.EN_DPL_FLAG     = (Reg_Value & (1 << 2)) ? NRF24L01_ENABLE : NRF24L01_DISABLE;
            break;
        default:
            break;
#endif
    }
}
