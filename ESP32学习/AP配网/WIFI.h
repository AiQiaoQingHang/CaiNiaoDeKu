#ifndef WIFI_H
#define WIFI_H
#include "esp_event.h"
#include "esp_err.h"
#include <stdint.h>

esp_err_t WIFI_Init();
esp_err_t WIFI_Enter_STA();
esp_err_t WIFI_Enter_AP(uint8_t channel, uint8_t max_connection, const char *ssid, const char *password);
esp_err_t WIFI_Connect(const char *ssid, const char *password);
esp_err_t WIFI_Scan(uint16_t Scan_List_Size, uint8_t Task_Priority, uint8_t Task_Core_I);
#endif