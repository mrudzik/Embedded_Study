#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/ledc.h"


// #define PIN_ADC ADC_CHANNEL_0
// // #define PIN_LED GPIO_NUM_18

// #define ADC_UNIT      ADC_UNIT_1        // ADC1
// #define ADC_BITWIDTH  ADC_BITWIDTH_12   // 12-bit resolution (0-4095)
// #define ADC_ATTEN     ADC_ATTEN_DB_12   // ~3.3V full-scale voltage


#define PIN_ADC 		ADC_CHANNEL_3
#define ADC_UNIT      	ADC_UNIT_1        // ADC1
#define ADC_BITWIDTH  	ADC_BITWIDTH_12   // 12-bit resolution (0-4095)
#define ADC_ATTEN     	ADC_ATTEN_DB_12   // ~3.3V full-scale voltage




#define SERVO_GPIO      18
#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_DUTY_RES   LEDC_TIMER_14_BIT

#define SERVO_MAX_DUTY (1u<<14)
#define SERVO_PERIOD_US 20000u


static void servo_init(void)
{
    ledc_timer_config_t t = {
        .speed_mode      = LEDC_MODE,
        .timer_num       = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz         = 50,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&t));

    ledc_channel_config_t c = {
        .gpio_num   = SERVO_GPIO,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL,
        .timer_sel  = LEDC_TIMER,
        .intr_type  = LEDC_INTR_DISABLE,
        .duty       = 0,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&c));
}


static void servo_set_us(uint32_t us){
	if (us > 2600) 	us = 2600;
	if (us < 400) 	us = 400;

	uint32_t duty = (uint32_t)((uint32_t)(us * SERVO_MAX_DUTY) / SERVO_PERIOD_US);

	ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
	ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}


void app_main() {

	// ------
    // Analog PIN initialization
    // ------
	int adc_value;
    adc_oneshot_unit_handle_t adc_handle;
	 // Initialize ADC Oneshot Mode Driver on the ADC Unit
    adc_oneshot_unit_init_cfg_t init_config = {
		// .gpio_num	= 18,
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





	servo_init();
	servo_set_us(400);

	while(1)
	{
		// for(int i = 0 ; i <= 2600; i+=100){
		// 	servo_set_us(i);
		// 	vTaskDelay(pdMS_TO_TICKS(150));
		// }
		// vTaskDelay(pdMS_TO_TICKS(1000));

		// for(int i = 2600 ; i >= 400; i-=100){
		// 	servo_set_us(i);
		// 	vTaskDelay(pdMS_TO_TICKS(150));
		// }

		// Read ADC value with Oneshot
        adc_oneshot_read(adc_handle, PIN_ADC, &adc_value);
        // Print ADC value
        ESP_LOGI("\nADC Value", "%d", adc_value);
		printf("\nValue %d, Divided %ld", adc_value, (uint32_t)(adc_value / 1.5));

		servo_set_us((uint32_t)(adc_value / 1.5));



		vTaskDelay(pdMS_TO_TICKS(100));
	}

	

}