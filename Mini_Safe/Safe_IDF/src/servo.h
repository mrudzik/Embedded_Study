#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/ledc.h"





#define SERVO_GPIO      17
#define LEDC_TIMER      LEDC_TIMER_1
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL    LEDC_CHANNEL_1
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

	// ESP_LOGI("\nServo duty update", "");
}

static uint32_t currentServoUs = 400;
static uint32_t targetServoUs = 400;
static uint32_t stepServoUs = 150;

static void servo_target(){
	if (targetServoUs > currentServoUs) {
		currentServoUs += stepServoUs;
		if (targetServoUs < currentServoUs)
			currentServoUs = targetServoUs;
	} else if (targetServoUs < currentServoUs) {
		currentServoUs -= stepServoUs;
		if (targetServoUs > currentServoUs)
			currentServoUs = targetServoUs;
	}

	servo_set_us(currentServoUs);
}

static void servo_target_set(uint32_t us){
	if (us > 2600) 	us = 2600;
	if (us < 400) 	us = 400;

	targetServoUs = us;
}