#ifndef __NRF24L01_DEFINE_H
#define __NRF24L01_DEFINE_H

/*NRF24L01ָ��궨��*/
#define NRF24L01_R_REGISTER						0x00					//���Ĵ�������3λΪָ���룬��5λΪ�Ĵ�����ַ��������1~5�ֽڶ�����
#define NRF24L01_W_REGISTER						0x20					//д�Ĵ�������3λΪָ���룬��5λΪ�Ĵ�����ַ��������1~5�ֽ�д����
#define NRF24L01_R_RX_PAYLOAD					0x61					//��Rx��Ч�غɣ�������1~32�ֽڶ�����
#define NRF24L01_W_TX_PAYLOAD					0xA0					//дTx��Ч�غɣ�������1~32�ֽ�д����
#define NRF24L01_FLUSH_TX							0xE1					//���Tx FIFO�������ݣ�����ָ��
#define NRF24L01_FLUSH_RX							0xE2					//���Rx FIFO�������ݣ�����ָ��
#define NRF24L01_REUSE_TX_PL					0xE3					//����ʹ�����һ�η��͵���Ч�غɣ�����ָ��
#define NRF24L01_R_RX_PL_WID					0x60					//��̬��������»�ȡRX FIFO�е�һ�����ݰ��ĳ���
#define NRF24L01_W_ACK_PAYLOAD				0xA8					//дӦ�𸽴�����Ч�غɣ���5λΪָ���룬��3λΪͨ���ţ�������1~32�ֽ�д���ݣ���������Ӧ�𸽴��غ�ģʽ
#define NRF24L01_W_TX_PAYLOAD_NOACK		0xB0					//дTx��Ч�غɣ���Ҫ��Ӧ�𣬺�����1~32�ֽ�д���ݣ��������ڲ�Ҫ��Ӧ��ģʽ
#define NRF24L01_NOP									0xFF					//�ղ���������ָ������ö�ȡ״̬�Ĵ���

/*NRF24L01�Ĵ�����ַ�궨��*/
#define NRF24L01_CONFIG								0x00					//���üĴ�����1�ֽ�
#define NRF24L01_EN_AA								0x01					//ʹ���Զ�Ӧ��1�ֽ�
#define NRF24L01_EN_RXADDR						0x02					//ʹ�ܽ���ͨ����1�ֽ�
#define NRF24L01_SETUP_AW							0x03					//���õ�ַ��ȣ�1�ֽ�
#define NRF24L01_SETUP_RETR						0x04					//�����Զ��ش���1�ֽ�
#define NRF24L01_RF_CH								0x05					//��Ƶͨ����1�ֽ�
#define NRF24L01_RF_SETUP							0x06					//��Ƶ��ز������ã�1�ֽ�
#define NRF24L01_STATUS								0x07					//״̬�Ĵ�����1�ֽ�
#define NRF24L01_OBSERVE_TX						0x08					//���͹۲�Ĵ�����1�ֽ�
#define NRF24L01_RPD									0x09					//���չ��ʼ�⣬1�ֽ�
#define NRF24L01_RX_ADDR_P0						0x0A					//����ͨ��0��ַ��5�ֽ�
#define NRF24L01_RX_ADDR_P1						0x0B					//����ͨ��1��ַ��5�ֽ�
#define NRF24L01_RX_ADDR_P2						0x0C					//����ͨ��2��ַ��1�ֽڣ���λ��ַ�����ͨ��1��ͬ
#define NRF24L01_RX_ADDR_P3						0x0D					//����ͨ��3��ַ��1�ֽڣ���λ��ַ�����ͨ��1��ͬ
#define NRF24L01_RX_ADDR_P4						0x0E					//����ͨ��4��ַ��1�ֽڣ���λ��ַ�����ͨ��1��ͬ
#define NRF24L01_RX_ADDR_P5						0x0F					//����ͨ��5��ַ��1�ֽڣ���λ��ַ�����ͨ��1��ͬ
#define NRF24L01_TX_ADDR							0x10					//���͵�ַ��5�ֽ�
#define NRF24L01_RX_PW_P0							0x11					//����ͨ�� 0 ��Ч�غ����ݿ�ȣ�1�ֽ�
#define NRF24L01_RX_PW_P1							0x12					//����ͨ�� 1 ��Ч�غɵ����ݿ�ȣ�1�ֽ�
#define NRF24L01_RX_PW_P2							0x13					//����ͨ�� 2 ��Ч�غɵ����ݿ�ȣ�1�ֽ�
#define NRF24L01_RX_PW_P3							0x14					//����ͨ�� 3 ��Ч�غɵ����ݿ�ȣ�1�ֽ�
#define NRF24L01_RX_PW_P4							0x15					//����ͨ�� 4 ��Ч�غɵ����ݿ�ȣ�1�ֽ�
#define NRF24L01_RX_PW_P5							0x16					//����ͨ�� 5 ��Ч�غɵ����ݿ�ȣ�1�ֽ�
#define NRF24L01_FIFO_STATUS					0x17					//���ͺͽ���FIFO״̬��1�ֽ�
#define NRF24L01_DYNPD								0x1C					//ʹ�ܽ���ͨ���Ķ�̬����ģʽ��1�ֽ�
#define NRF24L01_FEATURE							0x1D					//ʹ�ܸ߼����ܣ�1�ֽ�

