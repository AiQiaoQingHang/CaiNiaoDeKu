#ifndef __NRF24L01_DEFINE_H
#define __NRF24L01_DEFINE_H

/*NRF24L01指令宏定义*/
#define NRF24L01_R_REGISTER						0x00					//读寄存器，高3位为指令码，低5位为寄存器地址，后续跟1~5字节读数据
#define NRF24L01_W_REGISTER						0x20					//写寄存器，高3位为指令码，低5位为寄存器地址，后续跟1~5字节写数据
#define NRF24L01_R_RX_PAYLOAD					0x61					//读Rx有效载荷，后续跟1~32字节读数据
#define NRF24L01_W_TX_PAYLOAD					0xA0					//写Tx有效载荷，后续跟1~32字节写数据
#define NRF24L01_FLUSH_TX							0xE1					//清空Tx FIFO所有数据，单独指令
#define NRF24L01_FLUSH_RX							0xE2					//清空Rx FIFO所有数据，单独指令
#define NRF24L01_REUSE_TX_PL					0xE3					//重新使用最后一次发送的有效载荷，单独指令
#define NRF24L01_R_RX_PL_WID					0x60					//动态包长情况下获取RX FIFO中第一个数据包的长度
#define NRF24L01_W_ACK_PAYLOAD				0xA8					//写应答附带的有效载荷，高5位为指令码，低3位为通道号，后续跟1~32字节写数据，仅适用于应答附带载荷模式
#define NRF24L01_W_TX_PAYLOAD_NOACK		0xB0					//写Tx有效载荷，不要求应答，后续跟1~32字节写数据，仅适用于不要求应答模式
#define NRF24L01_NOP									0xFF					//空操作，单独指令，可以用读取状态寄存器

/*NRF24L01寄存器地址宏定义*/
#define NRF24L01_CONFIG								0x00					//配置寄存器，1字节
#define NRF24L01_EN_AA								0x01					//使能自动应答，1字节
#define NRF24L01_EN_RXADDR						0x02					//使能接收通道，1字节
#define NRF24L01_SETUP_AW							0x03					//设置地址宽度，1字节
#define NRF24L01_SETUP_RETR						0x04					//设置自动重传，1字节
#define NRF24L01_RF_CH								0x05					//射频通道，1字节
#define NRF24L01_RF_SETUP							0x06					//射频相关参数设置，1字节
#define NRF24L01_STATUS								0x07					//状态寄存器，1字节
#define NRF24L01_OBSERVE_TX						0x08					//发送观察寄存器，1字节
#define NRF24L01_RPD									0x09					//接收功率检测，1字节
#define NRF24L01_RX_ADDR_P0						0x0A					//接收通道0地址，5字节
#define NRF24L01_RX_ADDR_P1						0x0B					//接收通道1地址，5字节
#define NRF24L01_RX_ADDR_P2						0x0C					//接收通道2地址，1字节，高位地址与接收通道1相同
#define NRF24L01_RX_ADDR_P3						0x0D					//接收通道3地址，1字节，高位地址与接收通道1相同
#define NRF24L01_RX_ADDR_P4						0x0E					//接收通道4地址，1字节，高位地址与接收通道1相同
#define NRF24L01_RX_ADDR_P5						0x0F					//接收通道5地址，1字节，高位地址与接收通道1相同
#define NRF24L01_TX_ADDR							0x10					//发送地址，5字节
#define NRF24L01_RX_PW_P0							0x11					//接收通道 0 有效载荷数据宽度，1字节
#define NRF24L01_RX_PW_P1							0x12					//接收通道 1 有效载荷的数据宽度，1字节
#define NRF24L01_RX_PW_P2							0x13					//接收通道 2 有效载荷的数据宽度，1字节
#define NRF24L01_RX_PW_P3							0x14					//接收通道 3 有效载荷的数据宽度，1字节
#define NRF24L01_RX_PW_P4							0x15					//接收通道 4 有效载荷的数据宽度，1字节
#define NRF24L01_RX_PW_P5							0x16					//接收通道 5 有效载荷的数据宽度，1字节
#define NRF24L01_FIFO_STATUS					0x17					//发送和接收FIFO状态，1字节
#define NRF24L01_DYNPD								0x1C					//使能接收通道的动态包长模式，1字节
#define NRF24L01_FEATURE							0x1D					//使能高级功能，1字节

/*寄存器状态*/
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
 PRIM_RX                  =   	(1<<0),             //模式选择【1 接收】【0 发送】
 PWR_UP                   =   	(1<<1),             //上电
 CRC_1_byte               =   	(0<<2),             //一字节CRC
 CRC_2_byte               =   	(1<<2),             //两字节CRC
 ENCRC                    =   	(1<<3),             //使能CRC
 MASK_MAX_RT              =   	(1<<4),             //最大重传中断使能位
 MASK_TX_DS               =   	(1<<5),             //接收中断使能位
 MASK_RX_DR							  =	 	  (1<<6),             //发送中断使能位
}Reg_CONFIG;

