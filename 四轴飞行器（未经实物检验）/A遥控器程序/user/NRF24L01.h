#ifndef NRF24L01
#define NRF24L01
#include "stm32f10x.h"                 
#include "Delay.h"
#include "NRF24L01_define.h"																																					//����NRF24L01�Ĵ���ӳ����ָ��궨��
#define dummy 0xFF                                                             	  										//��Ԫ�ֽ�

/*�޸���������ע�⣺
 *�޸��ж�������Ҫע���ж��������жϺ�����һ���ԣ�
 *�����Ӳ��SPI������ӳ����Ҫ�ֶ��޸ġ�NRF24L01_SPI_Init()����������롾My_SPI���ĺ궨��
 */
 
 
typedef struct{
	uint8_t Data[32];     //����
	uint8_t pipe;         //�ܵ���
}NRF24L01_Pack;

extern  NRF24L01_Pack Rx_Buffer;																																			//Rx������
extern volatile int8_t SendFLAG;																																			//���ͱ�־λ		   	��0 ������	��1 ���ͳ�ʱ��
extern volatile int8_t Receive_DataFLAG;																															//�������ݱ�־λ 	��0 �����ݡ���1 �����ݡ�

#define Sofa_SPI_ENABLE		1                                                                       		//�Ƿ��������SPI
#define My_SPI					SPI1																																					//Ӳ��SPIѡ������ֻ�Ƿ���SPI������ӳ�䣬���Ҫ������ӳ��ֻ��Ҫ��SPI��ʼ�����������ӳ����޸Ĵ˴�SPI��ѡ�񼴿ɣ������޸���������



#if Sofa_SPI_ENABLE == 0		//�������SPI��1 ���SPI����0 Ӳ��SPI��
/*-------------------------------------------- �� �� S P I �� �� �� --------------------------------------------*/
/*-------------------------------------------- �� �� �� �� �� �� �� --------------------------------------------*/										
#define NRF24L01_SCK_Port 						GPIOA                                                   				//SCK����
#define NRF24L01_SCK_Pin  						GPIO_Pin_5                                              				//SCK��Pin��
														
#define NRF24L01_MISO_Port 						GPIOA                                                  					//MISO����
#define NRF24L01_MISO_Pin  						GPIO_Pin_6                                             					//MISO��Pin��
										
#define NRF24L01_MOSI_Port 						GPIOA                                                  					//MOSI����
#define NRF24L01_MOSI_Pin  						GPIO_Pin_7                                             					//MOSI��Pin��
										
#define NRF24L01_CS_Port 							GPIOA		                                                				//CS����
#define NRF24L01_CS_Pin  							GPIO_Pin_8                                               				//CS��Pin��
											
#define NRF24L01_CE_Port 							GPIOA		                                                				//CE����
#define NRF24L01_CE_Pin  							GPIO_Pin_4                                               				//CE��Pin��
									
#define NRF24L01_CLOCK_SCK  					RCC_APB2Periph_GPIOA                                  					//SCK��ʱ��
#define NRF24L01_CLOCK_MISO 					RCC_APB2Periph_GPIOA                                  					//MISO��ʱ��
#define NRF24L01_CLOCK_MOSI 					RCC_APB2Periph_GPIOA                                  					//MOSI��ʱ��
#define NRF24L01_CLOCK_CS   					RCC_APB2Periph_GPIOA                                  					//CS��ʱ��
#define NRF24L01_CLOCK_CE   					RCC_APB2Periph_GPIOA                                  					//CE��ʱ��
/*---------- �� �� S P I �� �� �� �� ----------*/
void NRF24L01_SPI_Init(void);                                                    											//���ų�ʼ��
void NRF24L01_W_CS(uint8_t status);                                               										//дCS
void NRF24L01_W_CE(uint8_t status);                                               										//дCE
void NRF24L01_W_MOSI(uint8_t status);                                             										//дMOSI
void NRF24L01_W_SCK(uint8_t status);                                              										//дSCK
uint8_t NRF24L01_R_MISO(void);                                                    										//��MISO
uint8_t SPI_SwapByte(uint8_t txData);                                             										//SPI����һ�ֽ�



#else									//����Ӳ��SPI
/*-------------------------------------------- Ӳ �� S P I �� �� �� --------------------------------------------*/	
/*ע�⣺��ӳ����Ҫ�����޸ĺ���*/
#define NRF24L01_CS_Port 							GPIOA		                                                				//CS����
#define NRF24L01_CS_Pin  							GPIO_Pin_8                                               				//CS��Pin��
											
#define NRF24L01_CE_Port 							GPIOA		                                                				//CE����
#define NRF24L01_CE_Pin  							GPIO_Pin_4                                               				//CE��Pin��

#define NRF24L01_CLOCK_CS   					RCC_APB2Periph_GPIOA                                  					//CS��ʱ��
#define NRF24L01_CLOCK_CE   					RCC_APB2Periph_GPIOA                                  					//CE��ʱ��
/*---------- Ӳ �� S P I �� �� �� �� ----------*/
void NRF24L01_SPI_Init(void);                                                    										 	//���ų�ʼ��
void NRF24L01_W_CS(uint8_t status);                                               										//дCS
void NRF24L01_W_CE(uint8_t status);                                               										//дCE
uint8_t SPI_SwapByte(uint8_t txData);                                             										//SPI����һ�ֽ�
#endif



