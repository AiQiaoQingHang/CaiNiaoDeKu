#include "stm32f10x.h"                  // Device header
#include "NRF24L01.h"

uint8_t Txad[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};   //Ŀ���ַ
uint8_t Rxad[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};	 //������ַ

NRF24L01_Pack Rx_Buffer;

volatile int8_t SendFLAG=0;						//���ͱ�־λ		   	��0 ������	��1 ���ͳ�ʱ��
volatile int8_t Receive_DataFLAG=0;		//�������ݱ�־λ 	��0 �����ݡ���1 �����ݡ�

/*****************************************************************
 *��	�飺NRF24L01ģ���ʼ��
 *��	������
 *����ֵ����
 *˵	����ʹ��ʱ����
 *****************************************************************/
void NRF24L01_Init()
{
    /*��ʼ������*/
    NRF24L01_SPI_Init();
		/*��ʼ���ж�*/
		NRF24L01_EXTI_Init();
		/*����оƬ*/
    NRF24L01_Set_Addr_Width(Address_Width_5);				//����5�ֽڵ�ַ
	
		NRF24L01_Set_TxAddr(Txad,5);										//д��Ŀ���ַ
		
    NRF24L01_SetFrequencyMHz(2400);									//
    NRF24L01_SetAir_data_rate(RF_DR_2Mbps);					//
    NRF24L01_PA_Control(RF_PWR_0dBm);								//
	
    NRF24L01_SET_RETR(ARD_1000us,ARC_5);											//�Զ��ش�
		
		NRF24L01_EN_RxAddr(ERX_P0,NRF24L01_ENABLE);								//����ͨ��ʹ��
		NRF24L01_EN_AutoACK(EN_AA_P0,NRF24L01_ENABLE);						//�Զ�Ӧ��ʹ��
		NRF24L01_Set_RxNum(RX_PW_P0,32);
		NRF24L01_Set_RxAddr(RX_ADDR_P0,Rxad,5);										//д�����ͨ����ַ
    NRF24L01_Set_CRCByte(CRC_2_byte);													//16λCRC
		
    NRF24L01_ClearFIFORx();		//�������RXFIFO
    NRF24L01_ClearFIFOTx();		//�������TXFIFO
    NRF24L01_EnterRx();   		//�������ģʽ
}

/******************************************************************
 *��	�飺��ʼ���ⲿ�ж�
 *��	������
 *����ֵ����
 *˵	�����ϲ㺯������
 ******************************************************************/
void NRF24L01_EXTI_Init(void)
{  
	RCC_APB2PeriphClockCmd(NRF24L01_CLOCK_IRQ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	/*IRQ--PA11*/
	GPIO_InitTypeDef GPIO_Initstruct;
  GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IPU;						//�ж����ŵ͵�ƽ��Ч��Ĭ�ϸߵ�ƽ
  GPIO_Initstruct.GPIO_Pin = NRF24L01_IRQ_Pin;
  GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(NRF24L01_IRQ_Port,&GPIO_Initstruct);
	GPIO_EXTILineConfig(EXTI_Line_Port,EXTI_Line_Pin);		//ӳ��
	/*EXTI��ʼ��*/
	EXTI_InitTypeDef EXTI_Initstruct;
	EXTI_Initstruct.EXTI_Line = EXTI_Line_Source;
	EXTI_Initstruct.EXTI_LineCmd = ENABLE;
	EXTI_Initstruct.EXTI_Mode = EXTI_Mode_Interrupt;				//�ж�
	EXTI_Initstruct.EXTI_Trigger = EXTI_Trigger_Falling;			//�½��ش���
	EXTI_Init(&EXTI_Initstruct);
	/*NVIC����*/
	NVIC_InitTypeDef NVIC_Initstruct;
	NVIC_Initstruct.NVIC_IRQChannel = NVIC_IRQChannel_Source;
	NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Initstruct.NVIC_IRQChannelSubPriority = NVIC_SubPriority;
	NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = NVIC_PreemptionPriority;
	NVIC_Init(&NVIC_Initstruct);
}

/******************************************************************
 *��	�飺�ⲿ�жϺ���
 *��	������
 *����ֵ����
 *˵	�����ϲ㺯������
 ******************************************************************/
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line_Source) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line_Source);		//�����־λ
		NRF24L01_IRQHandler();											//�����жϺ���
	}
}

/********************************************************************
 *��	�飺NRF24L01�жϷ�����
 *��	������
 *����ֵ����
 *˵	�������������жϣ���������ⲿ�жϷ�������
 *********************************************************************/
