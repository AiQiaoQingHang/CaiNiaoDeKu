#include "stm32f10x.h"                  // Device header
#include "NRF24L01.h"

uint8_t Txad[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};   //目标地址
uint8_t Rxad[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};	 //本机地址

NRF24L01_Pack Rx_Buffer;

volatile int8_t SendFLAG=0;						//发送标志位		   	【0 正常】	【1 发送超时】
volatile int8_t Receive_DataFLAG=0;		//接收数据标志位 	【0 无数据】【1 有数据】

/*****************************************************************
 *简	介：NRF24L01模块初始化
 *参	数：无
 *返回值：无
 *说	明：使用时调用
 *****************************************************************/
void NRF24L01_Init()
{
    /*初始化引脚*/
    NRF24L01_SPI_Init();
		/*初始化中断*/
		NRF24L01_EXTI_Init();
		/*配置芯片*/
    NRF24L01_Set_Addr_Width(Address_Width_5);				//设置5字节地址
	
		NRF24L01_Set_TxAddr(Txad,5);										//写入目标地址
		
    NRF24L01_SetFrequencyMHz(2400);									//
    NRF24L01_SetAir_data_rate(RF_DR_2Mbps);					//
    NRF24L01_PA_Control(RF_PWR_0dBm);								//
	
    NRF24L01_SET_RETR(ARD_1000us,ARC_5);											//自动重传
		
		NRF24L01_EN_RxAddr(ERX_P0,NRF24L01_ENABLE);								//接收通道使能
		NRF24L01_EN_AutoACK(EN_AA_P0,NRF24L01_ENABLE);						//自动应答使能
		NRF24L01_Set_RxNum(RX_PW_P0,32);
		NRF24L01_Set_RxAddr(RX_ADDR_P0,Rxad,5);										//写入接收通道地址
    NRF24L01_Set_CRCByte(CRC_2_byte);													//16位CRC
		
    NRF24L01_ClearFIFORx();		//清空三级RXFIFO
    NRF24L01_ClearFIFOTx();		//清空三级TXFIFO
    NRF24L01_EnterRx();   		//进入接收模式
}

/******************************************************************
 *简	介：初始化外部中断
 *参	数：无
 *返回值：无
 *说	明：上层函数调用
 ******************************************************************/
void NRF24L01_EXTI_Init(void)
{  
	RCC_APB2PeriphClockCmd(NRF24L01_CLOCK_IRQ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	/*IRQ--PA11*/
	GPIO_InitTypeDef GPIO_Initstruct;
  GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IPU;						//中断引脚低电平有效，默认高电平
  GPIO_Initstruct.GPIO_Pin = NRF24L01_IRQ_Pin;
  GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(NRF24L01_IRQ_Port,&GPIO_Initstruct);
	GPIO_EXTILineConfig(EXTI_Line_Port,EXTI_Line_Pin);		//映射
	/*EXTI初始化*/
	EXTI_InitTypeDef EXTI_Initstruct;
	EXTI_Initstruct.EXTI_Line = EXTI_Line_Source;
	EXTI_Initstruct.EXTI_LineCmd = ENABLE;
	EXTI_Initstruct.EXTI_Mode = EXTI_Mode_Interrupt;				//中断
	EXTI_Initstruct.EXTI_Trigger = EXTI_Trigger_Falling;			//下降沿触发
	EXTI_Init(&EXTI_Initstruct);
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_Initstruct;
	NVIC_Initstruct.NVIC_IRQChannel = NVIC_IRQChannel_Source;
	NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Initstruct.NVIC_IRQChannelSubPriority = NVIC_SubPriority;
	NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = NVIC_PreemptionPriority;
	NVIC_Init(&NVIC_Initstruct);
}

/******************************************************************
 *简	介：外部中断函数
 *参	数：无
 *返回值：无
 *说	明：上层函数调用
 ******************************************************************/
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line_Source) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line_Source);		//清除标志位
		NRF24L01_IRQHandler();											//调用中断函数
	}
}