/*----------------------------- STSTUS ----------------------------*/
typedef enum {
 TX_FULL                  =     (1<<0),             //TX_FIFO满，只读
 RX_P_NO_Config_Bit       = ((1<<1)|(1<<2)|(1<<3)), //判断接收数据从哪个通道来的
 MAX_RT                   =     (1<<4),             //最大重传中断标志位
 TX_DS                    =     (1<<5),             //发送中断标志位
 RX_DR                    =     (1<<6),             //接收中断标志位
}Reg_STSTUS;

/*----------------------------- RX_TX_ADDR ---------------------------*/
/*注:这里存的实际是寄存器地址，用枚举方便参数检查*/
typedef enum {
 RX_ADDR_P0							 	=			0x0A,               //通道 0 接收字节数
 RX_ADDR_P1              	=     0x0B,               //通道 1 接收字节数
 RX_ADDR_P2              	=     0x0C,               //通道 2 接收字节数
 RX_ADDR_P3              	=     0x0D,               //通道 3 接收字节数
 RX_ADDR_P4              	=     0x0E,               //通道 4 接收字节数
 RX_ADDR_P5              	=     0x0F,               //通道 5 接收字节数
}Reg_RX_ADDR;

/*------------------------------- RX_PW -----------------------------*/
/*注:这里存的实际是寄存器地址，用枚举方便参数检查*/
typedef enum {
 RX_PW_P0							 		=			0x11,               //通道 0 接收地址
 RX_PW_P1              		=     0x12,               //通道 1 接收地址
 RX_PW_P2              		=     0x13,               //通道 2 接收地址
 RX_PW_P3              		=     0x14,               //通道 3 接收地址
 RX_PW_P4              		=     0x15,               //通道 4 接收地址
 RX_PW_P5              		=     0x16,               //通道 5 接收地址
}Reg_RX_PW;

/*----------------------------- EN_AA -----------------------------*/
typedef enum {
 EN_AA_P0									 =		(1<<0), 						//启用通道 0 自动应答
 EN_AA_P1									 =		(1<<1), 						//启用通道 1 自动应答
 EN_AA_P2									 =		(1<<2), 						//启用通道 2 自动应答
 EN_AA_P3									 =		(1<<3), 						//启用通道 3 自动应答
 EN_AA_P4									 =		(1<<4), 						//启用通道 4 自动应答
 EN_AA_P5									 =		(1<<5), 						//启用通道 5 自动应答
}Reg_EN_AA;

/*--------------------------- EN_RXADDR ---------------------------*/
typedef enum {
 ERX_P0                    =  	(1<<0),  						//启用接收通道 0
 ERX_P1                    =  	(1<<1),  						//启用接收通道 1
 ERX_P2                    =  	(1<<2),  						//启用接收通道 2
 ERX_P3                    =  	(1<<3),  						//启用接收通道 3
 ERX_P4                    =  	(1<<4),  						//启用接收通道 4
 ERX_P5                    =  	(1<<5),  						//启用接收通道 5
}Reg_EN_RXADDR;

/*--------------------------- SETUP_AW ---------------------------*/
typedef enum {
 Address_Width_3           = 			0x01, 						//三字节地址
 Address_Width_4           = 			0x02,  						//四字节地址
 Address_Width_5        	 =			0x03,   					//五字节地址
 Address_Width_Config_Bit  = ((1<<0)|(1<<1)),     	//地址字节数配置位
}Reg_SETUP_AW;

/*--------------------------- SETUP_RETR --------------------------*/
/**
 * Auto Retransmit Count (ARC) - 自动重传次数
 * 重传次数: 0-15 (0 = 禁用自动重传)
 * 占用位: [3:0]
 */
typedef enum {
 ARC_DISABLE             	 =  				0x00, 				// 禁用自动重传
 ARC_1                   	 =  				0x01, 				// 最大重传 1 次 
 ARC_2                   	 =  				0x02, 				// 最大重传 2 次 
 ARC_3                   	 =  				0x03, 				// 最大重传 3 次 
 ARC_4                   	 =  				0x04, 				// 最大重传 4 次 
 ARC_5                   	 =  				0x05, 				// 最大重传 5 次 
 ARC_6                   	 =  				0x06, 				// 最大重传 6 次 
 ARC_7                   	 =  				0x07, 				// 最大重传 7 次 
 ARC_8                   	 =  				0x08, 				// 最大重传 8 次 
 ARC_9                   	 =  				0x09, 				// 最大重传 9 次 
 ARC_10                  	 =  				0x0A, 				// 最大重传 10 次
 ARC_11                  	 =  				0x0B, 				// 最大重传 11 次
 ARC_12                  	 =  				0x0C, 				// 最大重传 12 次
 ARC_13                  	 =  				0x0D, 				// 最大重传 13 次
 ARC_14                  	 =  				0x0E, 				// 最大重传 14 次
 ARC_15                  	 =  				0x0F, 				// 最大重传 15 次
}Reg_SETUP_RETR_ARC;