void NRF24L01_IRQHandler(void)
{
	/*��ȡ�жϱ�־λ*/
	uint8_t ITFLAG =(NRF24L01_R_Reg(NRF24L01_STATUS) & (MAX_RT | TX_DS | RX_DR));		//һ����ȡ��ȫ���жϱ�־λ,��ȥ������λ
	/*�жϴ�����ʲô�ж�*/
	if(ITFLAG & MAX_RT)		//�ﵽ����ش�
	{
		NRF24L01_ClearITFLAG(MAX_RT);		//�����־λ
	}
	if(ITFLAG & TX_DS)		//���յ�ACK
	{
		NRF24L01_ClearITFLAG(TX_DS);
	}
	if(ITFLAG & RX_DR)		//�����ݵ���RXFIFO
	{
		NRF24L01_ClearITFLAG(RX_DR);
		Receive_DataFLAG = 1;		//�ñ�־λ�������ж��н��ж�ȡ����ֹ���ж��￨̫��
	}
}

/********************************************************************
 *��	�飺��ʼ��NRF24L01������
 *��	������
 *����ֵ����
 *˵	�����ϲ㺯������
 *********************************************************************/
void NRF24L01_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initstruct;
#if Sofa_SPI_ENABLE == 0		//���SPI
    /*SCK--PA5*/
    RCC_APB2PeriphClockCmd(NRF24L01_CLOCK_SCK,ENABLE);
    GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initstruct.GPIO_Pin = NRF24L01_SCK_Pin;
    GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NRF24L01_SCK_Port,&GPIO_Initstruct);
    /*MISO--PA6*/
    RCC_APB2PeriphClockCmd(NRF24L01_CLOCK_MISO,ENABLE);
    GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Initstruct.GPIO_Pin = NRF24L01_MISO_Pin;
    GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NRF24L01_MISO_Port,&GPIO_Initstruct);
    /*MOSI--PA7*/
    RCC_APB2PeriphClockCmd(NRF24L01_CLOCK_MOSI,ENABLE);
    GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initstruct.GPIO_Pin = NRF24L01_MOSI_Pin;
    GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NRF24L01_MOSI_Port,&GPIO_Initstruct);
#else		//Ӳ��SPI
    /*SCK--PA5��MOSI--PA7*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Initstruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_Initstruct);
    /*MISO--PA6*/
    GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Initstruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_Initstruct);
    /*Ӳ��SPI��ʼ��*/
    SPI_InitTypeDef SPI_Initstruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
    SPI_Initstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//9Mhz,NRF24L01+���֧��10Mhz
    SPI_Initstruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_Initstruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_Initstruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_Initstruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_Initstruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Initstruct.SPI_Mode = SPI_Mode_Master;
    SPI_Initstruct.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(My_SPI,&SPI_Initstruct);
    SPI_Cmd(My_SPI,ENABLE);
#endif
    /*CS--PA8*/
    RCC_APB2PeriphClockCmd(NRF24L01_CLOCK_CS,ENABLE);
    GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initstruct.GPIO_Pin = NRF24L01_CS_Pin;
    GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NRF24L01_CS_Port,&GPIO_Initstruct);
    /*CE--PA4*/	
    RCC_APB2PeriphClockCmd(NRF24L01_CLOCK_CE,ENABLE);
    GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initstruct.GPIO_Pin = NRF24L01_CE_Pin;
    GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NRF24L01_CE_Port,&GPIO_Initstruct);
    /*����Ĭ��״̬*/
    NRF24L01_W_CE(0);  	//CE�������źŷ�������
    NRF24L01_W_CS(1);	 	//�͵�ƽ��Ч��Ĭ�ϸߵ�ƽ
#if Sofa_SPI_ENABLE == 0		//Ӳ��SPI����Ҫ
    NRF24L01_W_MOSI(0);
    NRF24L01_W_SCK(0);  		//ģʽ0��ʱ��Ĭ�ϵ͵�ƽ
#endif
}

/******************************************************************
 *��	�飺дCE
 *��	����0�͵�ƽ��1�ߵ�ƽ
 *����ֵ����
 *˵	�����ϲ㺯������
 ******************************************************************/
void NRF24L01_W_CE(uint8_t status)
{
    GPIO_WriteBit(NRF24L01_CE_Port,NRF24L01_CE_Pin,(BitAction)status);
}

/******************************************************************
 *��	�飺дCS
 *��	����0�͵�ƽ��1�ߵ�ƽ
 *����ֵ����
 *˵	�����ϲ㺯�����ã��͵�ƽѡ��
 ******************************************************************/
void NRF24L01_W_CS(uint8_t status)
{
    GPIO_WriteBit(NRF24L01_CS_Port,NRF24L01_CS_Pin,(BitAction)status);
}