/********************************************************************
 *简	介：NRF24L01中断服务函数
 *参	数：无
 *返回值：无
 *说	明：处理三个中断，需放置在外部中断服务函数中
 *********************************************************************/
void NRF24L01_IRQHandler(void)
{
	/*获取中断标志位*/
	uint8_t ITFLAG =(NRF24L01_R_Reg(NRF24L01_STATUS) & (MAX_RT | TX_DS | RX_DR));		//一次性取出全部中断标志位,并去除其他位
	/*判断触发了什么中断*/
	if(ITFLAG & MAX_RT)		//达到最大重传
	{
		NRF24L01_ClearITFLAG(MAX_RT);		//清除标志位
	}
	if(ITFLAG & TX_DS)		//接收到ACK
	{
		NRF24L01_ClearITFLAG(TX_DS);
	}
	if(ITFLAG & RX_DR)		//有数据到达RXFIFO
	{
		NRF24L01_ClearITFLAG(RX_DR);
		Receive_DataFLAG = 1;		//置标志位，不在中断中进行读取，防止在中断里卡太久
	}
}

/********************************************************************
 *简	介：初始化NRF24L01的引脚
 *参	数：无
 *返回值：无
 *说	明：上层函数调用
 *********************************************************************/
void NRF24L01_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initstruct;
#if Sofa_SPI_ENABLE == 0		//软件SPI
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
#else		//硬件SPI
    /*SCK--PA5，MOSI--PA7*/
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
    /*硬件SPI初始化*/
    SPI_InitTypeDef SPI_Initstruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
    SPI_Initstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//9Mhz,NRF24L01+最大支持10Mhz
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
    /*引脚默认状态*/
    NRF24L01_W_CE(0);  	//CE给脉冲信号发送数据
    NRF24L01_W_CS(1);	 	//低电平有效，默认高电平
#if Sofa_SPI_ENABLE == 0		//硬件SPI不需要
    NRF24L01_W_MOSI(0);
    NRF24L01_W_SCK(0);  		//模式0，时钟默认低电平
#endif
}

/******************************************************************
 *简	介：写CE
 *参	数：0低电平，1高电平
 *返回值：无
 *说	明：上层函数调用
 ******************************************************************/
void NRF24L01_W_CE(uint8_t status)
{
    GPIO_WriteBit(NRF24L01_CE_Port,NRF24L01_CE_Pin,(BitAction)status);
}

/******************************************************************
 *简	介：写CS
 *参	数：0低电平，1高电平
 *返回值：无
 *说	明：上层函数调用，低电平选中
 ******************************************************************/
void NRF24L01_W_CS(uint8_t status)
{
    GPIO_WriteBit(NRF24L01_CS_Port,NRF24L01_CS_Pin,(BitAction)status);
}

#if Sofa_SPI_ENABLE == 0		//启用硬件SPI
/*******************************************************************
 *简	介：写MOSI
 *参	数：0低电平，1高电平
 *返回值：无
 *说	明：上层函数调用
 *******************************************************************/
void NRF24L01_W_MOSI(uint8_t status)
{
    GPIO_WriteBit(NRF24L01_MOSI_Port,NRF24L01_MOSI_Pin,(BitAction)status);
}

/*******************************************************************
 *简	介：写SCK
 *参	数：0低电平，1高电平
 *返回值：无
 *说	明：上层函数调用
 *******************************************************************/
void NRF24L01_W_SCK(uint8_t status)
{
    GPIO_WriteBit(NRF24L01_SCK_Port,NRF24L01_SCK_Pin,(BitAction)status);
}

/********************************************************************
 *简	介：读MISO
 *参	数：0低电平，1高电平
 *返回值：MISO的电平状况
 *说	明：上层函数调用
 *********************************************************************/
uint8_t NRF24L01_R_MISO(void)
{
    return GPIO_ReadInputDataBit(NRF24L01_MISO_Port,NRF24L01_MISO_Pin);
}
#endif


/********************************************************************
 *简	介：初SPI交换一字节
 *参	数：要发送的字节
 *返回值：要接收的字节
 *说	明：上层函数调用
 *********************************************************************/
