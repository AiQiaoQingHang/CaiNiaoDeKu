#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "WIFI.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "MQTT.h"
#include "driver/gpio.h"

void app_main(void)
{
    gpio_config_t gpio_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = (1UL << 4),
    };
    gpio_config(&gpio_cfg);
    gpio_set_level(GPIO_NUM_4, 0);

    WIFI_Init();
    WIFI_Enter_STA();
    WIFI_Connect("1111111", "25802580");
    Onenet_Start();
}