#if Sofa_SPI_ENABLE == 0		//����Ӳ��SPI
/*******************************************************************
 *��	�飺дMOSI
 *��	����0�͵�ƽ��1�ߵ�ƽ
 *����ֵ����
 *˵	�����ϲ㺯������
 *******************************************************************/
void NRF24L01_W_MOSI(uint8_t status)
{
    GPIO_WriteBit(NRF24L01_MOSI_Port,NRF24L01_MOSI_Pin,(BitAction)status);
}

/*******************************************************************
 *��	�飺дSCK
 *��	����0�͵�ƽ��1�ߵ�ƽ
 *����ֵ����
 *˵	�����ϲ㺯������
 *******************************************************************/
void NRF24L01_W_SCK(uint8_t status)
{
    GPIO_WriteBit(NRF24L01_SCK_Port,NRF24L01_SCK_Pin,(BitAction)status);
}

/********************************************************************
 *��	�飺��MISO
 *��	����0�͵�ƽ��1�ߵ�ƽ
 *����ֵ��MISO�ĵ�ƽ״��
 *˵	�����ϲ㺯������
 *********************************************************************/
uint8_t NRF24L01_R_MISO(void)
{
    return GPIO_ReadInputDataBit(NRF24L01_MISO_Port,NRF24L01_MISO_Pin);
}
#endif


/********************************************************************
 *��	�飺��SPI����һ�ֽ�
 *��	����Ҫ���͵��ֽ�
 *����ֵ��Ҫ���յ��ֽ�
 *˵	�����ϲ㺯������
 *********************************************************************/
uint8_t SPI_SwapByte(uint8_t txData)
{
#if Sofa_SPI_ENABLE == 0		//���SPI
    NRF24L01_W_SCK(0);  //ȷ��SCK�ǵ͵�ƽ
    for (uint8_t i = 0; i < 8; i ++)
    {
        /*SPIΪ��λ���У�����Ƴ���λ��MOSI����*/
        if (txData & 0x80)			//�ж�Byte�����λ
            NRF24L01_W_MOSI(1);		//���Ϊ1�����MOSI���1
        else
            NRF24L01_W_MOSI(0);		//���Ϊ0�����MOSI���0
        txData <<= 1;						//Byte����һλ�����λ�ճ������ڽ�������λ
        /*����SCK�Ͻ���*/
        NRF24L01_W_SCK(1);
        Delay_us(1);
        /*��ȡMISO����*/
        if (NRF24L01_R_MISO())
            txData |= 0x01;				//���Ϊ1�����Byte���λ��1������Ĭ��0

        /*����SCK�½���*/
        NRF24L01_W_SCK(0);
        Delay_us(1);
    }
    /*����Byte���ݣ���ʱ��ByteΪSPI�������յõ���һ���ֽ�����*/
    return txData;
#else
    uint16_t Timeout = 1000;
    while((SPI_I2S_GetFlagStatus(My_SPI,SPI_I2S_FLAG_TXE) == RESET) && Timeout !=0)
        Timeout--;
    SPI_I2S_SendData(My_SPI,txData);
    Timeout = 1000;
    while((SPI_I2S_GetFlagStatus(My_SPI,SPI_I2S_FLAG_RXNE) == RESET) && Timeout !=0)
        Timeout--;
    return SPI_I2S_ReceiveData(My_SPI);
#endif
}

/********************************************************************
 *��	�飺��NRF24L01�Ĵ���
 *��	�����Ĵ�����ַ
 *����ֵ���Ĵ���״̬
 *˵	�����ϲ㺯������
 *********************************************************************/
uint8_t NRF24L01_R_Reg(uint8_t Reg_adress)
{
    uint8_t status;
    /*����Ƭѡ����ʼͨ��*/
    NRF24L01_W_CS(0);
    /*����������ͼĴ�����ַ*/
    SPI_SwapByte(NRF24L01_R_REGISTER|Reg_adress);
    /*���ֽ�ά��ʱ�ӣ������Ĵ�����Ϣ*/
    status = SPI_SwapByte(dummy);
    /*����Ƭѡ������ͨ��*/
    NRF24L01_W_CS(1);
    return status;
}

/********************************************************************
 *��	�飺��NRF24L01�Ĵ������ֽ�
 *��	�����Ĵ�����ַ�����������������ֽ�
 *����ֵ���Ĵ���״̬
 *˵	�����˺�����Ҫ���ڲ�����Ҫ���ֽڶ���ļĴ���������յ�ַ�����͵�ַ
 *ע	�⣺�������ֽ�ӦС�ڻ�������С
 *********************************************************************/