uint8_t SPI_SwapByte(uint8_t txData)
{
#if Sofa_SPI_ENABLE == 0		//软件SPI
    NRF24L01_W_SCK(0);  //确保SCK是低电平
    for (uint8_t i = 0; i < 8; i ++)
    {
        /*SPI为高位先行，因此移出高位至MOSI引脚*/
        if (txData & 0x80)			//判断Byte的最高位
            NRF24L01_W_MOSI(1);		//如果为1，则给MOSI输出1
        else
            NRF24L01_W_MOSI(0);		//如果为0，则给MOSI输出0
        txData <<= 1;						//Byte左移一位，最低位空出来用于接收数据位
        /*产生SCK上降沿*/
        NRF24L01_W_SCK(1);
        Delay_us(1);
        /*读取MISO引脚*/
        if (NRF24L01_R_MISO())
            txData |= 0x01;				//如果为1，则给Byte最低位置1，左移默认0

        /*产生SCK下降沿*/
        NRF24L01_W_SCK(0);
        Delay_us(1);
    }
    /*返回Byte数据，此时的Byte为SPI交换接收得到的一个字节数据*/
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
 *简	介：读NRF24L01寄存器
 *参	数：寄存器地址
 *返回值：寄存器状态
 *说	明：上层函数调用
 *********************************************************************/
uint8_t NRF24L01_R_Reg(uint8_t Reg_adress)
{
    uint8_t status;
    /*拉低片选，开始通信*/
    NRF24L01_W_CS(0);
    /*发送命令码和寄存器地址*/
    SPI_SwapByte(NRF24L01_R_REGISTER|Reg_adress);
    /*空字节维持时钟，交换寄存器信息*/
    status = SPI_SwapByte(dummy);
    /*拉高片选，结束通信*/
    NRF24L01_W_CS(1);
    return status;
}

/********************************************************************
 *简	介：读NRF24L01寄存器多字节
 *参	数：寄存器地址，缓存区，读多少字节
 *返回值：寄存器状态
 *说	明：此函数主要用于部分需要多字节读入的寄存器，如接收地址，发送地址
 *注	意：读多少字节应小于缓存区大小
 *********************************************************************/
uint8_t* NRF24L01_R_Regs(uint8_t Reg_adress,uint8_t* Rece,uint8_t size)
{
    /*拉低片选，开始通信*/
    NRF24L01_W_CS(0);
    /*发送命令码和寄存器地址*/
    SPI_SwapByte(NRF24L01_R_REGISTER|Reg_adress);
    /*写入寄存器数据*/
    for(uint8_t i=0; i<size; i++)
    {
        (*(Rece+i))=SPI_SwapByte(dummy);
    }
    /*拉高片选，结束通信*/
    NRF24L01_W_CS(1);
    return Rece;
}

/********************************************************************
 *简	介：写NRF24L01寄存器
 *参	数：寄存器地址，写入的数据
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_W_Reg(uint8_t Reg_adress,uint8_t Data)
{
    /*拉低片选，开始通信*/
    NRF24L01_W_CS(0);
    /*发送命令码和寄存器地址*/
    SPI_SwapByte(NRF24L01_W_REGISTER|Reg_adress);
    /*写入寄存器数据*/
    SPI_SwapByte(Data);
    /*拉高片选，结束通信*/
    NRF24L01_W_CS(1);
}

/********************************************************************
 *简	介：写NRF24L01寄存器多字节
 *参	数：寄存器地址，要写入的数据，数据个数
 *返回值：无
 *说	明：此函数主要用于部分需要多字节写入的寄存器，如接收地址，发送地址
 *********************************************************************/
void NRF24L01_W_Regs(uint8_t Reg_adress,uint8_t* Data,uint8_t size)
{
    /*拉低片选，开始通信*/
    NRF24L01_W_CS(0);
    /*发送命令码和寄存器地址*/
    SPI_SwapByte(NRF24L01_W_REGISTER|Reg_adress);
    /*写入寄存器数据*/
    for(uint8_t i=0; i<size; i++)
    {
        SPI_SwapByte(*(Data+i));
    }
    /*拉高片选，结束通信*/
    NRF24L01_W_CS(1);
}