/**
 * Auto Retransmit Delay (ARD) - 自动重传延迟时间
 * 延迟时间 = (ARD + 1) × 250μs
 * 占用位: [7:4]
 */
typedef enum {
 ARD_250us                 =  		(0x00 << 4),   		// 延迟 250  us
 ARD_500us                 =  		(0x01 << 4),   		// 延迟 500  us
 ARD_750us                 =  		(0x02 << 4),   		// 延迟 750  us
 ARD_1000us                =  		(0x03 << 4),   		// 延迟 1000 us
 ARD_1250us                =  		(0x04 << 4),   		// 延迟 1250 us
 ARD_1500us                =  		(0x05 << 4),   		// 延迟 1500 us
 ARD_1750us                =  		(0x06 << 4),   		// 延迟 1750 us
 ARD_2000us                =  		(0x07 << 4),   		// 延迟 2000 us
 ARD_2250us                =  		(0x08 << 4),   		// 延迟 2250 us
 ARD_2500us                =  		(0x09 << 4),   		// 延迟 2500 us
 ARD_2750us                =  		(0x0A << 4),   		// 延迟 2750 us
 ARD_3000us                =  		(0x0B << 4),   		// 延迟 3000 us
 ARD_3250us                =  		(0x0C << 4),   		// 延迟 3250 us
 ARD_3500us                =  		(0x0D << 4),   		// 延迟 3500 us
 ARD_3750us                =  		(0x0E << 4),   		// 延迟 3750 us
 ARD_4000us                =  		(0x0F << 4),   		// 延迟 4000 us
}Reg_SETUP_RETR_ARD;

/*------------------------ RF_SETUP -------------------------------*/
typedef enum {
 RF_PWR_18dBm              =	((0<<1)|(0<<2)),    	// -18 dBm
 RF_PWR_12dBm              =	((1<<1)|(0<<2)),    	// -12 dBm  
 RF_PWR_6dBm               =	((0<<1)|(1<<2)),   		// - 6 dBm 
 RF_PWR_0dBm               =	((1<<1)|(1<<2)),   		//   0 dBm 
 RF_PWR_Config_Bit         = 	((1<<1)|(1<<2)),			//	PWR配置位
		
 RF_DR_1Mbps               =	((0<<3)|(0<<5)),			//	1Mbps   (RF_DR_LOW=0, RF_DR_HIGH=0)
 RF_DR_2Mbps               =			(1 << 3),  				//	2Mbps   (RF_DR_LOW=0, RF_DR_HIGH=1)
 RF_DR_250Kbps             =			(1 << 5), 				//	250kbps (RF_DR_LOW=1, RF_DR_HIGH=0)
 RF_DR_Config_Bit          = ((1 << 5)|(1 << 3)), 	//	DR配置位
}Reg_RF_SETUP;

/*-------------------------- DYNPD --------------------------------*/
/*需要EN_DPL和EN_AA,分别在【FEATURE】和【EN_AA】寄存器中*/
typedef enum {
 EN_DPL_P0								 =			(1<<0),         	//启用通道 0 动态载荷
 EN_DPL_P1								 =			(1<<1),         	//启用通道 1 动态载荷
 EN_DPL_P2								 =			(1<<2),         	//启用通道 2 动态载荷
 EN_DPL_P3								 =			(1<<3),         	//启用通道 3 动态载荷
 EN_DPL_P4								 =			(1<<4),         	//启用通道 4 动态载荷
 EN_DPL_P5								 =			(1<<5),         	//启用通道 5 动态载荷
}Reg_DYNPD;

/*------------------------ FEATURE --------------------------------*/
typedef enum {
 EN_DYN_ACK_ENABLE				 =			(1<<0),         	//启用 W_TX_PAYLOAD_NOACK 命令，数据包可以没有ACK
 EN_ACK_PAY_ENABLE         =  		(1<<1),        		//启用带负载的ACK
 EN_DPL_ENABLE             =  		(1<<2),        		//启用动态载荷
}Reg_FEATURE;

/*---------------------- W_TX_Command -----------------------------*/
/*注:这里存的实际是命令字节，用枚举方便参数检查*/
typedef enum {
 W_TX_PAYLOAD				 			=				0xA0,         		//写TX有效载荷
 W_TX_ACK_PAYLOAD         =  			0xA8,        			//写ACK附带的有效载荷
 W_TX_PAYLOAD_NOACK    		=  			0xB0,        			//写TX无需ACK
}W_TX_Command;

#endif
