#include "Onenet_DM.h"
#include "cJSON.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include "mqtt_client.h"
#include "MQTT.h"
#include "driver/gpio.h"

const char *TAG = "OnenetErr";

static int Brightness = 0;       // 明亮度【0 ~ 100%】
static bool LightSwitch = false; // 开关

/**
 * @brief 服务器下发【属性】数据处理
 *
 * @param Property 服务器下发的cjson文件
 */
void Onenet_Property_ProcHandle(cJSON *Property)
{
    /*
    {
        "id" : "123",
        "version" : "1.0",
        "params" : {
                    "Brightness" : 20%
                    "LightSwitch" : false
                    }
    }
    */
    if (!Property)
    {
        ESP_LOGE(TAG, "Onenet_Property_ProcHandle Inval\r\n");
        return;
    }
    cJSON *params = cJSON_GetObjectItem(Property, "params"); // 获取【params】下的内容
    if (params)
    {
        cJSON *params_name = params->child; // 指向【params】下第一个键名
        while (params_name)
        {
            if (!strcmp(params_name->string, "Brightness")) // 字符串相同返回 0
            {
                Brightness = cJSON_GetNumberValue(params_name);
            }
            if (!strcmp(params_name->string, "LightSwitch")) // 字符串相同返回 0
            {
                if (params_name->type == cJSON_True)
                {
                    LightSwitch = true;
                    gpio_set_level(GPIO_NUM_4, 1);
                }
                else
                {
                    LightSwitch = false;
                    gpio_set_level(GPIO_NUM_4, 0);
                }
            }
            params_name = params_name->next; // 指向下一个键值
        }
    }
}

/**
 * @brief 【属性】上行数据打包
 *
 * @return cJSON* 数据处理完毕后的cjson
 * @note 注意使用完需要释放内存
 */
cJSON *Onenet_Property_UpData(void)
{
    /*
                    {
                  "id": "123",
                  "version": "1.0",
                  "params": {
                    "Brightness": {
                      "value": 50,
                      "time": 1524448722123
                    },
                    "LightSwitch": {
                      "value": true,
                      "time": 1524448722123
                    }
                  }
                }
    */
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "id", "111");
    cJSON_AddStringToObject(root, "version", "1.0");
    cJSON *params_js = cJSON_AddObjectToObject(root, "params");
    /* 亮度 */
    cJSON *Brightness_js = cJSON_AddObjectToObject(params_js, "Brightness");
    cJSON_AddNumberToObject(Brightness_js, "value", Brightness);
    /* 开关 */
    cJSON *LightSwitch_js = cJSON_AddObjectToObject(params_js, "LightSwitch");
    if (LightSwitch == true)
        cJSON_AddBoolToObject(LightSwitch_js, "value", cJSON_True);
    else
        cJSON_AddBoolToObject(LightSwitch_js, "value", cJSON_False);
    return root; // cjson是malloc的堆上内存
}

/**
 * @brief 回复ACK
 *
 * @param Product_ID 设备ID
 * @param Product_Device_Name 设备名字
 * @param id 服务器下发指令时数据包里的ID
 * @param code 错误码
 * @param msg 消息
 */
void Onenet_Property_Ack(const char *Product_ID, const char *Product_Device_Name, const char *id, int code, const char *msg)
{
    char Topic_Buf[200] = {0};
    snprintf(Topic_Buf, 256, "$sys/%s/%s/thing/property/set_reply", Product_ID, Product_Device_Name);
    /*    {
    "id":"123",
    "code":200,
    "msg":"xxxx"
    }
    */
    cJSON *Ack_js = cJSON_CreateObject();
    cJSON_AddStringToObject(Ack_js, "id", id);
    cJSON_AddNumberToObject(Ack_js, "code", code);
    cJSON_AddStringToObject(Ack_js, "msg", msg);
    char *Data = cJSON_PrintUnformatted(Ack_js); // 转换成字符串，节省传输字节
    esp_mqtt_client_publish(Onenet_handle, Topic_Buf, Data, strlen(Data), 1, 0);
    /* 释放内存 */
    cJSON_free(Data);
    cJSON_Delete(Ack_js);
}

/**
 * @brief 订阅主题
 *
 * @param Product_ID 设备ID
 * @param Product_Device_Name 设备名称
 */
void Onenet_SUBSCRIBE_Topic(const char *Product_ID, const char *Product_Device_Name)
{
    char SUBSCRIBE_Buf[200] = {0};
    /* 订阅设备属性上报主题 */
    snprintf(SUBSCRIBE_Buf, 200, "$sys/%s/%s/thing/property/post", Product_ID, Product_Device_Name);
    esp_mqtt_client_subscribe_single(Onenet_handle, SUBSCRIBE_Buf, 1);
    /* 订阅设备属性设置主题 */
    snprintf(SUBSCRIBE_Buf, 200, "$sys/%s/%s/thing/property/set", Product_ID, Product_Device_Name);
    esp_mqtt_client_subscribe_single(Onenet_handle, SUBSCRIBE_Buf, 1);
}

/**
 * @brief 上行
 *
 * @param Product_ID 设备ID
 * @param Product_Device_Name 设备名称
 * @param Data 待发送数据
 * @note Data需要是cjson的字符串
 */
void Onenet_Property_Push(const char *Product_ID, const char *Product_Device_Name, char *Data)
{
    char SUBSCRIBE_Buf[200] = {0};
    /* 订阅设备属性上报主题 */
    snprintf(SUBSCRIBE_Buf, 200, "$sys/%s/%s/thing/property/post", Product_ID, Product_Device_Name);
    esp_mqtt_client_publish(Onenet_handle, SUBSCRIBE_Buf, Data, strlen(Data), 1, 0);
}