#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/ledc.h"


#define PIN_ADC ADC_CHANNEL_3
// #define PIN_LED GPIO_NUM_18

#define ADC_UNIT      ADC_UNIT_1        // ADC1
#define ADC_BITWIDTH  ADC_BITWIDTH_12   // 12-bit resolution (0-4095)
#define ADC_ATTEN     ADC_ATTEN_DB_12   // ~3.3V full-scale voltage


#define PWM_LED_OUTPUT_IO   (18)              	// The GPIO pin you want to use
#define PWM_LED_CHANNEL     LEDC_CHANNEL_0    	// LEDC channel (0 to 7)
#define PWM_LED_TIMER		LEDC_TIMER_0		// LEDC timer (0 to 3)
#define PWM_LED_FREQUENCY   (5000)            	// Frequency in Hz (5 kHz)

#define PWM_MOTOR_OUTPUT_IO (17) 
#define PWM_MOTOR_CHANNEL   LEDC_CHANNEL_1
#define PWM_MOTOR_TIMER     LEDC_TIMER_1
#define PWM_MOTOR_FREQUENCY (19500)

#define PWM_MODE            LEDC_LOW_SPEED_MODE // Speed mode (LEDC_LOW_SPEED_MODE is universally supported)
#define PWM_RESOLUTION      LEDC_TIMER_12_BIT 	// 12-bit resolution (Duty range: 0 to 4095)





void init_analog_pin(adc_oneshot_unit_handle_t *adc_handle){
	// ------
    // Analog PIN initialization
    // ------
	
    
	 // Initialize ADC Oneshot Mode Driver on the ADC Unit
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, adc_handle));
	// Configure ADC channel
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH,
        .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc_handle, PIN_ADC, &config));

}


void init_dual_pwm(void) {
    // 1. Configure the PWM Timer
    ledc_timer_config_t led_timer = {
        .speed_mode       = PWM_MODE,
        .duty_resolution  = PWM_RESOLUTION,
        .timer_num        = PWM_LED_TIMER,
        .freq_hz          = PWM_LED_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&led_timer));

	ledc_timer_config_t motor_timer = {
        .speed_mode       = PWM_MODE,
        .duty_resolution  = PWM_RESOLUTION,
        .timer_num        = PWM_MOTOR_TIMER,
        .freq_hz          = PWM_MOTOR_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&motor_timer));




    // 2. Configure the PWM Channel and bind it to the GPIO pin
    ledc_channel_config_t led_channel = {
        .speed_mode     = PWM_MODE,
        .channel        = PWM_LED_CHANNEL,
        .timer_sel      = PWM_LED_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PWM_LED_OUTPUT_IO,
        .duty           = 0, // Start with a 0% duty cycle
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&led_channel));

	ledc_channel_config_t motor_channel = {
        .speed_mode     = PWM_MODE,
        .channel        = PWM_MOTOR_CHANNEL,
        .timer_sel      = PWM_MOTOR_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PWM_MOTOR_OUTPUT_IO,
        .duty           = 0, // Start with a 0% duty cycle
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&motor_channel));
}



void set_pwm_duty(uint32_t duty_value, ledc_channel_t channel) {
    // Set the new duty cycle value
    ESP_ERROR_CHECK(ledc_set_duty(PWM_MODE, channel, duty_value));
    // Apply the update to the hardware
    ESP_ERROR_CHECK(ledc_update_duty(PWM_MODE, channel));
}


void app_main() {
	int adc_value;
	adc_oneshot_unit_handle_t adc_handle;
	init_analog_pin(&adc_handle);
	init_dual_pwm();

 	// ADC Oneshot Analog Read loop
    while (1) {
        // Read ADC value with Oneshot
        adc_oneshot_read(adc_handle, PIN_ADC, &adc_value);
        // Print ADC value
        ESP_LOGI("ADC Value", "%d", adc_value);

		set_pwm_duty(adc_value, PWM_LED_CHANNEL);
		set_pwm_duty(adc_value, PWM_MOTOR_CHANNEL);

        // Delay 0.1 second
        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }

}