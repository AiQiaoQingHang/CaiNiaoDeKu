#include "Ws_Server.h"
#include "esp_log.h"
#include "esp_http_server.h"

static const char *TAG = "Ws_Server";

static const char *html_code = NULL;       // 网页
static Ws_Rece_cb Ws_Rece_CallBack = NULL; // WS接收回调函数,主要是把数据放到自己的内存空间
static int Ws_Sockfd = -1;                 // 客户端Sockfd
httpd_handle_t Server_Handle = NULL;       // 服务器句柄

/**
 * @note http GET请求回调
 * @return esp_err_t 函数执行结果
 */
static esp_err_t
http_GET_CallBack(httpd_req_t *r)
{
    esp_err_t ERR = ESP_OK;
    ERR = httpd_resp_send(r, html_code, HTTPD_RESP_USE_STRLEN); // http的GET请求触发后直接发回网页内容
    if (ERR != ESP_OK)
    {
        ESP_LOGE(TAG, "http GET请求发送服务器页面数据错误\r\n");
    }
    return ERR;
}

/**
 * @note Ws GET请求回调
 * @return esp_err_t 函数执行结果
 */
static esp_err_t Ws_GET_CallBack(httpd_req_t *r)
{
    /* 处理第一次升级请求 */
    if (r->method == HTTP_GET)
    {
        Ws_Sockfd = httpd_req_to_sockfd(r);
        ESP_LOGI(TAG, "升级Ws\r\n");
        return ESP_OK; // 第一次没有payload直接返回
    }
    /* 接收数据 */
    httpd_ws_frame_t Packet;
    esp_err_t ERR = ESP_OK;
    ERR = httpd_ws_recv_frame(r, &Packet, 0); // 主要用于填充长度信息
    if (ERR != ESP_OK)
    {
        ESP_LOGE(TAG, "Ws GET请求获取长度信息错误\r\n");
        return ESP_FAIL;
    }
    uint8_t *buf = (uint8_t *)malloc(Packet.len + 1); // 根据获取长度信息申请内存，加 1 为了字符串结尾/0
    if (buf == NULL)
    {
        ESP_LOGE(TAG, "Ws GET请求申请内存失败\r\n");
        return ESP_FAIL;
    }
    Packet.payload = buf;                              // 将payload指针指向申请的内存空间
    ERR = httpd_ws_recv_frame(r, &Packet, Packet.len); // 获取payload
    if (ERR != ESP_OK)
    {
        ESP_LOGE(TAG, "Ws GET请求获取payload错误\r\n");
        return ESP_FAIL;
    }
    if (Packet.type != HTTPD_WS_TYPE_TEXT) // 判断是不是TEXT类型
    {
        ESP_LOGE(TAG, "Ws GET请求数据类型错误\r\n");
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGI(TAG, "Ws GET请求成功接收到数据\r\n");
        if (Ws_Rece_CallBack)
        {
            Ws_Rece_CallBack(Packet.payload, Packet.len); // 把数据放到自己的内存空间
        }
    }
    /* 释放内存 */
    free(buf);
    buf = NULL;
    return ESP_OK;
}

/**
 * @brief 启动服务器
 *
 * @param html 网页代码
 * @param Rece_CallBack 接收函数回调
 * @return esp_err_t 函数执行结果
 */
esp_err_t Web_Ws_Start(const char *html, Ws_Rece_cb Rece_CallBack)
{
    if ((html_code == NULL) || (Rece_CallBack == NULL))
    {
        ESP_LOGE(TAG, "Ws_Start启动参数无效,启动失败\r\n");
        return ESP_FAIL;
    }
    esp_err_t ERR = ESP_OK;
    /* 保存网页和回调函数 */
    html_code = html;
    Ws_Rece_CallBack = Rece_CallBack;
    /* 配置并启动服务器 */
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG(); // 服务器配置默认
    ERR = httpd_start(&Server_Handle, &cfg);     // 启动服务器，保存句柄
    /* 配置相关请求与目录 */
    /* HTTP */
    httpd_uri_t HTTP_GET_URL =
        {
            .uri = "/",                   // 表示 【根目录】
            .handler = http_GET_CallBack, // 回调函数，当使用【GET请求】访问 【根目录】 会调用此函数
            .method = HTTP_GET,           // HTTP_GET请求
            .is_websocket = false,        // 非Ws
        };
    ERR = httpd_register_uri_handler(&Server_Handle, &HTTP_GET_URL); // 注册到服务器
    /* Ws */
    httpd_uri_t Ws_GET_URL =
        {
            .uri = "/Ws",               // 表示 【根目录 / Ws】
            .handler = Ws_GET_CallBack, // 回调函数，当使用【GET请求】访问 【根目录 / Ws】 会调用此函数
            .method = HTTP_GET,         // HTTP_GET请求
            .is_websocket = true,       // Ws
        };
    ERR = httpd_register_uri_handler(&Server_Handle, &Ws_GET_URL); // 注册到服务器
    return ERR;
}

/**
 * @brief 关闭服务器
 *
 */
void Web_Ws_Stop(void)
{
    if (Server_Handle)
    {
        httpd_stop(Server_Handle);
        Server_Handle = NULL;
        ESP_LOGI(TAG, "服务器关闭\r\n");
    }
}

/**
 * @brief Ws发送数据
 *
 * @param Data 待发送的数据
 * @param Len 数据长度
 * @return esp_err_t 函数运行结果
 */
esp_err_t Web_Ws_Send(uint8_t *Data, uint16_t Len)
{
    httpd_ws_frame_t Send =
        {
            .payload = Data,
            .len = Len,
            .type = HTTPD_WS_TYPE_TEXT,
        };
    return httpd_ws_send_data(Server_Handle, Ws_Sockfd, &Send);
}