/*�Ĵ���״̬*/
typedef enum {
 NRF24L01_ENABLE					=				1,
 NRF24L01_DISABLE					=				0,
} NRF24L01_Status;

typedef enum {
 NRF24L01_SET							=				1,
 NRF24L01_RESET						=				0,
} NRF24L01_FLAG_Status;

/*----------------------------- CONFIG ----------------------------*/
typedef enum {
 PRIM_RX                  =   	(1<<0),             //ģʽѡ��1 ���ա���0 ���͡�
 PWR_UP                   =   	(1<<1),             //�ϵ�
 CRC_1_byte               =   	(0<<2),             //һ�ֽ�CRC
 CRC_2_byte               =   	(1<<2),             //���ֽ�CRC
 ENCRC                    =   	(1<<3),             //ʹ��CRC
 MASK_MAX_RT              =   	(1<<4),             //����ش��ж�ʹ��λ
 MASK_TX_DS               =   	(1<<5),             //�����ж�ʹ��λ
 MASK_RX_DR							  =	 	  (1<<6),             //�����ж�ʹ��λ
}Reg_CONFIG;

/*----------------------------- STSTUS ----------------------------*/
typedef enum {
 TX_FULL                  =     (1<<0),             //TX_FIFO����ֻ��
 RX_P_NO_Config_Bit       = ((1<<1)|(1<<2)|(1<<3)), //�жϽ������ݴ��ĸ�ͨ������
 MAX_RT                   =     (1<<4),             //����ش��жϱ�־λ
 TX_DS                    =     (1<<5),             //�����жϱ�־λ
 RX_DR                    =     (1<<6),             //�����жϱ�־λ
}Reg_STSTUS;

/*----------------------------- RX_TX_ADDR ---------------------------*/
/*ע:������ʵ���ǼĴ�����ַ����ö�ٷ���������*/
typedef enum {
 RX_ADDR_P0							 	=			0x0A,               //ͨ�� 0 �����ֽ���
 RX_ADDR_P1              	=     0x0B,               //ͨ�� 1 �����ֽ���
 RX_ADDR_P2              	=     0x0C,               //ͨ�� 2 �����ֽ���
 RX_ADDR_P3              	=     0x0D,               //ͨ�� 3 �����ֽ���
 RX_ADDR_P4              	=     0x0E,               //ͨ�� 4 �����ֽ���
 RX_ADDR_P5              	=     0x0F,               //ͨ�� 5 �����ֽ���
}Reg_RX_ADDR;

/*------------------------------- RX_PW -----------------------------*/
/*ע:������ʵ���ǼĴ�����ַ����ö�ٷ���������*/
typedef enum {
 RX_PW_P0							 		=			0x11,               //ͨ�� 0 ���յ�ַ
 RX_PW_P1              		=     0x12,               //ͨ�� 1 ���յ�ַ
 RX_PW_P2              		=     0x13,               //ͨ�� 2 ���յ�ַ
 RX_PW_P3              		=     0x14,               //ͨ�� 3 ���յ�ַ
 RX_PW_P4              		=     0x15,               //ͨ�� 4 ���յ�ַ
 RX_PW_P5              		=     0x16,               //ͨ�� 5 ���յ�ַ
}Reg_RX_PW;

