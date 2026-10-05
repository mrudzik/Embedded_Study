// main.c

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

#include "ssd1306.h"
#include "ds1307.h"

#define PIN_SDA    8
#define PIN_SCL    9
#define OLED_ADDR  0x3C

#define TEXT_SCALE 4
#define TEXT_X     2
#define TEXT_Y     ((OLED_H - 5 * TEXT_SCALE) / 2) 

static const char *TAG = "clock";

static const ds1307_time_t new_time = {
    .sec   = 0,
    .min   = 30,
    .hour  = 12,
    .dow   = 1,
    .day   = 17,
    .month = 8,
    .year  = 2026,
};

static i2c_master_bus_handle_t i2c_bus_init(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port                     = I2C_NUM_0,
        .sda_io_num                   = PIN_SDA,
        .scl_io_num                   = PIN_SCL,
        .clk_source                   = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt            = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus));
    return bus;
}

static void show_text(const char *str)
{
    fb_clear();
    fb_text(TEXT_X, TEXT_Y, str, TEXT_SCALE);
    oled_flush();
}

void app_main(void)
{
    i2c_master_bus_handle_t bus = i2c_bus_init();

    oled_init(bus, OLED_ADDR);      
    ESP_ERROR_CHECK(ds1307_init(bus));

    bool running;
    ESP_ERROR_CHECK(ds1307_is_running(&running));
    if (!running) {
        ESP_ERROR_CHECK(ds1307_set_time(&new_time));
        ESP_LOGI(TAG, "Годинник стояв — час встановлено");
    }

    ds1307_time_t t;
    int  last_sec = -1;
    char buf[12];

    while (1)
    {
        if (ds1307_get_time(&t) == ESP_OK)
        {
            if (t.sec != last_sec)
            {
                last_sec = t.sec;

                snprintf(buf, sizeof(buf), "%02d:%02d:%02d", t.hour, t.min, t.sec);
                show_text(buf);

                ESP_LOGI(TAG, "%s", buf);
            }
        }
        else
        {
            ESP_LOGE(TAG, "Помилка читання годинника");
            show_text("--:--:--");
            last_sec = -1;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}