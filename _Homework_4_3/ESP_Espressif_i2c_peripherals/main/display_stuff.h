#include "esp_err.h"
#include <driver/i2c_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>

#include <ssd1306.h>


static ssd1306_config_t cfg;
static ssd1306_handle_t d;

static void init_display(){

    // Needs to be declared here via constructor (i suppose)
    ssd1306_config_t local_cfg = {
        .width  = 128,
        .height = 64,
        .fb     = NULL, // let driver allocate internally
        .fb_len = 0,
        .iface.i2c =
            {
                .port     = I2C_NUM_0,
                .addr     = 0x3C,        // typical SSD1306 I2C address
                .rst_gpio = GPIO_NUM_NC, // no reset pin
            },
    };
    ssd1306_handle_t local_d = NULL;

    // Overriding static values.
    cfg = local_cfg;
    d = local_d;

    ESP_ERROR_CHECK(ssd1306_new_i2c(&cfg, &d));
}

static void test_display() {
    // ----- Clear screen -----
    ESP_ERROR_CHECK(ssd1306_clear(d));

    // ----- Draw pixels in corners of screen -----
    ESP_ERROR_CHECK(ssd1306_draw_pixel(d, 0, 0, true));
    ESP_ERROR_CHECK(ssd1306_draw_pixel(d, cfg.width - 1, 0, true));
    ESP_ERROR_CHECK(ssd1306_draw_pixel(d, 0, cfg.height - 1, true));
    ESP_ERROR_CHECK(ssd1306_draw_pixel(d, cfg.width - 1, cfg.height - 1, true));

    // ----- Draw rectangles -----
    ESP_ERROR_CHECK(ssd1306_draw_rect(d, 2, 2, 40, 20, false));
    ESP_ERROR_CHECK(ssd1306_draw_rect(d, 2, 24, 32, 16, true));

    // ----- Draw circles -----
    ESP_ERROR_CHECK(ssd1306_draw_circle(d, 32, 52, 8, true));
    ESP_ERROR_CHECK(ssd1306_draw_circle(d, 100, 52, 4, false));

    // ----- Draw lines -----
    ESP_ERROR_CHECK(ssd1306_draw_line(d, 2, 2, 40, 20, true));
    ESP_ERROR_CHECK(ssd1306_draw_line(d, 32, 52, 100, 52, true));

    // ----- Draw text -----
    ESP_ERROR_CHECK(ssd1306_draw_text(d, 48, 2, "OK!", true));
    ESP_ERROR_CHECK(
        ssd1306_draw_text_scaled(d, 48, 10, "Hello\nWorld!", true, 1));

    ESP_ERROR_CHECK(ssd1306_display(d));

    ESP_LOGI("Main", "Display updated successfully.");
}