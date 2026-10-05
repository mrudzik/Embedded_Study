//ds1307.h

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

#define DS1307_ADDR 0x68

typedef struct
{
    uint8_t  sec;
    uint8_t  min;
    uint8_t  hour;
    uint8_t  dow;
    uint8_t  day;
    uint8_t  month;
    uint16_t year;
} ds1307_time_t;


esp_err_t ds1307_init(i2c_master_bus_handle_t bus);

esp_err_t ds1307_set_time(const ds1307_time_t *time);
esp_err_t ds1307_get_time(ds1307_time_t *time);

esp_err_t ds1307_is_running(bool *running);