/*------------------------------------------------- �� �� �� �� -------------------------------------------------*/
//GPIO
#define NRF24L01_IRQ_Port 						GPIOA		                                                				//IRQ����
#define NRF24L01_IRQ_Pin  						GPIO_Pin_11                                               			//IRQ��Pin��
#define NRF24L01_CLOCK_IRQ   					RCC_APB2Periph_GPIOA                                  					//IRQ��ʱ��
//EXTI
#define	EXTI_Line_Source							EXTI_Line11																											//EXTI�����ã���������һ��
#define EXTI_Line_Port								GPIO_PortSourceGPIOA																						//EXTI��ӳ�䣬������һ��
#define EXTI_Line_Pin									GPIO_PinSource11																								//EXTI������ӳ�䣬������һ��
//NVIC
#define NVIC_IRQChannel_Source				EXTI15_10_IRQn																									//�ж�ͨ��ʹ�ܣ�������һ��
#define NVIC_SubPriority							2																																//��Ӧ���ȼ�
#define NVIC_PreemptionPriority				2																																//��ռ���ȼ�
/*---------- �� �� �� �� ----------*/
void NRF24L01_IRQHandler(void);                                                                       //�жϷ������������ⲿ�ж�
void NRF24L01_EXTI_Init(void);																																				//��ʼ���ⲿ�ж�
void EXTI15_10_IRQHandler(void);																																			//�ⲿ�жϷ�����



/*---------------------------------------N R F 2 4 L 0 1 �� �� ģ �� �� �� --------------------------------------*/
/*--------- �� д �� �� �� ---------*/
uint8_t NRF24L01_R_Reg(uint8_t Reg_adress);                                       										//���Ĵ���
uint8_t* NRF24L01_R_Regs(uint8_t Reg_adress,uint8_t* Rece,uint8_t size);                              //���Ĵ������ֽ�
void NRF24L01_W_Reg(uint8_t Reg_adress,uint8_t Data);                             										//д�Ĵ���
void NRF24L01_W_Regs(uint8_t Reg_adress,uint8_t* Data,uint8_t size);                                  //д�Ĵ������ֽ�
/*---------- �� �� �� �� ----------*/
void NRF24L01_Init(void);                                                                             //��ʼ��NRF24L01
void NRF24L01_EnterPowerDown(void);                                                                   //�������ģʽ
void NRF24L01_EnterTx(void);                                                                          //���뷢��ģʽ
void NRF24L01_EnterRx(void);                                                                          //�������ģʽ
void NRF24L01_EnterStandby_1(void);                                                                   //�������ģʽ1
void NRF24L01_ClearFIFOTx(void);                                                             					//���FIFOTx
void NRF24L01_ClearFIFORx(void);                                                             					//���FIFORx
void NRF24L01_R_RxFIFO(NRF24L01_Pack* Rece);                                               						//��Rx_FIFO����̬�غɡ�����̬�غɡ�
void NRF24L01_W_Tx(W_TX_Command W_TX,uint8_t* Send,uint8_t size,uint8_t Pipe_X);                      //дTx_FIFO
void NRF24L01_SendData(void);																																					//��������
NRF24L01_FLAG_Status NRF24L01_GetITFLAG(Reg_STSTUS IT_FLAG);																					//��ȡ�жϱ�־λ
void NRF24L01_ClearITFLAG(Reg_STSTUS IT_FLAG);                                                        //����жϱ�־λ
uint8_t NRF24L01_R_RxNum(Reg_RX_PW RX_PW_Px);                                                					//��ȡ����ͨ�������ֽ�������̬�غɡ�
uint8_t NRF24L01_GetDataPipe(void);                                                                   //��ȡ���������ĸ��ܵ�
/*---------- �� �� �� �� ----------*/
void NRF24L01_EN_AutoACK(Reg_EN_AA EN_AA,NRF24L01_Status new_status);                                 //ʹ���Զ�Ӧ��
void NRF24L01_EN_RxAddr(Reg_EN_RXADDR ERX,NRF24L01_Status new_status);                          			//ʹ�ܽ���ͨ��
void NRF24L01_Set_Addr_Width(Reg_SETUP_AW Address_Width);                                             //���õ�ַ����
void NRF24L01_SET_RETR(Reg_SETUP_RETR_ARD ARD,Reg_SETUP_RETR_ARC ARC);                                //�����Զ��ش���ʱ���Զ��ش�����
void NRF24L01_SetAir_data_rate(Reg_RF_SETUP RF_DR);                                                   //���ÿ�����������
void NRF24L01_PA_Control(Reg_RF_SETUP RF_PWR);                                                        //������Ƶ�������
void NRF24L01_SetFrequencyMHz(uint16_t freqMHz);                                                      //����2.4gͨ��Ƶ��
void NRF24L01_EN_DPL(Reg_DYNPD EN_DPL,NRF24L01_Status new_status);                                    //ʹ��ͨ����̬�غ�
void NRF24L01_EN_FEATURE(Reg_FEATURE EN_FEATURE,NRF24L01_Status new_status);                          //ʹ�����⹦��
void NRF24L01_Set_CRCByte(Reg_CONFIG CRC_x);                                                          //���ü��ֽ�CRCУ����
void NRF24L01_EN_Interrupt(Reg_CONFIG MASK_Int,NRF24L01_Status new_status);                           //ʹ���ж�
void NRF24L01_Set_RxAddr(Reg_RX_ADDR RX_ADDR_Px,uint8_t* Addr,uint8_t Addr_length);										//���ý���ͨ����ַ
void NRF24L01_Set_TxAddr(uint8_t* Addr,uint8_t Addr_length);																					//����Ŀ���ַ
void NRF24L01_Set_RxNum(Reg_RX_PW RX_PW_Px,uint8_t count);                                            //���ý���ͨ�������ֽ���
#endif