/********************************************************************
 *简	介：修改2.4G信号频段
 *参	数：2.4g频段【2400-2525】Mhz
 *返回值：无
 *说	明：计算公式F= 2400 + RF_CH [MHz]
 *********************************************************************/
void NRF24L01_SetFrequencyMHz(uint16_t freqMHz)
{
    // 限制范围 2400 MHz ~ 2525 MHz
    if (freqMHz < 2400) freqMHz = 2400;
    if (freqMHz > 2525) freqMHz = 2525;
    uint8_t channel = (uint8_t)(freqMHz - 2400);
    NRF24L01_W_Reg(NRF24L01_RF_CH, channel);
}

/********************************************************************
 *简	介：修改空中数据速率
 *参	数：查看宏定义中【RF_SETUP】
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_SetAir_data_rate(Reg_RF_SETUP RF_DR)
{
    uint8_t RF_SETUP_Reg=0;
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    RF_SETUP_Reg = NRF24L01_R_Reg(NRF24L01_RF_SETUP);
    /*位操作*/
    RF_SETUP_Reg &= ~(RF_DR_Config_Bit);			//	(RF_SETUP_Reg & (1 1 0 1		 0 1 1 0)) = (x x 0 x 		0 x x 0)清零位3、5,最低位必须为0
    /*写入空中数据速率*/
    RF_SETUP_Reg |= RF_DR;
    /*写回寄存器*/
    NRF24L01_W_Reg(NRF24L01_RF_SETUP,RF_SETUP_Reg);
}

/*********************************************************************
 *简	介：修改射频输出功率
 *参	数：查看宏定义中【RF_SETUP】
 *返回值：无
 *说	明：无
 **********************************************************************/
void NRF24L01_PA_Control(Reg_RF_SETUP RF_PWR)
{
    uint8_t RF_SETUP_Reg=0;
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    RF_SETUP_Reg = NRF24L01_R_Reg(NRF24L01_RF_SETUP);
    /*位操作*/
    RF_SETUP_Reg &= ~(RF_PWR_Config_Bit);			//	(RF_SETUP_Reg & (1 1 1 1		1 0 0 0)) = (x x x x 		x 0 0 0)清零位1、2，最低位必须为0
    /*写入射频输出功率*/
    RF_SETUP_Reg |= RF_PWR;
    /*写回寄存器*/
    NRF24L01_W_Reg(NRF24L01_RF_SETUP,RF_SETUP_Reg);
}

/********************************************************************
 *简	介：清空FIFOTx
 *参	数：无
 *返回值：无
 *说	明：FIFOTX有三级，只能一次清除
 *********************************************************************/
void NRF24L01_ClearFIFOTx()
{
    /*拉低CS，开始通信*/
    NRF24L01_W_CS(0);
    /*发送【清空FIFOTx】命令字节*/
    SPI_SwapByte(NRF24L01_FLUSH_TX);
    /*拉高CS，结束通信*/
    NRF24L01_W_CS(1);
    /*等待清除完成，具体时间待测*/
    Delay_us(20);
}

/********************************************************************
 *简	介：清空FIFORx
 *参	数：无
 *返回值：无
 *说	明：FIFORX有三级，只能一次清除
 *********************************************************************/
void NRF24L01_ClearFIFORx()
{
    /*拉低CS，开始通信*/
    NRF24L01_W_CS(0);
    /*发送【清空FIFORx】命令字节*/
    SPI_SwapByte(NRF24L01_FLUSH_RX);
    /*拉高CS，结束通信*/
    NRF24L01_W_CS(1);
    /*等待清除完成，具体时间待测*/
    Delay_us(20);
}

/********************************************************************
 *简	介：读RXFIFO
 *参	数：缓存区结构体
 *返回值：无
 *说	明：此函数用于读取RX
 *注	意：缓存区大小要 >= 个数
 *********************************************************************/
