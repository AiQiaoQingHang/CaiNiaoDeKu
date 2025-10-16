#ifndef ONENEAT_DM_H
#define ONENEAT_DM_H

#include "cJSON.h"

void Onenet_Property_ProcHandle(cJSON *Property);                                                                             // 【属性】下行数据处理
cJSON *Onenet_Property_UpData(void);                                                                                          // 【属性】上行数据打包
void Onenet_Property_Ack(const char *Product_ID, const char *Product_Device_Name, const char *id, int code, const char *msg); // 【属性】下行应答
void Onenet_SUBSCRIBE_Topic(const char *Product_ID, const char *Product_Device_Name);                                         // 订阅主题
void Onenet_Property_Push(const char *Product_ID, const char *Product_Device_Name, char *Data);                               // 客户端发送数据
#endif