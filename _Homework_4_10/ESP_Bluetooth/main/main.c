#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"

static const char *TAG = "BLE_SCAN";

// Налаштування сканування
static esp_ble_scan_params_t scan_params = {
    .scan_type = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval = 0x50,
    .scan_window = 0x30,
    .scan_duplicate = BLE_SCAN_DUPLICATE_ENABLE
};

// Callback для подій Bluetooth
static void gap_callback(esp_gap_ble_cb_event_t event,
                         esp_ble_gap_cb_param_t *param)
{
    if (event == ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT) {
        if (param->scan_param_cmpl.status == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "Scanning for 10 seconds...");
            ESP_ERROR_CHECK(esp_ble_gap_start_scanning(10));
        }
    }

    if (event == ESP_GAP_BLE_SCAN_RESULT_EVT) {

        if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT) {
            ESP_LOGI(TAG, "Scan completed");
            return;
        }

        if (param->scan_rst.search_evt != ESP_GAP_SEARCH_INQ_RES_EVT) {
            return;
        }

        // Отримуємо назву пристрою з Advertising
        uint8_t name_len = 0;
        uint8_t *name = esp_ble_resolve_adv_data_by_type(
            param->scan_rst.ble_adv,
            param->scan_rst.adv_data_len +
                param->scan_rst.scan_rsp_len,
            ESP_BLE_AD_TYPE_NAME_CMPL,
            &name_len
        );

        if (!name) {
            name = esp_ble_resolve_adv_data_by_type(
                param->scan_rst.ble_adv,
                param->scan_rst.adv_data_len +
                    param->scan_rst.scan_rsp_len,
                ESP_BLE_AD_TYPE_NAME_SHORT,
                &name_len
            );
        }

        ESP_LOGI(TAG,
            ESP_BD_ADDR_STR " | RSSI: %d dBm | Name: %.*s",
            ESP_BD_ADDR_HEX(param->scan_rst.bda),
            param->scan_rst.rssi,
            name ? name_len : 0,
            name ? (char *)name : ""
        );
    }
}

void app_main(void)
{
    // Ініціалізація NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    // Ініціалізація Bluetooth Controller
    esp_bt_controller_config_t bt_cfg =
        BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    // Ініціалізація Bluedroid
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    // Реєструємо callback
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_callback));

    // Починаємо налаштування сканування
    ESP_ERROR_CHECK(esp_ble_gap_set_scan_params(&scan_params));
}

