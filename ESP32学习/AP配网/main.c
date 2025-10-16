#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "WIFI.h"

#define WIFI_SSID "11111111"
#define WIFI_PASS "25802580"

void app_main(void)
{
    WIFI_Init();
    WIFI_Enter_STA();
    vTaskDelay(pdMS_TO_TICKS(15000));

    while (1)
    {
        WIFI_Scan(10, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