void NRF24L01_R_RxFIFO(NRF24L01_Pack* Rece)
{
	uint8_t length = 0;		//数据长度
	/*获取下一个数据来自哪个管道*/
	uint8_t Pipe = NRF24L01_GetDataPipe();
	if(Pipe > 5)		return;		//RxFIFO内无数据
	/*判断此管道是动态载荷还是静态载荷*/
	if((NRF24L01_R_Reg(NRF24L01_FEATURE) & EN_DPL_ENABLE) && (NRF24L01_R_Reg(NRF24L01_DYNPD) & (0x01<<Pipe)))		//无需在意数据位，if只判断是不是0
	//通道号与动态载荷的使能刚好一一对应，左移刚好得到对应通道的动态载荷使能情况，只有动态载荷的总开关和通道的动态载荷同时使能才算使能动态载荷
	{
		/*动态载荷，用命令获取长度*/
		NRF24L01_W_CS(0);		//开始通信
		SPI_SwapByte(NRF24L01_R_RX_PL_WID);		//发送命令字节
		length = SPI_SwapByte(dummy);		//哑元字节维持时钟，得到长度
		NRF24L01_W_CS(1);		//结束通信
	}
	else		//静态载荷，读取对应寄存器
	{
		length = NRF24L01_R_Reg(NRF24L01_RX_PW_P0 + Pipe);		//地址连续，加上Pipe偏移即可
	}
	/*判断参数合法*/
	if(length == 0 || length > 32)
		return;
	/*更新数据*/
	NRF24L01_W_CS(0);
	/*发送命令字节*/
	SPI_SwapByte(NRF24L01_R_RX_PAYLOAD);
	/*读取数据*/
	for(uint8_t i = 0;i < length;i++)
	{
		Rece->Data[i] = SPI_SwapByte(dummy);		//哑元字节维持时钟
	}
	/*结束通信*/
	NRF24L01_W_CS(1);
	/*更新通道信息*/
	Rece->pipe = Pipe;	
}

/********************************************************************
 *简	介：写TX
 *参	数：指令，要写的数据，数据大小，管道号
 *返回值：无
 *说	明：有三种写TX指令，其中写ACK附带载荷需要指定管道号，Pipe_X参数只在此时有用
 *********************************************************************/
void NRF24L01_W_Tx(W_TX_Command W_TX,uint8_t* Send,uint8_t size,uint8_t Pipe_X)
{
    /*确保最大就32字节*/
    if(size 	> 32)		size = 32;
    /*拉低CS，开始通信*/
    NRF24L01_W_CS(0);
		/*根据命令完成不同操作*/
		if(W_TX == W_TX_ACK_PAYLOAD)
		{
			if(Pipe_X > 5	)	Pipe_X = 5;			//防止传入0-5以外的参数
			SPI_SwapByte(W_TX | Pipe_X);		//ACK附带有效载荷需指定管道号
		}
		else
		{
			SPI_SwapByte(W_TX);		//剩下两种直接写入即可
		}
    /*SPI保持发送*/
    for(uint8_t i=0; i<size; i++)
    {
        SPI_SwapByte(Send[i]);  //往TX内写数据
    }
    /*拉高CS，结束通信*/
    NRF24L01_W_CS(1);
}

/********************************************************************
 *简	介：进入TX
 *参	数：无
 *返回值：无
 *说	明：这里实际是进入【待机模式1】，发送数据需要CE脉冲
 *********************************************************************/
void NRF24L01_EnterTx()
{
    uint8_t config_reg=0;
		/*拉低CE*/
		NRF24L01_W_CE(0);
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    config_reg = 	NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*清除第0，1位*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*将PWR_UP置1,PRIM_Rx置0*/
    config_reg |= PWR_UP;				// x x x x		x x 1 0，PRIM_RX在上面已经是0
    /*写回寄存器*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,config_reg);
}

/********************************************************************
 *简	介：进入RX
 *参	数：无
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_EnterRx()
{
    uint8_t config_reg=0;
	  /*拉低CE，修改寄存器*/
    NRF24L01_W_CE(0);
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    config_reg = 	NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*清除第0，1位*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*将PWR_UP和PRIM_Rx置1*/
    config_reg |= (PWR_UP | PRIM_RX);			// x x x x		x x 1 1
    /*写回寄存器*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,config_reg);
    /*拉高CE，持续监听*/
    NRF24L01_W_CE(1);
}

