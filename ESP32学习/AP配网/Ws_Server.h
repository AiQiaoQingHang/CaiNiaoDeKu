#ifndef WS_SERVER_H
#define WS_SERVER_H
#include "esp_err.h"
typedef void (*Ws_Rece_cb)(uint8_t *PayLoad, uint16_t Len); // 接收回调函数

esp_err_t Web_Ws_Start(const char *html, Ws_Rece_cb Rece_CallBack);
void Web_Ws_Stop(void);
esp_err_t Web_Ws_Send(uint8_t *Data, uint16_t Len);

#endif