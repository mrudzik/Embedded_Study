


// Init i2c
static i2c_master_bus_handle_t i2c_bus0_init(gpio_num_t sda, gpio_num_t scl,
                                             uint32_t hz) {
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port                     = I2C_NUM_0,
        .sda_io_num                   = sda,
        .scl_io_num                   = scl,
        .clk_source                   = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt            = 0,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus));
    // NOTE: per-device speed is set when adding the device (in driver bind).
    return bus;
}