/********************************************************************
 *简	介：进入待机模式1
 *参	数：无
 *返回值：无
 *说	明：用于修改寄存器值
 *********************************************************************/
void NRF24L01_EnterStandby_1(void)
{
	  uint8_t config_reg=0;
		/*拉低CE*/
		NRF24L01_W_CE(0);
		/*拉低CE*/
		NRF24L01_W_CE(0);
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    config_reg = 	NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*清除第0，1位*/
    config_reg &= ~(PRIM_RX | PWR_UP);
    /*将PWR_UP置1,PRIM_Rx置0*/
    config_reg |= PWR_UP;				// x x x x		x x 1 0，PRIM_RX在上面已经是0
    /*写回寄存器*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,config_reg);
}

/********************************************************************
 *简	介：进入掉电模式
 *参	数：无
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_EnterPowerDown(void)
{
	   uint8_t config_reg=0;
		/*拉低CE*/
		NRF24L01_W_CE(0);
    /*先读取，再位操作，重写回，不影响其他位*/
    /*读取*/
    config_reg = 	NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*清除PWR_UP位*/
    config_reg &= ~(PWR_UP);						// x x x x		x x 0 x
    /*写回寄存器*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,config_reg);
}

/********************************************************************
 *简	介：发送数据
 *参	数：无
 *返回值：无
 *说	明：产生CE脉冲
 *********************************************************************/
void NRF24L01_SendData(void)
{
    /*进入TX，此时CE没有脉冲*/
    NRF24L01_EnterTx();
    /*产生脉冲*/
    NRF24L01_W_CE(1);
    Delay_us(20);
    NRF24L01_W_CE(0);
		/*剩余内容在中断中处理*/
		/*在中断中重新进入接收模式*/
}

/********************************************************************
 *简	介：使能通道自动应答
 *参	数：通道，使能还是失能
 *返回值：无
 *说	明：这个函数只能操作一个通道
 *********************************************************************/
void NRF24L01_EN_AutoACK(Reg_EN_AA EN_AA,NRF24L01_Status new_status)
{
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_EN_AA);
    /*清0对应位置*/
    Reg &= ~EN_AA;		//这里通道与数据位刚好一一对应
    /*判断使能还是失能*/
    if(new_status)		//使能置位，失能不管
        Reg |= EN_AA;
    /*重新写回数据*/
    NRF24L01_W_Reg(NRF24L01_EN_AA,Reg);
}

/********************************************************************
 *简	介：使能接收通道
 *参	数：接收通道，使能还是失能
 *返回值：无
 *说	明：这个函数只能操作一个通道
 *********************************************************************/
void NRF24L01_EN_RxAddr(Reg_EN_RXADDR EN_RXADDR,NRF24L01_Status new_status)
{
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_EN_RXADDR);
    /*清0对应位置*/
    Reg &= ~EN_RXADDR;		//这里通道与数据位刚好一一对应
    /*判断使能还是失能*/
    if(new_status)		//使能置位，失能不管
        Reg |= EN_RXADDR;
    /*重新写回数据*/
    NRF24L01_W_Reg(NRF24L01_EN_RXADDR,Reg);
}

/********************************************************************
 *简	介：设置地址宽度
 *参	数：地址宽度
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_Set_Addr_Width(Reg_SETUP_AW Address_Width)
{
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_SETUP_AW);
    /*第0位与第1位清0*/
    Reg &= ~(Address_Width_Config_Bit);
    /*写入参数*/
    Reg |= Address_Width;
    /*重新写回数据*/
    NRF24L01_W_Reg(NRF24L01_SETUP_AW,Reg);
}

