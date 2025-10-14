#ifndef NRF24L01
#define NRF24L01
#include "stm32f10x.h"                 
#include "Delay.h"
#include "NRF24L01_define.h"																																					//引用NRF24L01寄存器映射与指令宏定义
#define dummy 0xFF                                                             	  										//哑元字节

/*修改引脚配置注意：
 *修改中断引脚需要注意中断引脚与中断函数的一致性，
 *如果对硬件SPI进行重映射需要手动修改【NRF24L01_SPI_Init()】这个函数与【My_SPI】的宏定义
 */
 
 
typedef struct{
	uint8_t Data[32];     //缓存
	uint8_t pipe;         //管道号
}NRF24L01_Pack;

extern  NRF24L01_Pack Rx_Buffer;																																			//Rx缓存区
extern volatile int8_t SendFLAG;																																			//发送标志位		   	【0 正常】	【1 发送超时】
extern volatile int8_t Receive_DataFLAG;																															//接收数据标志位 	【0 无数据】【1 有数据】

#define Sofa_SPI_ENABLE		1                                                                       		//是否启用软件SPI
#define My_SPI					SPI1																																					//硬件SPI选择，这里只是方便SPI引脚重映射，如果要进行重映射只需要在SPI初始化部分添加重映射和修改此处SPI的选择即可，无需修改其他部分



#if Sofa_SPI_ENABLE == 0		//启用软件SPI【1 软件SPI】【0 硬件SPI】
/*-------------------------------------------- 软 件 S P I 宏 定 义 --------------------------------------------*/
/*-------------------------------------------- 引 脚 可 以 自 定 义 --------------------------------------------*/										
#define NRF24L01_SCK_Port 						GPIOA                                                   				//SCK引脚
#define NRF24L01_SCK_Pin  						GPIO_Pin_5                                              				//SCK的Pin脚
														
#define NRF24L01_MISO_Port 						GPIOA                                                  					//MISO引脚
#define NRF24L01_MISO_Pin  						GPIO_Pin_6                                             					//MISO的Pin脚
										
#define NRF24L01_MOSI_Port 						GPIOA                                                  					//MOSI引脚
#define NRF24L01_MOSI_Pin  						GPIO_Pin_7                                             					//MOSI的Pin脚
										
#define NRF24L01_CS_Port 							GPIOA		                                                				//CS引脚
#define NRF24L01_CS_Pin  							GPIO_Pin_8                                               				//CS的Pin脚
											
#define NRF24L01_CE_Port 							GPIOA		                                                				//CE引脚
#define NRF24L01_CE_Pin  							GPIO_Pin_4                                               				//CE的Pin脚
									
#define NRF24L01_CLOCK_SCK  					RCC_APB2Periph_GPIOA                                  					//SCK的时钟
#define NRF24L01_CLOCK_MISO 					RCC_APB2Periph_GPIOA                                  					//MISO的时钟
#define NRF24L01_CLOCK_MOSI 					RCC_APB2Periph_GPIOA                                  					//MOSI的时钟
#define NRF24L01_CLOCK_CS   					RCC_APB2Periph_GPIOA                                  					//CS的时钟
#define NRF24L01_CLOCK_CE   					RCC_APB2Periph_GPIOA                                  					//CE的时钟
/*---------- 软 件 S P I 功 能 函 数 ----------*/
void NRF24L01_SPI_Init(void);                                                    											//引脚初始化
void NRF24L01_W_CS(uint8_t status);                                               										//写CS
void NRF24L01_W_CE(uint8_t status);                                               										//写CE
void NRF24L01_W_MOSI(uint8_t status);                                             										//写MOSI
void NRF24L01_W_SCK(uint8_t status);                                              										//写SCK
uint8_t NRF24L01_R_MISO(void);                                                    										//读MISO
uint8_t SPI_SwapByte(uint8_t txData);                                             										//SPI交换一字节



#else									//启用硬件SPI
/*-------------------------------------------- 硬 件 S P I 宏 定 义 --------------------------------------------*/	
/*注意：重映射需要自行修改函数*/
#define NRF24L01_CS_Port 							GPIOA		                                                				//CS引脚
#define NRF24L01_CS_Pin  							GPIO_Pin_8                                               				//CS的Pin脚
											
#define NRF24L01_CE_Port 							GPIOA		                                                				//CE引脚
#define NRF24L01_CE_Pin  							GPIO_Pin_4                                               				//CE的Pin脚

#define NRF24L01_CLOCK_CS   					RCC_APB2Periph_GPIOA                                  					//CS的时钟
#define NRF24L01_CLOCK_CE   					RCC_APB2Periph_GPIOA                                  					//CE的时钟
/*---------- 硬 件 S P I 功 能 函 数 ----------*/
void NRF24L01_SPI_Init(void);                                                    										 	//引脚初始化
void NRF24L01_W_CS(uint8_t status);                                               										//写CS
void NRF24L01_W_CE(uint8_t status);                                               										//写CE
uint8_t SPI_SwapByte(uint8_t txData);                                             										//SPI交换一字节
#endif



