#include <stdio.h>

#include "esp_bt.h"          //蓝牙控制器和VHCI设置头文件
#include "esp_gap_ble_api.h" //GAP设置头文件，广播和连接相关参数配置
#include "esp_gatts_api.h"   //GATT配置头文件，创建Service和Characteristic
#include "esp_bt_main.h"     //蓝牙栈空间的初始化头文件
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

/*初始化蓝牙 》》 注册GATT和GAP事件回调函数 》》 注册Profile（GATT），此时产生ESP_GATTS_REG_EVT事件回调，在回调里创建服务，此时产生ESP_GATTS_CREATE_EVT事件回调，在回调里添加特征值之后启用服务（注意创建服务和特征值时需要保存句柄）
 *》》 配置广播数据包，此时产生ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT事件回调，在回调里配置广播行为后启动广播，此时就产生了蓝牙信号
 *
 *创建Profile，Service，character都会在事件回调内返回唯一句柄，之后读写操作可以直接根据事件回调带入参数的句柄与保存的句柄进行比较就可以得到实际需要操作的参数
 *gatt_if是区分Profile的，Service和character都由句柄区分
 *Profile -> Service -> character，这是大小关系，Profile下可以挂载多个Service，Service下可以挂载多个character
 * */

#define Service_UUID 0x181A // 温湿度服务UUID

#define Char_Temp_UUID 0x2A6E // 温度特征
#define Char_Humi_UUID 0x2A6F // 湿度特征

static char *TAG = "BLE";
static uint16_t Service_Handle;   // 服务句柄
static uint16_t Char_Temp_Handle; // 温度特征值句柄
static uint16_t Char_Humi_Handle; // 湿度特征值句柄

static volatile float Temp_Value = 0.0f; // 温度值
static volatile float Humi_Value = 0.0f; // 湿度值
static uint8_t Temp_Value_buf[4] = {0};  // 温度特征值缓存区
static uint8_t Humi_Value_buf[4] = {0};  // 湿度度特征值缓存区

#define PROFILE_APP_ID 0                                                                                            // Profile -> Server -> charater
static void gap_ble_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);                          // gap广播事件回调函数
static void gatt_ble_callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param); // gatt服务器事件回调函数
static void Add_Char(void);                                                                                         // 添加特征值
static void Start_Adv(void);                                                                                        // 开始广播

void app_main(void)
{
    esp_err_t ret = ESP_OK;
    nvs_flash_init();
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT(); // 使用默认参数初始化蓝牙控制器
    /* 蓝牙控制器初始化 */
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) // 初始化蓝牙控制器
    {
        ESP_LOGE(TAG, "控制器初始化失败");
        return;
    }
    else
        ESP_LOGI(TAG, "控制器初始化成功");
    /* 使能蓝牙控制器 */
    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_BLE)) != ESP_OK)
    {
        ESP_LOGE(TAG, "控制器使能失败");
        return;
    }
    else
        ESP_LOGI(TAG, "控制器使能成功");
    /* 初始化Bluedroid协议栈 */
    if ((ret = esp_bluedroid_init()) != ESP_OK)
    {
        ESP_LOGE(TAG, "Bluedroid初始化失败");
        return;
    }
    else
        ESP_LOGI(TAG, "Bluedroid初始化成功");
    /* 使能Bluedroid协议栈 */
    if ((ret = esp_bluedroid_enable()) != ESP_OK)
    {
        ESP_LOGE(TAG, "Bluedroid使能失败");
        return;
    }
    else
        ESP_LOGI(TAG, "Bluedroid使能成功");
    /* 注册BLE GAP回调（处理广播、扫描响应等） */
    if ((ret = esp_ble_gap_register_callback(gap_ble_callback)) != ESP_OK)
    {
        ESP_LOGE(TAG, "BLE GAP回调注册失败");
        return;
    }
    else
        ESP_LOGI(TAG, "BLE GAP回调注册成功");
    /* 注册 BLE GATT服务器回调（处理服务创建、特征值读写等）*/
    if ((ret = esp_ble_gatts_register_callback(gatt_ble_callback)) != ESP_OK)
    {
        ESP_LOGE(TAG, "BLE GATT回调注册失败");
        return;
    }
    else
        ESP_LOGI(TAG, "BLE GATT回调注册成功");
    /* 注册Profile */
    esp_ble_gatts_app_register(PROFILE_APP_ID);
    /* 广播数据包配置 */
    if ((ret = esp_ble_gap_set_device_name("ESP")) != ESP_OK)
    {
        ESP_LOGE(TAG, "NAME设置失败");
        return;
    }
    else
        ESP_LOGI(TAG, "NAME设置成功");
    // 配置广播数据（包含设备名称、服务UUID等）
    esp_ble_adv_data_t adv_data = {
        .set_scan_rsp = false,
        .include_name = true,
        .include_txpower = false,
        .max_interval = 0xFFFF,
        .min_interval = 0xFFFF,
        .appearance = 0,
        .manufacturer_len = 0,
        .p_manufacturer_data = NULL,
        .service_data_len = 0,
        .p_service_data = NULL,
        .service_uuid_len = 0,
        .p_service_uuid = NULL,
        .flag = ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT,
    };
    ret = esp_ble_gap_config_adv_data(&adv_data);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "广播数据配置失败: %s", esp_err_to_name(ret)); // 打印具体错误码
    }
    else
    {
        ESP_LOGI(TAG, "广播数据配置请求已发送");
    }
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

