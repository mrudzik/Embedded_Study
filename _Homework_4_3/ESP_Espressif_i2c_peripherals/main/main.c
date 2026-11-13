#include <stdio.h>
#include "esp_err.h"
#include <driver/i2c_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>

#include <ssd1306.h>

#include <i2c_stuff.h>
#include <display_stuff.h>

#define PIN_SDA GPIO_NUM_8
#define PIN_SDL GPIO_NUM_9


void app_main(void)
{
    i2c_master_bus_handle_t i2c_bus =
        i2c_bus0_init(PIN_SDA, PIN_SDL, 400000);

    init_display();
    test_display();
    

    
}