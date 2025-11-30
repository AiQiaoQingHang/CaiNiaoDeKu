#ifndef ICM42688_USE_H
#define ICM42688_USE_H

#include "icm42688_Drv.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define ICM_Addr 0x68

extern ICM42688_Driver_t ICM_Drv;   // ICM驱动结构体
extern TaskHandle_t ICM_Taskhandle; // ICM传感器任务句柄
extern QueueHandle_t ICM_DMA_Busy;  // ICM传感器DMA空闲二值信号量句柄

void USER_ICM_Init(void);                         // 初始化ICM42688
void ICM_FreeRTOSTask( void * arg );              // ICM的FreeRTOS任务

#endif