/*----------------------------- EN_AA -----------------------------*/
typedef enum {
 EN_AA_P0									 =		(1<<0), 						//����ͨ�� 0 �Զ�Ӧ��
 EN_AA_P1									 =		(1<<1), 						//����ͨ�� 1 �Զ�Ӧ��
 EN_AA_P2									 =		(1<<2), 						//����ͨ�� 2 �Զ�Ӧ��
 EN_AA_P3									 =		(1<<3), 						//����ͨ�� 3 �Զ�Ӧ��
 EN_AA_P4									 =		(1<<4), 						//����ͨ�� 4 �Զ�Ӧ��
 EN_AA_P5									 =		(1<<5), 						//����ͨ�� 5 �Զ�Ӧ��
}Reg_EN_AA;

/*--------------------------- EN_RXADDR ---------------------------*/
typedef enum {
 ERX_P0                    =  	(1<<0),  						//���ý���ͨ�� 0
 ERX_P1                    =  	(1<<1),  						//���ý���ͨ�� 1
 ERX_P2                    =  	(1<<2),  						//���ý���ͨ�� 2
 ERX_P3                    =  	(1<<3),  						//���ý���ͨ�� 3
 ERX_P4                    =  	(1<<4),  						//���ý���ͨ�� 4
 ERX_P5                    =  	(1<<5),  						//���ý���ͨ�� 5
}Reg_EN_RXADDR;

/*--------------------------- SETUP_AW ---------------------------*/
typedef enum {
 Address_Width_3           = 			0x01, 						//���ֽڵ�ַ
 Address_Width_4           = 			0x02,  						//���ֽڵ�ַ
 Address_Width_5        	 =			0x03,   					//���ֽڵ�ַ
 Address_Width_Config_Bit  = ((1<<0)|(1<<1)),     	//��ַ�ֽ�������λ
}Reg_SETUP_AW;

/*--------------------------- SETUP_RETR --------------------------*/
/**
 * Auto Retransmit Count (ARC) - �Զ��ش�����
 * �ش�����: 0-15 (0 = �����Զ��ش�)
 * ռ��λ: [3:0]
 */
typedef enum {
 ARC_DISABLE             	 =  				0x00, 				// �����Զ��ش�
 ARC_1                   	 =  				0x01, 				// ����ش� 1 �� 
 ARC_2                   	 =  				0x02, 				// ����ش� 2 �� 
 ARC_3                   	 =  				0x03, 				// ����ش� 3 �� 
 ARC_4                   	 =  				0x04, 				// ����ش� 4 �� 
 ARC_5                   	 =  				0x05, 				// ����ش� 5 �� 
 ARC_6                   	 =  				0x06, 				// ����ش� 6 �� 
 ARC_7                   	 =  				0x07, 				// ����ش� 7 �� 
 ARC_8                   	 =  				0x08, 				// ����ش� 8 �� 
 ARC_9                   	 =  				0x09, 				// ����ش� 9 �� 
 ARC_10                  	 =  				0x0A, 				// ����ش� 10 ��
 ARC_11                  	 =  				0x0B, 				// ����ش� 11 ��
 ARC_12                  	 =  				0x0C, 				// ����ش� 12 ��
 ARC_13                  	 =  				0x0D, 				// ����ش� 13 ��
 ARC_14                  	 =  				0x0E, 				// ����ش� 14 ��
 ARC_15                  	 =  				0x0F, 				// ����ش� 15 ��
}Reg_SETUP_RETR_ARC;

/**
 * Auto Retransmit Delay (ARD) - �Զ��ش��ӳ�ʱ��
 * �ӳ�ʱ�� = (ARD + 1) �� 250��s
 * ռ��λ: [7:4]
 */