uint8_t* NRF24L01_R_Regs(uint8_t Reg_adress,uint8_t* Rece,uint8_t size)
{
    /*����Ƭѡ����ʼͨ��*/
    NRF24L01_W_CS(0);
    /*����������ͼĴ�����ַ*/
    SPI_SwapByte(NRF24L01_R_REGISTER|Reg_adress);
    /*д��Ĵ�������*/
    for(uint8_t i=0; i<size; i++)
    {
        (*(Rece+i))=SPI_SwapByte(dummy);
    }
    /*����Ƭѡ������ͨ��*/
    NRF24L01_W_CS(1);
    return Rece;
}

/********************************************************************
 *��	�飺дNRF24L01�Ĵ���
 *��	�����Ĵ�����ַ��д�������
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_W_Reg(uint8_t Reg_adress,uint8_t Data)
{
    /*����Ƭѡ����ʼͨ��*/
    NRF24L01_W_CS(0);
    /*����������ͼĴ�����ַ*/
    SPI_SwapByte(NRF24L01_W_REGISTER|Reg_adress);
    /*д��Ĵ�������*/
    SPI_SwapByte(Data);
    /*����Ƭѡ������ͨ��*/
    NRF24L01_W_CS(1);
}

/********************************************************************
 *��	�飺дNRF24L01�Ĵ������ֽ�
 *��	�����Ĵ�����ַ��Ҫд������ݣ����ݸ���
 *����ֵ����
 *˵	�����˺�����Ҫ���ڲ�����Ҫ���ֽ�д��ļĴ���������յ�ַ�����͵�ַ
 *********************************************************************/
void NRF24L01_W_Regs(uint8_t Reg_adress,uint8_t* Data,uint8_t size)
{
    /*����Ƭѡ����ʼͨ��*/
    NRF24L01_W_CS(0);
    /*����������ͼĴ�����ַ*/
    SPI_SwapByte(NRF24L01_W_REGISTER|Reg_adress);
    /*д��Ĵ�������*/
    for(uint8_t i=0; i<size; i++)
    {
        SPI_SwapByte(*(Data+i));
    }
    /*����Ƭѡ������ͨ��*/
    NRF24L01_W_CS(1);
}

/********************************************************************
 *��	�飺�޸�2.4G�ź�Ƶ��
 *��	����2.4gƵ�Ρ�2400-2525��Mhz
 *����ֵ����
 *˵	�������㹫ʽF= 2400 + RF_CH [MHz]
 *********************************************************************/
void NRF24L01_SetFrequencyMHz(uint16_t freqMHz)
{
    // ���Ʒ�Χ 2400 MHz ~ 2525 MHz
    if (freqMHz < 2400) freqMHz = 2400;
    if (freqMHz > 2525) freqMHz = 2525;
    uint8_t channel = (uint8_t)(freqMHz - 2400);
    NRF24L01_W_Reg(NRF24L01_RF_CH, channel);
}

/********************************************************************
 *��	�飺�޸Ŀ�����������
 *��	�����鿴�궨���С�RF_SETUP��
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_SetAir_data_rate(Reg_RF_SETUP RF_DR)
{
    uint8_t RF_SETUP_Reg=0;
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    RF_SETUP_Reg = NRF24L01_R_Reg(NRF24L01_RF_SETUP);
    /*λ����*/
    RF_SETUP_Reg &= ~(RF_DR_Config_Bit);			//	(RF_SETUP_Reg & (1 1 0 1		 0 1 1 0)) = (x x 0 x 		0 x x 0)����λ3��5,���λ����Ϊ0
    /*д�������������*/
    RF_SETUP_Reg |= RF_DR;
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(NRF24L01_RF_SETUP,RF_SETUP_Reg);
}

/*********************************************************************
 *��	�飺�޸���Ƶ�������
 *��	�����鿴�궨���С�RF_SETUP��
 *����ֵ����
 *˵	������
 **********************************************************************/
void NRF24L01_PA_Control(Reg_RF_SETUP RF_PWR)
{
    uint8_t RF_SETUP_Reg=0;
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    RF_SETUP_Reg = NRF24L01_R_Reg(NRF24L01_RF_SETUP);
    /*λ����*/
    RF_SETUP_Reg &= ~(RF_PWR_Config_Bit);			//	(RF_SETUP_Reg & (1 1 1 1		1 0 0 0)) = (x x x x 		x 0 0 0)����λ1��2�����λ����Ϊ0
    /*д����Ƶ�������*/
    RF_SETUP_Reg |= RF_PWR;
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(NRF24L01_RF_SETUP,RF_SETUP_Reg);
}

/********************************************************************
 *��	�飺���FIFOTx
 *��	������
 *����ֵ����
 *˵	����FIFOTX��������ֻ��һ�����
 *********************************************************************/