/*------------------------------------------------- 中 断 定 义 -------------------------------------------------*/
//GPIO
#define NRF24L01_IRQ_Port 						GPIOA		                                                				//IRQ引脚
#define NRF24L01_IRQ_Pin  						GPIO_Pin_11                                               			//IRQ的Pin脚
#define NRF24L01_CLOCK_IRQ   					RCC_APB2Periph_GPIOA                                  					//IRQ的时钟
//EXTI
#define	EXTI_Line_Source							EXTI_Line11																											//EXTI线配置，需与引脚一致
#define EXTI_Line_Port								GPIO_PortSourceGPIOA																						//EXTI重映射，与引脚一致
#define EXTI_Line_Pin									GPIO_PinSource11																								//EXTI引脚重映射，与引脚一致
//NVIC
#define NVIC_IRQChannel_Source				EXTI15_10_IRQn																									//中断通道使能，与引脚一致
#define NVIC_SubPriority							2																																//响应优先级
#define NVIC_PreemptionPriority				2																																//抢占优先级
/*---------- 中 断 函 数 ----------*/
void NRF24L01_IRQHandler(void);                                                                       //中断服务函数，依赖外部中断
void NRF24L01_EXTI_Init(void);																																				//初始化外部中断
void EXTI15_10_IRQHandler(void);																																			//外部中断服务函数



/*---------------------------------------N R F 2 4 L 0 1 无 线 模 块 函 数 --------------------------------------*/
/*--------- 读 写 寄 存 器 ---------*/
uint8_t NRF24L01_R_Reg(uint8_t Reg_adress);                                       										//读寄存器
uint8_t* NRF24L01_R_Regs(uint8_t Reg_adress,uint8_t* Rece,uint8_t size);                              //读寄存器多字节
void NRF24L01_W_Reg(uint8_t Reg_adress,uint8_t Data);                             										//写寄存器
void NRF24L01_W_Regs(uint8_t Reg_adress,uint8_t* Data,uint8_t size);                                  //写寄存器多字节
/*---------- 功 能 函 数 ----------*/
void NRF24L01_Init(void);                                                                             //初始化NRF24L01
void NRF24L01_EnterPowerDown(void);                                                                   //进入掉电模式
void NRF24L01_EnterTx(void);                                                                          //进入发送模式
void NRF24L01_EnterRx(void);                                                                          //进入接收模式
void NRF24L01_EnterStandby_1(void);                                                                   //进入待机模式1
void NRF24L01_ClearFIFOTx(void);                                                             					//清空FIFOTx
void NRF24L01_ClearFIFORx(void);                                                             					//清空FIFORx
void NRF24L01_R_RxFIFO(NRF24L01_Pack* Rece);                                               						//读Rx_FIFO【静态载荷】【动态载荷】
void NRF24L01_W_Tx(W_TX_Command W_TX,uint8_t* Send,uint8_t size,uint8_t Pipe_X);                      //写Tx_FIFO
void NRF24L01_SendData(void);																																					//发送数据
NRF24L01_FLAG_Status NRF24L01_GetITFLAG(Reg_STSTUS IT_FLAG);																					//获取中断标志位
void NRF24L01_ClearITFLAG(Reg_STSTUS IT_FLAG);                                                        //清除中断标志位
uint8_t NRF24L01_R_RxNum(Reg_RX_PW RX_PW_Px);                                                					//读取接收通道接收字节数【静态载荷】
uint8_t NRF24L01_GetDataPipe(void);                                                                   //获取数据来自哪个管道
/*---------- 配 置 函 数 ----------*/
void NRF24L01_EN_AutoACK(Reg_EN_AA EN_AA,NRF24L01_Status new_status);                                 //使能自动应答
void NRF24L01_EN_RxAddr(Reg_EN_RXADDR ERX,NRF24L01_Status new_status);                          			//使能接收通道
void NRF24L01_Set_Addr_Width(Reg_SETUP_AW Address_Width);                                             //设置地址长度
void NRF24L01_SET_RETR(Reg_SETUP_RETR_ARD ARD,Reg_SETUP_RETR_ARC ARC);                                //设置自动重传延时，自动重传次数
void NRF24L01_SetAir_data_rate(Reg_RF_SETUP RF_DR);                                                   //设置空中数据速率
void NRF24L01_PA_Control(Reg_RF_SETUP RF_PWR);                                                        //设置射频输出功率
void NRF24L01_SetFrequencyMHz(uint16_t freqMHz);                                                      //设置2.4g通信频段
void NRF24L01_EN_DPL(Reg_DYNPD EN_DPL,NRF24L01_Status new_status);                                    //使能通道动态载荷
void NRF24L01_EN_FEATURE(Reg_FEATURE EN_FEATURE,NRF24L01_Status new_status);                          //使能特殊功能
void NRF24L01_Set_CRCByte(Reg_CONFIG CRC_x);                                                          //设置几字节CRC校验码
void NRF24L01_EN_Interrupt(Reg_CONFIG MASK_Int,NRF24L01_Status new_status);                           //使能中断
void NRF24L01_Set_RxAddr(Reg_RX_ADDR RX_ADDR_Px,uint8_t* Addr,uint8_t Addr_length);										//设置接收通道地址
void NRF24L01_Set_TxAddr(uint8_t* Addr,uint8_t Addr_length);																					//设置目标地址
void NRF24L01_Set_RxNum(Reg_RX_PW RX_PW_Px,uint8_t count);                                            //设置接收通道接收字节数
#endif
