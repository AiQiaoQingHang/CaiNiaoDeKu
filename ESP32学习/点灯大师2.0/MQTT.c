#include "MQTT.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>
#include "onenet_token.h"
#include "esp_event.h"
#include "Onenet_DM.h"
#include "cJSON.h"

static const char *TAG = "MQTT";

esp_mqtt_client_handle_t Onenet_handle = NULL;

static void MQTT_Event_cb(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        /* 订阅 */
        Onenet_SUBSCRIBE_Topic(PRODUCT_ID, PRODUCT_DEVICE_NAME);
        /* 连接上后同步状态 */
        cJSON *Data = Onenet_Property_UpData();
        char *Send_Data = cJSON_PrintUnformatted(Data);
        Onenet_Property_Push(PRODUCT_ID, PRODUCT_DEVICE_NAME, Send_Data);
        free(Send_Data);
        cJSON_Delete(Data);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED: // 服务器确认收到数据
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA: // 客户端收到数据
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        if (strstr(event->topic, "/thing/property/set"))
        {
            /* 解析服务器cjson */
            cJSON *Data = cJSON_Parse(event->data);
            Onenet_Property_ProcHandle(Data);
            /* 获取ID */
            cJSON *id_js = cJSON_GetObjectItem(Data, "id");
            char *id = cJSON_GetStringValue(id_js);
            /* 给服务器应答 */
            Onenet_Property_Ack(PRODUCT_ID, PRODUCT_DEVICE_NAME, id, 200, "success");
            /* 释放内存 */
            cJSON_Delete(Data);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id");
        break;
    }
}

esp_err_t Onenet_Start(void)
{
    /* 填充初始化结构体 */
    esp_mqtt_client_config_t mqtt_cfg;
    memset(&mqtt_cfg, 0, sizeof(esp_mqtt_client_config_t));
    mqtt_cfg.broker.address.uri = "mqtt://mqtts.heclouds.com";
    mqtt_cfg.broker.address.port = 1883;

    mqtt_cfg.credentials.client_id = PRODUCT_DEVICE_NAME;
    mqtt_cfg.credentials.username = PRODUCT_ID;

    static char Token_Buf[256];

    memset(Token_Buf, 0, 200);
    dev_token_generate(Token_Buf, SIG_METHOD_SHA256, 1915716180, PRODUCT_ID, PRODUCT_DEVICE_NAME, PRODUCT_ACCESS_KEY);
    if (Token_Buf[200 - 1] != 0)
    {
        ESP_LOGE(TAG, "Token数组超限,Token获取失败\r\n");
        return ESP_FAIL;
    }
    mqtt_cfg.credentials.authentication.password = Token_Buf;
    /* 初始化MQTT */
    Onenet_handle = esp_mqtt_client_init(&mqtt_cfg);
    /* 注册事件回调函数 */
    esp_mqtt_client_register_event(Onenet_handle, MQTT_EVENT_ANY, MQTT_Event_cb, NULL);
    return esp_mqtt_client_start(Onenet_handle);
}