void NRF24L01_ClearFIFOTx()
{
    /*����CS����ʼͨ��*/
    NRF24L01_W_CS(0);
    /*���͡����FIFOTx�������ֽ�*/
    SPI_SwapByte(NRF24L01_FLUSH_TX);
    /*����CS������ͨ��*/
    NRF24L01_W_CS(1);
    /*�ȴ������ɣ�����ʱ�����*/
    Delay_us(20);
}

/********************************************************************
 *��	�飺���FIFORx
 *��	������
 *����ֵ����
 *˵	����FIFORX��������ֻ��һ�����
 *********************************************************************/
void NRF24L01_ClearFIFORx()
{
    /*����CS����ʼͨ��*/
    NRF24L01_W_CS(0);
    /*���͡����FIFORx�������ֽ�*/
    SPI_SwapByte(NRF24L01_FLUSH_RX);
    /*����CS������ͨ��*/
    NRF24L01_W_CS(1);
    /*�ȴ������ɣ�����ʱ�����*/
    Delay_us(20);
}

/********************************************************************
 *��	�飺��RXFIFO
 *��	�����������ṹ��
 *����ֵ����
 *˵	�����˺������ڶ�ȡRX
 *ע	�⣺��������СҪ >= ����
 *********************************************************************/
void NRF24L01_R_RxFIFO(NRF24L01_Pack* Rece)
{
	uint8_t length = 0;		//���ݳ���
	/*��ȡ��һ�����������ĸ��ܵ�*/
	uint8_t Pipe = NRF24L01_GetDataPipe();
	if(Pipe > 5)		return;		//RxFIFO��������
	/*�жϴ˹ܵ��Ƕ�̬�غɻ��Ǿ�̬�غ�*/
	if((NRF24L01_R_Reg(NRF24L01_FEATURE) & EN_DPL_ENABLE) && (NRF24L01_R_Reg(NRF24L01_DYNPD) & (0x01<<Pipe)))		//������������λ��ifֻ�ж��ǲ���0
	//ͨ�����붯̬�غɵ�ʹ�ܸպ�һһ��Ӧ�����Ƹպõõ���Ӧͨ���Ķ�̬�غ�ʹ�������ֻ�ж�̬�غɵ��ܿ��غ�ͨ���Ķ�̬�غ�ͬʱʹ�ܲ���ʹ�ܶ�̬�غ�
	{
		/*��̬�غɣ��������ȡ����*/
		NRF24L01_W_CS(0);		//��ʼͨ��
		SPI_SwapByte(NRF24L01_R_RX_PL_WID);		//���������ֽ�
		length = SPI_SwapByte(dummy);		//��Ԫ�ֽ�ά��ʱ�ӣ��õ�����
		NRF24L01_W_CS(1);		//����ͨ��
	}
	else		//��̬�غɣ���ȡ��Ӧ�Ĵ���
	{
		length = NRF24L01_R_Reg(NRF24L01_RX_PW_P0 + Pipe);		//��ַ����������Pipeƫ�Ƽ���
	}
	/*�жϲ����Ϸ�*/
	if(length == 0 || length > 32)
		return;
	/*��������*/
	NRF24L01_W_CS(0);
	/*���������ֽ�*/
	SPI_SwapByte(NRF24L01_R_RX_PAYLOAD);
	/*��ȡ����*/
	for(uint8_t i = 0;i < length;i++)
	{
		Rece->Data[i] = SPI_SwapByte(dummy);		//��Ԫ�ֽ�ά��ʱ��
	}
	/*����ͨ��*/
	NRF24L01_W_CS(1);
	/*����ͨ����Ϣ*/
	Rece->pipe = Pipe;	
}

/********************************************************************
 *��	�飺дTX
 *��	����ָ�Ҫд�����ݣ����ݴ�С���ܵ���
 *����ֵ����
 *˵	����������дTXָ�����дACK�����غ���Ҫָ���ܵ��ţ�Pipe_X����ֻ�ڴ�ʱ����
 *********************************************************************/
void NRF24L01_W_Tx(W_TX_Command W_TX,uint8_t* Send,uint8_t size,uint8_t Pipe_X)
{
    /*ȷ������32�ֽ�*/
    if(size 	> 32)		size = 32;
    /*����CS����ʼͨ��*/
    NRF24L01_W_CS(0);
		/*����������ɲ�ͬ����*/
		if(W_TX == W_TX_ACK_PAYLOAD)
		{
			if(Pipe_X > 5	)	Pipe_X = 5;			//��ֹ����0-5����Ĳ���
			SPI_SwapByte(W_TX | Pipe_X);		//ACK������Ч�غ���ָ���ܵ���
		}
		else
		{
			SPI_SwapByte(W_TX);		//ʣ������ֱ��д�뼴��
		}
    /*SPI���ַ���*/
    for(uint8_t i=0; i<size; i++)
    {
        SPI_SwapByte(Send[i]);  //��TX��д����
    }
    /*����CS������ͨ��*/
    NRF24L01_W_CS(1);
}