/**
 * @brief GATT事件回调
 *
 */
void gatt_ble_callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_err_t ret = ESP_OK;
    switch (event)
    {
    /* GATT注册成功事件 */
    case ESP_GATTS_REG_EVT:
        esp_gatt_srvc_id_t gatts_srvc = {
            .id = {
                .uuid = {
                    .len = ESP_UUID_LEN_16,
                    .uuid.uuid16 = Service_UUID,
                },
                .inst_id = 0,
            },
            .is_primary = true,
        };
        ret = esp_ble_gatts_create_service(gatts_if, &gatts_srvc, 10); // 这里gatts_if是区分Profile的，每次事件都会携带，Profile -> Server -> charater
        if (ret != ESP_OK)
        {
            ESP_LOGW(TAG, "服务器创建失败%s", esp_err_to_name(ret));
        }
        else
            ESP_LOGI(TAG, "服务器创建成功");
        break;
    /* 服务创建成功事件 */
    case ESP_GATTS_CREATE_EVT:
        Service_Handle = param->create.service_handle; // 需要保存服务句柄，只在服务创建成功时携带，后续操作需要使用
        /* 添加特征值 */
        Add_Char();
        /* 开启服务 */
        ret = esp_ble_gatts_start_service(Service_Handle);
        if (ret != ESP_OK)
        {
            ESP_LOGW(TAG, "温湿度服务启动失败：%s", esp_err_to_name(ret));
        }
        else
        {
            ESP_LOGI(TAG, "温湿度服务启动成功（已激活）");
        }
        break;
    /* 添加特征值事件 */
    case ESP_GATTS_ADD_CHAR_EVT:
        uint16_t char_uuid = param->add_char.char_uuid.uuid.uuid16; // 获取添加特征值的UUID
        if (char_uuid == Char_Temp_UUID)
        {
            Char_Temp_Handle = param->add_char.attr_handle; // 保存句柄
        }
        else if (char_uuid == Char_Humi_UUID)
        {
            Char_Humi_Handle = param->add_char.attr_handle;
        }
        break;
    /* 客户端读特征值事件 */
    case ESP_GATTS_READ_EVT:
        esp_gatt_rsp_t rsp = {0};
        rsp.attr_value.handle = param->read.handle;
        // 处理温度特征读取
        if (param->read.handle == Char_Temp_Handle)
        {
            memcpy(Temp_Value_buf, &Temp_Value, 4); // 获取最新值
            rsp.attr_value.offset = 0;
            rsp.attr_value.len = 4;
            memcpy(rsp.attr_value.value, Temp_Value_buf, 4);
        }
        // 处理湿度特征读取
        else if (param->read.handle == Char_Humi_Handle)
        {
            memcpy(Humi_Value_buf, &Humi_Value, 4);
            rsp.attr_value.offset = 0;
            rsp.attr_value.len = 4;
            memcpy(rsp.attr_value.value, Humi_Value_buf, 4); // 获取最新值
        }
        // 向客户端发送响应（返回数据）
        esp_ble_gatts_send_response(
            gatts_if,
            param->read.conn_id,  // 当前连接ID
            param->read.trans_id, // 事务ID（匹配请求）
            ESP_GATT_OK,          // 成功状态
            &rsp                  // 响应数据
        );
        break;
    /* 客户端写入特征值事件 */
    case ESP_GATTS_WRITE_EVT:
        // 处理"准备写入"（长数据）
        if (param->write.is_prep)
        {
            ESP_LOGI(TAG, "收到准备写入请求，暂不处理");
            break;
        }

        // 提取写入的句柄、数据和长度
        uint16_t write_handle = param->write.handle;
        uint8_t *write_data = param->write.value;
        uint16_t write_len = param->write.len;

        // 匹配温度特征句柄，处理温度写入
        if (write_handle == Char_Temp_Handle)
        {
            // 检查数据长度是否为4字节
            if (write_len != 4)
            {
                ESP_LOGE(TAG, "温度写入数据长度错误(需4字节,实际%d字节)", write_len);
                // 发送错误响应
                if (param->write.need_rsp)
                {
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                                param->write.trans_id, ESP_GATT_INVALID_ATTR_LEN, NULL);
                }
                break;
            }
            memcpy(&Temp_Value, write_data, 4);
            ESP_LOGI(TAG, "客户端写入温度：%f", Temp_Value);
        }
        // 匹配湿度特征句柄，处理湿度写入
        else if (write_handle == Char_Humi_Handle)
        {
            if (write_len != 4)
            {
                ESP_LOGE(TAG, "湿度写入数据长度错误(需4字节,实际%d字节)", write_len);
                if (param->write.need_rsp)
                {
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                                param->write.trans_id, ESP_GATT_INVALID_ATTR_LEN, NULL);
                }
                break;
            }
            memcpy(&Humi_Value, write_data, 4);
            ESP_LOGI(TAG, "客户端写入湿度：%.2f", Humi_Value);
        }
        if (param->write.need_rsp)
        {
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                        param->write.trans_id, ESP_GATT_OK, NULL);
            ESP_LOGI(TAG, "已发送写入响应");
        }
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(TAG, "设备断开连接，重新广播");
        Start_Adv();
        break;
    case ESP_GATTS_CONNECT_EVT:
        ESP_LOGI(TAG, "有设备连接");
    default:
        break;
    }
}

