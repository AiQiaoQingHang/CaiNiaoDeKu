#ifndef MQTT_H
#define MQTT_H
#include "esp_err.h"
#include "mqtt_client.h"

extern esp_mqtt_client_handle_t Onenet_handle;

#define PRODUCT_ID "2p1u6QB8b3"                                           // 产品ID
#define PRODUCT_ACCESS_KEY "qxU5HTvZxfvPOUG5BTufRZ2Hvaclzz97+dM/MQF9vb8=" // 产品密钥
#define PRODUCT_DEVICE_NAME "esp32led"                                    // 设备名称

esp_err_t Onenet_Start(void);
#endif