/********************************************************************
 *��	�飺����TX
 *��	������
 *����ֵ����
 *˵	��������ʵ���ǽ��롾����ģʽ1��������������ҪCE����
 *********************************************************************/
void NRF24L01_EnterTx()
{
    uint8_t config_reg=0;
		/*����CE*/
		NRF24L01_W_CE(0);
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    config_reg = 	NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*�����0��1λ*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*��PWR_UP��1,PRIM_Rx��0*/
    config_reg |= PWR_UP;				// x x x x		x x 1 0��PRIM_RX�������Ѿ���0
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,config_reg);
}

/********************************************************************
 *��	�飺����RX
 *��	������
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_EnterRx()
{
    uint8_t config_reg=0;
	  /*����CE���޸ļĴ���*/
    NRF24L01_W_CE(0);
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    config_reg = 	NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*�����0��1λ*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*��PWR_UP��PRIM_Rx��1*/
    config_reg |= (PWR_UP | PRIM_RX);			// x x x x		x x 1 1
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,config_reg);
    /*����CE����������*/
    NRF24L01_W_CE(1);
}

/********************************************************************
 *��	�飺�������ģʽ1
 *��	������
 *����ֵ����
 *˵	���������޸ļĴ���ֵ
 *********************************************************************/
void NRF24L01_EnterStandby_1(void)
{
	  uint8_t config_reg=0;
		/*����CE*/
		NRF24L01_W_CE(0);
		/*����CE*/
		NRF24L01_W_CE(0);
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    config_reg = 	NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*�����0��1λ*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*��PWR_UP��1,PRIM_Rx��0*/
    config_reg |= PWR_UP;				// x x x x		x x 1 0��PRIM_RX�������Ѿ���0
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,config_reg);
}

/********************************************************************
 *��	�飺�������ģʽ
 *��	������
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_EnterPowerDown(void)
{
	   uint8_t config_reg=0;
		/*����CE*/
		NRF24L01_W_CE(0);
    /*�ȶ�ȡ����λ��������д�أ���Ӱ������λ*/
    /*��ȡ*/
    config_reg = 	NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*���PWR_UPλ*/
    config_reg &= ~(PWR_UP);						// x x x x		x x 0 x
    /*д�ؼĴ���*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,config_reg);
}

/********************************************************************
 *��	�飺��������
 *��	������
 *����ֵ����
 *˵	��������CE����
 *********************************************************************/
void NRF24L01_SendData(void)
{
    /*����TX����ʱCEû������*/
    NRF24L01_EnterTx();
    /*��������*/
    NRF24L01_W_CE(1);
    Delay_us(20);
    NRF24L01_W_CE(0);
		/*ʣ���������ж��д���*/
		/*���ж������½������ģʽ*/
}

/********************************************************************
 *��	�飺ʹ��ͨ���Զ�Ӧ��
 *��	����ͨ����ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	�����������ֻ�ܲ���һ��ͨ��
 *********************************************************************/
void NRF24L01_EN_AutoACK(Reg_EN_AA EN_AA,NRF24L01_Status new_status)
{
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_EN_AA);
    /*��0��Ӧλ��*/
    Reg &= ~EN_AA;		//����ͨ��������λ�պ�һһ��Ӧ
    /*�ж�ʹ�ܻ���ʧ��*/
    if(new_status)		//ʹ����λ��ʧ�ܲ���
        Reg |= EN_AA;
    /*����д������*/
    NRF24L01_W_Reg(NRF24L01_EN_AA,Reg);
}

/********************************************************************
 *��	�飺ʹ�ܽ���ͨ��
 *��	��������ͨ����ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	�����������ֻ�ܲ���һ��ͨ��
 *********************************************************************/
void NRF24L01_EN_RxAddr(Reg_EN_RXADDR EN_RXADDR,NRF24L01_Status new_status)
{
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_EN_RXADDR);
    /*��0��Ӧλ��*/
    Reg &= ~EN_RXADDR;		//����ͨ��������λ�պ�һһ��Ӧ
    /*�ж�ʹ�ܻ���ʧ��*/
    if(new_status)		//ʹ����λ��ʧ�ܲ���
        Reg |= EN_RXADDR;
    /*����д������*/
    NRF24L01_W_Reg(NRF24L01_EN_RXADDR,Reg);
}