/**
 * @brief GAP事件回调
 *
 */
void gap_ble_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
        /* 广播数据配置完成事件 */
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        ESP_LOGI(TAG, "广播数据配置完成，开始广播");
        Start_Adv();
        break;

    /* 广播启动完成事件（可选，确认广播已启动） */
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "广播启动失败");
        }
        else
        {
            ESP_LOGI(TAG, "广播启动成功");
        }
        break;
    default:
        break;
    }
}

/**
 * @brief 添加特征值
 *
 */
void Add_Char(void)
{
    esp_err_t ret = ESP_OK;
    // 添加温度特征
    esp_bt_uuid_t Temp_uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid.uuid16 = Char_Temp_UUID,
    };
    memcpy(Temp_Value_buf, &Temp_Value, 4); // float类型四字节，直接把内存拷贝过去，蓝牙协议要求float类型，但是ESP只有uint8_t类型处理
    ret = esp_ble_gatts_add_char(Service_Handle,
                                 &Temp_uuid,
                                 ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                 ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE,
                                 &(esp_attr_value_t){
                                     .attr_max_len = 4,
                                     .attr_len = 4,
                                     .attr_value = Temp_Value_buf},
                                 NULL);
    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "温度特征值添加失败%s", esp_err_to_name(ret));
    }
    else
        ESP_LOGI(TAG, "温度特征值添加成功");
    // 添加湿度特征
    ret = ESP_OK;
    esp_bt_uuid_t Humi_uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid.uuid16 = Char_Humi_UUID,
    };
    memcpy(Humi_Value_buf, &Humi_Value, 4);
    ret = esp_ble_gatts_add_char(Service_Handle,
                                 &Humi_uuid,
                                 ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                 ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE,
                                 &(esp_attr_value_t){
                                     .attr_max_len = 4,
                                     .attr_len = 4,
                                     .attr_value = Humi_Value_buf},
                                 NULL);
    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "湿度度特征值添加失败%s", esp_err_to_name(ret));
    }
    else
        ESP_LOGI(TAG, "湿度特征值添加成功");
}

/**
 * @brief 开始广播
 *
 */
static void Start_Adv(void)
{
    // 配置广播行为结构体
    esp_ble_adv_params_t adv_params = {
        .adv_int_min = 0x00A0,
        .adv_int_max = 0x0100,
        .adv_type = ADV_TYPE_IND,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .channel_map = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    // 开始广播
    esp_ble_gap_start_advertising(&adv_params);
}