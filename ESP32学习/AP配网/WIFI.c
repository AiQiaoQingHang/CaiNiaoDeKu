#include "WIFI.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include <string.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#define ReTry_Num 5 // 最大重连次数

esp_event_handler_instance_t ANY_ID_Handle; // 回调注册句柄
esp_event_handler_instance_t GOT_IP_Handle; // 回调注册句柄
static SemaphoreHandle_t Scan_Flag;         // 扫描完成标志
static const char *TAG = "WIFI";

static void Scan_Task(void *Scan_List_Size);                                                           // 扫描的freertos任务
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data); // WIFI事件回调函数

/**
 * @brief WIFI初始化
 *
 */
esp_err_t WIFI_Init()
{
    esp_err_t ERR = ESP_OK;
    ERR = nvs_flash_init();
    ERR = esp_netif_init();
    ERR = esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ERR = esp_wifi_init(&cfg);
    ERR = esp_event_handler_instance_register(WIFI_EVENT,
                                              ESP_EVENT_ANY_ID,
                                              &event_handler,
                                              NULL,
                                              &ANY_ID_Handle);
    ERR = esp_event_handler_instance_register(IP_EVENT,
                                              IP_EVENT_STA_GOT_IP,
                                              &event_handler,
                                              NULL,
                                              &GOT_IP_Handle);
    Scan_Flag = xSemaphoreCreateBinary(); // 创建二值信号量，用于扫描检测
    xSemaphoreGive(Scan_Flag);            // 初始化为 1
    return ERR;
}

/**
 * @brief WIFI进入STA模式
 *
 */
esp_err_t WIFI_Enter_STA()
{
    esp_err_t ERR = ESP_OK;
    wifi_mode_t Mode;
    ERR = esp_wifi_get_mode(&Mode);
    if (Mode != WIFI_MODE_STA)
    {
        ERR = esp_wifi_stop();
        ERR = esp_wifi_set_mode(WIFI_MODE_STA);
    }
    ERR = esp_wifi_start();
    if (ERR != ESP_OK)
    {
        ESP_LOGW(TAG, "进入STA模式失败\r\n");
    }
    return ERR;
}

/**
 * @brief WIFI进入AP模式
 *
 * @param channel AP信道
 * @param max_connection 最大连接数
 * @param ssid WIFI名称
 * @param password WIFI密码
 */
esp_err_t WIFI_Enter_AP(uint8_t channel, uint8_t max_connection, const char *ssid, const char *password)
{
    esp_err_t ERR = ESP_OK;
    ERR = esp_wifi_stop(); // 停止WIFI
    wifi_mode_t Mode;
    ERR = esp_wifi_get_mode(&Mode);
    if (Mode != WIFI_MODE_AP)
    {
        ERR = esp_wifi_set_mode(WIFI_MODE_AP);
    }
    wifi_config_t wifi_config = {
        // 配置AP模式结构体
        .ap = {
            .channel = channel,
            .max_connection = max_connection,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_len = strlen(ssid),
        },
    };
    snprintf((char *)wifi_config.ap.ssid, sizeof(wifi_config.ap.ssid), "%s", ssid);
    snprintf((char *)wifi_config.ap.password, sizeof(wifi_config.ap.password), "%s", password);
    ERR = esp_wifi_set_config(WIFI_IF_AP, &wifi_config); // 应用配置
    ERR = esp_wifi_start();                              // 重新启动WIFI
    if (ERR != ESP_OK)
    {
        ESP_LOGW(TAG, "进入AP模式失败\r\n");
    }
    return ERR;
}

/**
 * @brief 连接指定WIFI
 *
 * @param ssid WIFI名称
 * @param password WIFI密码
 */