/********************************************************************
 *��	�飺���õ�ַ���
 *��	������ַ���
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_Set_Addr_Width(Reg_SETUP_AW Address_Width)
{
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_SETUP_AW);
    /*��0λ���1λ��0*/
    Reg &= ~(Address_Width_Config_Bit);
    /*д�����*/
    Reg |= Address_Width;
    /*����д������*/
    NRF24L01_W_Reg(NRF24L01_SETUP_AW,Reg);
}

/********************************************************************
 *��	�飺�����Զ��ش�
 *��	�����ش��ӳ٣��ش�����
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_SET_RETR(Reg_SETUP_RETR_ARD ARD,Reg_SETUP_RETR_ARC ARC)
{
    /*ֱ��д�뼴��*/
    NRF24L01_W_Reg(NRF24L01_SETUP_RETR,(ARD | ARC));
}

/********************************************************************
 *��	�飺ʹ�ܶ�̬�غ�
 *��	����ͨ����ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_EN_DPL(Reg_DYNPD EN_DPL,NRF24L01_Status new_status)
{
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_DYNPD);
    /*��0��Ӧλ��*/
    Reg &= ~EN_DPL;		//����ͨ��������λ�պ�һһ��Ӧ
    /*�ж�ʹ�ܻ���ʧ��*/
    if(new_status)		//ʹ����λ��ʧ�ܲ���
    {
        Reg |= EN_DPL;
        NRF24L01_EN_AutoACK( (Reg_EN_AA)EN_DPL, NRF24L01_ENABLE );		//����DPLҲ����ͨ��һһ��Ӧ�ģ���Ҫȷ���������Զ�Ӧ��
        NRF24L01_EN_FEATURE(EN_DPL_ENABLE,NRF24L01_ENABLE);						//ȷ����̬�غ��ܿ��ؿ���
    }
    /*����д������*/
    NRF24L01_W_Reg(NRF24L01_DYNPD,Reg);
}

/********************************************************************
 *��	�飺ʹ�����⹦��
 *��	�������⹦�ܣ�ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	������̬�غ��ܿ��أ�������ACK�����ݰ�����ACK
 *********************************************************************/
void NRF24L01_EN_FEATURE(Reg_FEATURE EN_FEATURE,NRF24L01_Status new_status)
{
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_FEATURE);
    /*��0��Ӧλ��*/
    Reg &= ~EN_FEATURE;		//�������⹦��������λһһ��Ӧ
    /*�ж�ʹ�ܻ���ʧ��*/
    if(new_status)		//ʹ����λ��ʧ�ܲ���
        Reg |= EN_FEATURE;
    /*����д������*/
    NRF24L01_W_Reg(NRF24L01_FEATURE,Reg);
}

/********************************************************************
 *��	�飺����CRCУ�����ֽ���
 *��	����CRC�ֽ���
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_Set_CRCByte(Reg_CONFIG CRC_x)
{
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*��0��Ӧλ��*/
    Reg &= ~(1<<2);
    /*д�����*/
    Reg |= CRC_x;		//�����1�ֽڣ���2λ����0����������ֽڵ�2λ����1
    /*����д������*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,Reg);
}

/********************************************************************
 *��	�飺ʹ���ж�
 *��	�����ж�λ��ʹ�ܻ���ʧ��
 *����ֵ����
 *˵	��������ش��жϣ������жϣ������ж�
 *********************************************************************/