/********************************************************************
 *简	介：设置自动重传
 *参	数：重传延迟，重传次数
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_SET_RETR(Reg_SETUP_RETR_ARD ARD,Reg_SETUP_RETR_ARC ARC)
{
    /*直接写入即可*/
    NRF24L01_W_Reg(NRF24L01_SETUP_RETR,(ARD | ARC));
}

/********************************************************************
 *简	介：使能动态载荷
 *参	数：通道，使能还是失能
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_EN_DPL(Reg_DYNPD EN_DPL,NRF24L01_Status new_status)
{
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_DYNPD);
    /*清0对应位置*/
    Reg &= ~EN_DPL;		//这里通道与数据位刚好一一对应
    /*判断使能还是失能*/
    if(new_status)		//使能置位，失能不管
    {
        Reg |= EN_DPL;
        NRF24L01_EN_AutoACK( (Reg_EN_AA)EN_DPL, NRF24L01_ENABLE );		//这里DPL也是与通道一一对应的，需要确保开启了自动应答
        NRF24L01_EN_FEATURE(EN_DPL_ENABLE,NRF24L01_ENABLE);						//确保动态载荷总开关开启
    }
    /*重新写回数据*/
    NRF24L01_W_Reg(NRF24L01_DYNPD,Reg);
}

/********************************************************************
 *简	介：使能特殊功能
 *参	数：特殊功能，使能还是失能
 *返回值：无
 *说	明：动态载荷总开关，带负载ACK，数据包可无ACK
 *********************************************************************/
void NRF24L01_EN_FEATURE(Reg_FEATURE EN_FEATURE,NRF24L01_Status new_status)
{
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_FEATURE);
    /*清0对应位置*/
    Reg &= ~EN_FEATURE;		//这里特殊功能与数据位一一对应
    /*判断使能还是失能*/
    if(new_status)		//使能置位，失能不管
        Reg |= EN_FEATURE;
    /*重新写回数据*/
    NRF24L01_W_Reg(NRF24L01_FEATURE,Reg);
}

/********************************************************************
 *简	介：设置CRC校验码字节数
 *参	数：CRC字节数
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_Set_CRCByte(Reg_CONFIG CRC_x)
{
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*清0对应位置*/
    Reg &= ~(1<<2);
    /*写入参数*/
    Reg |= CRC_x;		//如果是1字节，第2位就是0，如果是两字节第2位就是1
    /*重新写回数据*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,Reg);
}

/********************************************************************
 *简	介：使能中断
 *参	数：中断位，使能还是失能
 *返回值：无
 *说	明：最大重传中断，发送中断，接收中断
 *********************************************************************/
void NRF24L01_EN_Interrupt(Reg_CONFIG MASK_Int,NRF24L01_Status new_status)
{
    /*读取寄存器*/
    uint8_t Reg = NRF24L01_R_Reg(NRF24L01_CONFIG);
    /*清0对应位置*/
    Reg &= ~MASK_Int;				//写 0 使能中断
    /*判断使能还是失能*/
    if(new_status == NRF24L01_DISABLE)		//使能写0不管，失能写1，芯片规定 0 使能，1失能
        Reg |= MASK_Int;		//写 1 失能中断
    /*重新写回数据*/
    NRF24L01_W_Reg(NRF24L01_CONFIG,Reg);
}

/********************************************************************
 *简	介：设置接收通道地址
 *参	数：通道X，要设置的地址数组，数组长度
 *返回值：无
 *说	明：数组长度要满足要求
 *********************************************************************/
void NRF24L01_Set_RxAddr(Reg_RX_ADDR RX_ADDR_Px,uint8_t* Addr,uint8_t Addr_length)
{
    /*空指针检查*/
    if(!Addr)
        return;
    /*确定写入通道，决定最大地址字节数*/
    if((RX_ADDR_Px == RX_ADDR_P0) || (RX_ADDR_Px == RX_ADDR_P1))		//通道 0 和通道 1 最多可以写入5字节
    {
        /*获取芯片设置的地址长度*/
        uint8_t Reg_length = (NRF24L01_R_Reg(NRF24L01_SETUP_AW) & 0x03) + 2;		//加2是因为如果是3字节地址，寄存器的值是0x01，四字节地址就是0x02，加2正好是几字节地址,&0x03确保取低2位
        /*确定写入数组长度，防止非法访问*/
        uint8_t count = (Addr_length >= Reg_length) ? Reg_length : Addr_length;
        NRF24L01_W_Regs(RX_ADDR_Px,Addr,count);
    }
    else		//通道 2 到通道 5 只能写入1字节地址
    {
			if(Addr_length >= 1)		//确保至少有一个数据
        NRF24L01_W_Reg(RX_ADDR_Px,Addr[0]);
    }
}

