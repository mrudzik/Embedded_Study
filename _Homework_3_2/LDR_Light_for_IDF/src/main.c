#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>
#include "sdkconfig.h"
#include "driver/gpio.h"

#define PIN_ADC ADC_CHANNEL_3
#define PIN_LED GPIO_NUM_18

#define ADC_UNIT      ADC_UNIT_1        // ADC1
#define ADC_BITWIDTH  ADC_BITWIDTH_12   // 12-bit resolution (0-4095)
#define ADC_ATTEN     ADC_ATTEN_DB_12   // ~3.3V full-scale voltage


#define LIGHT_LOW_LEVEL 1500
#define LIGHT_HIGH_LEVEL 2500
bool ledOn = false;



void app_main() {
    // ------
    // Analog PIN initialization
    // ------
	int adc_value;
    adc_oneshot_unit_handle_t adc_handle;
	 // Initialize ADC Oneshot Mode Driver on the ADC Unit
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);
	// Configure ADC channel
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH,
        .atten = ADC_ATTEN,
    };
    adc_oneshot_config_channel(adc_handle, PIN_ADC, &config);

    // ------
    // Output PIN initialization
    // ------
    gpio_reset_pin(PIN_LED);
    // Define the configuration structure
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_LED), // Bit mask of the pins you want to configure
        .mode = GPIO_MODE_OUTPUT,                // Set as output mode
        .pull_up_en = GPIO_PULLUP_DISABLE,       // Disable internal pull-up
        .pull_down_en = GPIO_PULLDOWN_DISABLE,   // Disable internal pull-down
        .intr_type = GPIO_INTR_DISABLE           // Disable interrupts
    };
    // Apply the settings
    gpio_config(&io_conf);



	 // ADC Oneshot Analog Read loop
    while (1) {
        // Read ADC value with Oneshot
        adc_oneshot_read(adc_handle, PIN_ADC, &adc_value);
        // Print ADC value
        ESP_LOGI("ADC Value", "%d", adc_value);

        if (!ledOn && adc_value < LIGHT_LOW_LEVEL) {
            gpio_set_level(PIN_LED, 1);
            ledOn = true;
        } else if (ledOn && adc_value > LIGHT_HIGH_LEVEL) {
            gpio_set_level(PIN_LED, 0);
            ledOn = false;
        }

        // Delay 0.1 second
        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }

}