void NRF24L01_EN_Interrupt(Reg_CONFIG MASK_Int,NRF24L01_Status new_status)
{
    /*��ȡ�Ĵ���*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*��0��Ӧλ��*/
    Reg &= ~MASK_Int;				//д 0 ʹ���ж�
    /*�ж�ʹ�ܻ���ʧ��*/
    if(new_status == NRF24L01_DISABLE)		//ʹ��д0���ܣ�ʧ��д1��оƬ�涨 0 ʹ�ܣ�1ʧ��
        Reg |= MASK_Int;		//д 1 ʧ���ж�
    /*����д������*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,Reg);
}

/********************************************************************
 *��	�飺���ý���ͨ����ַ
 *��	����ͨ��X��Ҫ���õĵ�ַ���飬���鳤��
 *����ֵ����
 *˵	�������鳤��Ҫ����Ҫ��
 *********************************************************************/
void NRF24L01_Set_RxAddr(Reg_RX_ADDR RX_ADDR_Px,uint8_t* Addr,uint8_t Addr_length)
{
    /*��ָ����*/
    if(!Addr)
        return;
    /*ȷ��д��ͨ������������ַ�ֽ���*/
    if((RX_ADDR_Px == RX_ADDR_P0) || (RX_ADDR_Px == RX_ADDR_P1))		//ͨ�� 0 ��ͨ�� 1 ������д��5�ֽ�
    {
        /*��ȡоƬ���õĵ�ַ����*/
        uint8_t Reg_length = (NRF24L01_R_Reg(NRF24L01_SETUP_AW) & 0x03) + 2;		//��2����Ϊ�����3�ֽڵ�ַ���Ĵ�����ֵ��0x01�����ֽڵ�ַ����0x02����2�����Ǽ��ֽڵ�ַ,&0x03ȷ��ȡ��2λ
        /*ȷ��д�����鳤�ȣ���ֹ�Ƿ�����*/
        uint8_t count = (Addr_length >= Reg_length) ? Reg_length : Addr_length;
        NRF24L01_W_Regs(RX_ADDR_Px,Addr,count);
    }
    else		//ͨ�� 2 ��ͨ�� 5 ֻ��д��1�ֽڵ�ַ
    {
			if(Addr_length >= 1)		//ȷ��������һ������
        NRF24L01_W_Reg(RX_ADDR_Px,Addr[0]);
    }
}

/********************************************************************
 *��	�飺����Ŀ���ַ
 *��	����Ŀ���ַ���飬���鳤��
 *����ֵ����
 *˵	�������鳤��Ҫ����Ҫ��
 *********************************************************************/
void NRF24L01_Set_TxAddr(uint8_t* Addr,uint8_t Addr_length)
{
	/*�������Ϸ�*/
	if(Addr_length>= 5)
		Addr_length = 5;
	NRF24L01_W_Regs(NRF24L01_TX_ADDR,Addr,Addr_length);
}

/********************************************************************
 *��	�飺��ȡ�жϱ�־λ
 *��	�����ж�λ
 *����ֵ����
 *˵	������
 *********************************************************************/
NRF24L01_FLAG_Status NRF24L01_GetITFLAG(Reg_STSTUS IT_FLAG)
{
	/*��������Ч*/
	if((IT_FLAG == MAX_RT) || (IT_FLAG == TX_DS) || (IT_FLAG == RX_DR))
	{
		if((NRF24L01_R_Reg(NRF24L01_STATUS) & IT_FLAG))	//����־λ�������0��&��������0����λ��if������
			return NRF24L01_SET;
		else
			return NRF24L01_RESET;
	}
	else		//��������
		return NRF24L01_RESET;
}

/********************************************************************
 *��	�飺����жϱ�־λ
 *��	�����ж�λ
 *����ֵ����
 *˵	������
 *********************************************************************/
void NRF24L01_ClearITFLAG(Reg_STSTUS IT_FLAG)
{
	/*��������Ч*/
	if((IT_FLAG == MAX_RT) || (IT_FLAG == TX_DS) || (IT_FLAG == RX_DR))
	{
		/*��*/
		uint8_t status = NRF24L01_R_Reg(NRF24L01_STATUS);
		/*�����־λ*/
		status |= IT_FLAG;		//д 1 �����־λ
		/*д�ؼĴ���*/
		NRF24L01_W_Reg(NRF24L01_STATUS,status);
	}
}

/********************************************************************
 *��	�飺���ý���ͨ�������ֽ�������̬�غɡ�
 *��	����ͨ�����ֽ���
 *����ֵ����
 *˵	���������ͨ�����Ƕ�Ӧ�Ĵ����ĵ�ַ
 *********************************************************************/
void NRF24L01_Set_RxNum(Reg_RX_PW RX_PW_Px,uint8_t count)
{
	/*�������Ϸ���*/
	if(count > 32)
		count = 32;
	NRF24L01_W_Reg(RX_PW_Px,count);
}

/********************************************************************
 *��	�飺��ȡ����ͨ�������ֽ�������̬�غɡ�
 *��	����ͨ��
 *����ֵ����
 *˵	���������ͨ�����Ƕ�Ӧ�Ĵ����ĵ�ַ
 *********************************************************************/
uint8_t NRF24L01_R_RxNum(Reg_RX_PW RX_PW_Px)
{
	/*�������Ϸ���*/
	if((RX_PW_Px >= RX_PW_P0) && (RX_PW_Px <= RX_PW_P5))		//����Ĵ�����ַ�������ģ�ֻ��Ҫ�жϵ�ַ�Ĵ�С����
		return NRF24L01_R_Reg(RX_PW_Px);
	else
		return 0;
}

/********************************************************************
 *��	�飺��ȡ���������ĸ��ܵ�
 *��	������
 *����ֵ�����ݹܵ�
 *˵	������
 *********************************************************************/
uint8_t NRF24L01_GetDataPipe(void)
{
	return((NRF24L01_R_Reg(NRF24L01_STATUS) & RX_P_NO_Config_Bit) >> 1);		//����һλ������λ���뵽��0λ�����þ��Ƕ�Ӧͨ��,&����ȡ������λ
}




