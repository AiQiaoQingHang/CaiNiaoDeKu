#ifndef WATCHDOG_H
#define WATCHDOG_H


#define Dog_time 96											//看门狗时间，计算公式【1.6ms * x】，范围【0x00-0xFFF】
void Watch_dog_Init(void);							//看门狗初始化


#endif