/********************************************************************
 *简	介：设置目标地址
 *参	数：目标地址数组，数组长度
 *返回值：无
 *说	明：数组长度要满足要求
 *********************************************************************/
void NRF24L01_Set_TxAddr(uint8_t* Addr,uint8_t Addr_length)
{
	/*检查参数合法*/
	if(Addr_length>= 5)
		Addr_length = 5;
	NRF24L01_W_Regs(NRF24L01_TX_ADDR,Addr,Addr_length);
}

/********************************************************************
 *简	介：获取中断标志位
 *参	数：中断位
 *返回值：无
 *说	明：无
 *********************************************************************/
NRF24L01_FLAG_Status NRF24L01_GetITFLAG(Reg_STSTUS IT_FLAG)
{
	/*检查参数有效*/
	if((IT_FLAG == MAX_RT) || (IT_FLAG == TX_DS) || (IT_FLAG == RX_DR))
	{
		if((NRF24L01_R_Reg(NRF24L01_STATUS) & IT_FLAG))	//检查标志位，如果是0，&操作会清0所有位，if不成立
			return NRF24L01_SET;
		else
			return NRF24L01_RESET;
	}
	else		//参数错误
		return NRF24L01_RESET;
}

/********************************************************************
 *简	介：清除中断标志位
 *参	数：中断位
 *返回值：无
 *说	明：无
 *********************************************************************/
void NRF24L01_ClearITFLAG(Reg_STSTUS IT_FLAG)
{
	/*检查参数有效*/
	if((IT_FLAG == MAX_RT) || (IT_FLAG == TX_DS) || (IT_FLAG == RX_DR))
	{
		/*读*/
		uint8_t status = NRF24L01_R_Reg(NRF24L01_STATUS);
		/*清除标志位*/
		status |= IT_FLAG;		//写 1 清除标志位
		/*写回寄存器*/
		NRF24L01_W_Reg(NRF24L01_STATUS,status);
	}
}

/********************************************************************
 *简	介：设置接收通道接收字节数【静态载荷】
 *参	数：通道，字节数
 *返回值：无
 *说	明：传入的通道就是对应寄存器的地址
 *********************************************************************/
void NRF24L01_Set_RxNum(Reg_RX_PW RX_PW_Px,uint8_t count)
{
	/*检查参数合法性*/
	if(count > 32)
		count = 32;
	NRF24L01_W_Reg(RX_PW_Px,count);
}

/********************************************************************
 *简	介：读取接收通道接收字节数【静态载荷】
 *参	数：通道
 *返回值：无
 *说	明：传入的通道就是对应寄存器的地址
 *********************************************************************/
uint8_t NRF24L01_R_RxNum(Reg_RX_PW RX_PW_Px)
{
	/*检查参数合法性*/
	if((RX_PW_Px >= RX_PW_P0) && (RX_PW_Px <= RX_PW_P5))		//这里寄存器地址是连续的，只需要判断地址的大小即可
		return NRF24L01_R_Reg(RX_PW_Px);
	else
		return 0;
}

/********************************************************************
 *简	介：获取数据来自哪个管道
 *参	数：无
 *返回值：数据管道
 *说	明：无
 *********************************************************************/
uint8_t NRF24L01_GetDataPipe(void)
{
	return((NRF24L01_R_Reg(NRF24L01_STATUS) & RX_P_NO_Config_Bit) >> 1);		//右移一位将数据位对齐到第0位，正好就是对应通道,&操作取出配置位
}




