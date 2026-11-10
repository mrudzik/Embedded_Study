//ds1307.c

#include "ds1307.h"
#include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"


#define DS1307_REG_SEC   0x00
#define DS1307_CH_BIT    (1 << 7)



static const char *TAG = "ds1307";

static i2c_master_dev_handle_t s_rtc;

static inline uint8_t bcd2dec(uint8_t v) { return (v >> 4) * 10 + (v & 0x0F); }
static inline uint8_t dec2bcd(uint8_t v) { return ((v / 10) << 4) | (v % 10); }

esp_err_t ds1307_init(i2c_master_bus_handle_t bus)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = DS1307_ADDR,
        .scl_speed_hz    = 100000,
    };
    esp_err_t err = i2c_master_bus_add_device(bus, &dev_cfg, &s_rtc);
    if (err != ESP_OK)
        return err;

    err = i2c_master_probe(bus, DS1307_ADDR, 100);
    if (err == ESP_OK)
        ESP_LOGI(TAG, "RTC знайдено на 0x%02X", DS1307_ADDR);

    // ds1307_start_clock(s_rtc);

    return err;
}

esp_err_t ds1307_set_time(const ds1307_time_t *time)
{
    uint8_t data[8];

    data[0] = 0x00; 

    data[1] = dec2bcd(time->sec) & 0x7F;
    data[2] = dec2bcd(time->min);
    data[3] = dec2bcd(time->hour) & 0x3F; 

    data[4] = dec2bcd(time->dow);
    data[5] = dec2bcd(time->day);
    data[6] = dec2bcd(time->month);
    data[7] = dec2bcd(time->year - 2000);

    return i2c_master_transmit(s_rtc, data, sizeof(data), 1000);
}

esp_err_t ds1307_get_time(ds1307_time_t *time)
{
    uint8_t reg = 0x00;
    uint8_t data[7];

    esp_err_t err = i2c_master_transmit_receive(s_rtc, &reg, 1, data, sizeof(data), 1000);
    if (err != ESP_OK)
        return err;

    time->sec   = bcd2dec(data[0] & 0x7F);
    time->min   = bcd2dec(data[1] & 0x7F);
    time->hour  = bcd2dec(data[2] & 0x3F);
    time->dow   = bcd2dec(data[3] & 0x07);
    time->day   = bcd2dec(data[4] & 0x3F);
    time->month = bcd2dec(data[5] & 0x1F);
    time->year  = bcd2dec(data[6]) + 2000;

    return ESP_OK;
}

esp_err_t ds1307_is_running(bool *running)
{
    uint8_t reg = 0x00;
    uint8_t sec;

    esp_err_t err = i2c_master_transmit_receive(s_rtc, &reg, 1, &sec, 1, 1000);
    if (err != ESP_OK)
        return err;

    *running = (sec & 0x80) == 0;
    return ESP_OK;
}


// esp_err_t ds1307_start_clock(i2c_master_dev_handle_t dev_handle) 
// {
//     uint8_t reg_addr = DS1307_REG_SEC;
//     uint8_t seconds_reg_val = 0;
//     esp_err_t err;

//     // 1. Зчитуємо поточне значення регістру секунд (0x00)
//     err = i2c_master_transmit_receive(dev_handle, &reg_addr, 1, &seconds_reg_val, 1, -1);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG, "Помилка читання регістру секунд: %s", esp_err_to_name(err));
//         return err;
//     }

//     // Перевіряємо, чи біт CH взагалі встановлений (дорівнює 1)
//     if (seconds_reg_val & DS1307_CH_BIT) {
//         ESP_LOGI(TAG, "Генератор зупинено (CH=1). Запускаємо...");

//         // 2. Модифікуємо значення: скидаємо 7-й біт в 0
//         seconds_reg_val &= ~DS1307_CH_BIT;

//         // 3. Записуємо оновлене значення назад. 
//         // Потрібно надіслати спочатку адресу регістру (0x00), а потім саме значення.
//         uint8_t write_buf[2] = { DS1307_REG_SEC, seconds_reg_val };
        
//         err = i2c_master_transmit(dev_handle, write_buf, 2, -1);
//         if (err != ESP_OK) {
//             ESP_LOGE(TAG, "Помилка запису в регістр секунд: %s", esp_err_to_name(err));
//             return err;
//         }
//         ESP_LOGI(TAG, "Генератор успішно запущено (CH=0)!");
//     } else {
//         ESP_LOGI(TAG, "Генератор вже працює (CH=0). Скидання не потрібне.");
//     }


//     // Зупиняємо
//     uint8_t stop_buf[] = { 0x00, 0x80 }; // CH = 1
//     i2c_master_transmit(dev_handle, stop_buf, 2, -1);
//     vTaskDelay(pdMS_TO_TICKS(100));

//     // Запускаємо
//     uint8_t start_buf[] = { 0x00, 0x00 }; // CH = 0, Sec = 0
//     i2c_master_transmit(dev_handle, start_buf, 2, -1);

//     return ESP_OK;
// }