typedef enum {
 ARD_250us                 =  		(0x00 << 4),   		// �ӳ� 250  us
 ARD_500us                 =  		(0x01 << 4),   		// �ӳ� 500  us
 ARD_750us                 =  		(0x02 << 4),   		// �ӳ� 750  us
 ARD_1000us                =  		(0x03 << 4),   		// �ӳ� 1000 us
 ARD_1250us                =  		(0x04 << 4),   		// �ӳ� 1250 us
 ARD_1500us                =  		(0x05 << 4),   		// �ӳ� 1500 us
 ARD_1750us                =  		(0x06 << 4),   		// �ӳ� 1750 us
 ARD_2000us                =  		(0x07 << 4),   		// �ӳ� 2000 us
 ARD_2250us                =  		(0x08 << 4),   		// �ӳ� 2250 us
 ARD_2500us                =  		(0x09 << 4),   		// �ӳ� 2500 us
 ARD_2750us                =  		(0x0A << 4),   		// �ӳ� 2750 us
 ARD_3000us                =  		(0x0B << 4),   		// �ӳ� 3000 us
 ARD_3250us                =  		(0x0C << 4),   		// �ӳ� 3250 us
 ARD_3500us                =  		(0x0D << 4),   		// �ӳ� 3500 us
 ARD_3750us                =  		(0x0E << 4),   		// �ӳ� 3750 us
 ARD_4000us                =  		(0x0F << 4),   		// �ӳ� 4000 us
}Reg_SETUP_RETR_ARD;

/*------------------------ RF_SETUP -------------------------------*/
typedef enum {
 RF_PWR_18dBm              =	((0<<1)|(0<<2)),    	// -18 dBm
 RF_PWR_12dBm              =	((1<<1)|(0<<2)),    	// -12 dBm  
 RF_PWR_6dBm               =	((0<<1)|(1<<2)),   		// - 6 dBm 
 RF_PWR_0dBm               =	((1<<1)|(1<<2)),   		//   0 dBm 
 RF_PWR_Config_Bit         = 	((1<<1)|(1<<2)),			//	PWR����λ
		
 RF_DR_1Mbps               =	((0<<3)|(0<<5)),			//	1Mbps   (RF_DR_LOW=0, RF_DR_HIGH=0)
 RF_DR_2Mbps               =			(1 << 3),  				//	2Mbps   (RF_DR_LOW=0, RF_DR_HIGH=1)
 RF_DR_250Kbps             =			(1 << 5), 				//	250kbps (RF_DR_LOW=1, RF_DR_HIGH=0)
 RF_DR_Config_Bit          = ((1 << 5)|(1 << 3)), 	//	DR����λ
}Reg_RF_SETUP;

/*-------------------------- DYNPD --------------------------------*/
/*��ҪEN_DPL��EN_AA,�ֱ��ڡ�FEATURE���͡�EN_AA���Ĵ�����*/
typedef enum {
 EN_DPL_P0								 =			(1<<0),         	//����ͨ�� 0 ��̬�غ�
 EN_DPL_P1								 =			(1<<1),         	//����ͨ�� 1 ��̬�غ�
 EN_DPL_P2								 =			(1<<2),         	//����ͨ�� 2 ��̬�غ�
 EN_DPL_P3								 =			(1<<3),         	//����ͨ�� 3 ��̬�غ�
 EN_DPL_P4								 =			(1<<4),         	//����ͨ�� 4 ��̬�غ�
 EN_DPL_P5								 =			(1<<5),         	//����ͨ�� 5 ��̬�غ�
}Reg_DYNPD;

/*------------------------ FEATURE --------------------------------*/
typedef enum {
 EN_DYN_ACK_ENABLE				 =			(1<<0),         	//���� W_TX_PAYLOAD_NOACK ������ݰ�����û��ACK
 EN_ACK_PAY_ENABLE         =  		(1<<1),        		//���ô����ص�ACK
 EN_DPL_ENABLE             =  		(1<<2),        		//���ö�̬�غ�
}Reg_FEATURE;

/*---------------------- W_TX_Command -----------------------------*/
/*ע:������ʵ���������ֽڣ���ö�ٷ���������*/
typedef enum {
 W_TX_PAYLOAD				 			=				0xA0,         		//дTX��Ч�غ�
 W_TX_ACK_PAYLOAD         =  			0xA8,        			//дACK��������Ч�غ�
 W_TX_PAYLOAD_NOACK    		=  			0xB0,        			//дTX����ACK
}W_TX_Command;

#endif