esp_err_t WIFI_Connect(const char *ssid, const char *password)
{
    esp_err_t ERR = ESP_OK;
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    snprintf((char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", ssid);
    snprintf((char *)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", password);
    wifi_mode_t Mode;
    ERR = esp_wifi_get_mode(&Mode);
    if (Mode != WIFI_MODE_STA) // 检查是否处于STA模式
    {
        ERR = esp_wifi_stop();
        ERR = esp_wifi_set_mode(WIFI_MODE_STA);
    }
    ERR = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    ERR = esp_wifi_start();
    return ERR;
}

/**
 * @brief 启动WIFI扫描
 *
 * @param Scan_List_Size 扫描列表长度
 * @param Task_Priority 任务优先级
 * @param Task_Core_ID 指定核心
 * @note 该函数通过创建任务的方式进行扫描，注意要在WIFI确定启动后调用
 */
esp_err_t WIFI_Scan(uint16_t Scan_List_Size, uint8_t Task_Priority, uint8_t Task_Core_ID)
{
    static uint16_t List_Size = 0;
    if (Task_Core_ID > 1)
    {
        ESP_LOGW(TAG, "核心ID错误,启动扫描失败\r\n");
        return ESP_FAIL;
    }
    wifi_mode_t Mode;
    esp_wifi_get_mode(&Mode);                                 // 获取当前模式
    if ((Mode != WIFI_MODE_APSTA) && (Mode != WIFI_MODE_STA)) // 扫描模式需包含STA模式
    {
        ESP_LOGW(TAG, "当前模式不包含STA模式,启动扫描失败\r\n");
        return ESP_FAIL;
    }
    if (xSemaphoreTake(Scan_Flag, 0) == pdTRUE) // 信号量获取成功
    {
        List_Size = Scan_List_Size;
        BaseType_t result = xTaskCreatePinnedToCore(Scan_Task, "WIFI_Scan", 4096, &List_Size, Task_Priority, NULL, Task_Core_ID);
        if (result != pdTRUE) // 任务创建失败
        {
            ESP_LOGW(TAG, "扫描任务创建失败,扫描启动失败\r\n");
            xSemaphoreGive(Scan_Flag);
        }
        return ESP_FAIL;
    }
    else // 信号量获取失败
    {
        ESP_LOGW(TAG, "信号量获取失败,扫描启动失败\r\n");
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
 * @brief 扫描任务
 *
 */
static void Scan_Task(void *Scan_List_Size)
{
    wifi_ap_record_t *Scan_List = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * *(uint16_t *)Scan_List_Size); // 申请内存
    if (Scan_List == NULL)
    {
        ESP_LOGE(TAG, "内存分配失败,扫描任务退出\r\n");
        Scan_List = NULL;          // 显示置为空指针
        xSemaphoreGive(Scan_Flag); // 释放信号量
        vTaskDelete(NULL);         // 删除任务
        return;
    }
    memset(Scan_List, 0, sizeof(wifi_ap_record_t) * *(uint16_t *)Scan_List_Size); // 内存清零
    uint16_t Ap_Num = 0;
    esp_wifi_clear_ap_list();        // 清除上次扫描结果
    esp_wifi_scan_start(NULL, true); // 阻塞模式扫描
    esp_wifi_scan_get_ap_num(&Ap_Num);
    uint16_t Record_Count = (*(uint16_t *)Scan_List_Size > Ap_Num) ? Ap_Num : *(uint16_t *)Scan_List_Size; // 确定大小
    esp_wifi_scan_get_ap_records(&Record_Count, Scan_List);
    /* 打印数据 */
    for (int i = 0; i < Record_Count; i++)
    {
        ESP_LOGI(TAG, "AP%d: SSID=%s, 信号强度=%d dBm\r\n",
                 i + 1,
                 Scan_List[i].ssid,
                 Scan_List[i].rssi);
    }
    /* 释放内存 */
    free(Scan_List);           // 释放内存
    Scan_List = NULL;          // 显式置为空指针
    xSemaphoreGive(Scan_Flag); // 释放信号量
    vTaskDelete(NULL);         // 删除任务
}

/**
 * @brief 事件回调函数
 *
 */
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static uint8_t Retry_Con_NUM = 0; // 重连次数
    /* WIFI事件组 */
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        /* STA启动成功 */
        case WIFI_EVENT_STA_START:
            esp_wifi_connect(); // 连接AP
            ESP_LOGI(TAG, "STA启动成功\r\n");
            ESP_LOGI(TAG, "连接WIFI中...\r\n");
            break;
        /* STA停止成功 */
        case WIFI_EVENT_STA_STOP:
            ESP_LOGI(TAG, "STA停止成功\r\n");
            break;
        /* AP启动成功 */
        case WIFI_EVENT_AP_START:
            ESP_LOGI(TAG, "AP启动成功\r\n");
            break;
        /* AP停止成功 */
        case WIFI_EVENT_AP_STOP:
            ESP_LOGI(TAG, "AP停止成功\r\n");
            break;
        /* WIFI断开连接 */
        case WIFI_EVENT_STA_DISCONNECTED:
            if (Retry_Con_NUM < ReTry_Num)
            {
                Retry_Con_NUM++;
                ESP_LOGI(TAG, "重连次数: %d\r\n", Retry_Con_NUM);
                esp_wifi_connect(); // 重新连接AP
            }
            else
            {
                ESP_LOGI(TAG, "连接失败，停止重连\r\n");
            }
            break;
        /* 连接成功 */
        case WIFI_EVENT_STA_CONNECTED:
            Retry_Con_NUM = 0; // 重连次数清零
            ESP_LOGI(TAG, "连接成功\r\n");
            break;
        /* 设备连接成功 */
        case WIFI_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "AP模式设备连接成功\r\n");
            break;
        /* 设备断开连接 */
        case WIFI_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "AP模式设备断开连接\r\n");
            break;
        default:
            ESP_LOGI(TAG, "WIFI事件未定义子事件\r\n");
            break;
        }
    }
    /* IP事件组 */
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        /* 获取IP成功 */
        case IP_EVENT_STA_GOT_IP:
            Retry_Con_NUM = 0;
            ESP_LOGI(TAG, "获取ip成功\r\n");
            break;
        /* 分配IP成功 */
        case IP_EVENT_AP_STAIPASSIGNED:
            ESP_LOGI(TAG, "给连接设备分配IP成功\r\n");
            break;
        default:
            ESP_LOGI(TAG, "IP事件未定义子事件\r\n");
            break;
        }
    }
    /* 其他事件组 */
    else
    {
        ESP_LOGI(TAG, "未定义事件组\r\n");
    }
}