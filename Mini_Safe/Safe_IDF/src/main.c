#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/i2c_master.h"

#include "sound_notes.h"
#include "encoder.h"
#include "ssd1306.h"

// PIN digit selection LEDs
#define PIN_LED1 	GPIO_NUM_4
#define PIN_LED2 	GPIO_NUM_5
#define PIN_LED3 	GPIO_NUM_6
#define PIN_LED4 	GPIO_NUM_7

// Buzzer
#define PIN_BUZZER				GPIO_NUM_18


// I2C
#define PIN_SDA    8
#define PIN_SCL    9
#define OLED_ADDR  0x3C

// Display
#define TEXT_SCALE 8
#define TEXT_X     2
#define TEXT_Y     ((OLED_H - 5 * TEXT_SCALE) / 2) 






void init_pin_for_led(gpio_num_t pin_number){
	// 1. Create configuration structure
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin_number), 	// Select the pin
        .mode = GPIO_MODE_OUTPUT,            	// Set as output mode
        .pull_up_en = GPIO_PULLUP_DISABLE,   	// Disable pull-up resistor
        .pull_down_en = GPIO_PULLDOWN_DISABLE, 	// Disable pull-down resistor
        .intr_type = GPIO_INTR_DISABLE       	// Disable interrupts
    };
    // 2. Apply the configuration
    gpio_config(&io_conf);
    // 3. Set initial state (optional: turn high or low)
    gpio_set_level(pin_number, 0); 
}


// Get milliseconds using RTOS ticks
uint32_t millis_rtos() {
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}


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


void app_main() {
	init_pin_for_led(PIN_LED1);
	init_pin_for_led(PIN_LED2);
	init_pin_for_led(PIN_LED3);
	init_pin_for_led(PIN_LED4);

	init_buzzer(PIN_BUZZER);
	play_sound(start_melody, MELODY_SIZE(start_melody));

	encoder_init();


	i2c_master_bus_handle_t bus = i2c_bus_init();
    oled_init(bus, OLED_ADDR);      


	

	uint32_t now = millis_rtos();

	// uint32_t test1_time = now + 2000;
	// uint32_t test2_time = 0;//now + 4000;
	// // uint32_t test3_time = now + 6000;
	// bool playedFirst = false;
	
	


	bool blinkTest = false;
	uint32_t blinkTime = now;
	uint32_t blinkDelay = 1000;
	while(1)
	{
		now = millis_rtos();
		sound_engine(now);
		check_encoder();
        // vTaskDelay(pdMS_TO_TICKS(10));


		if (now >= blinkTime){
			gpio_set_level(PIN_LED1, blinkTest);
			gpio_set_level(PIN_LED2, blinkTest);
			gpio_set_level(PIN_LED3, blinkTest);
			gpio_set_level(PIN_LED4, blinkTest);
			blinkTest = !blinkTest;
			blinkTime = now+blinkDelay;
		}
		
		// ESP_LOGI("\nLED_check", "");
		

		show_text("1234");

		// vTaskDelay(pdMS_TO_TICKS(1000));
		vTaskDelay(pdMS_TO_TICKS(LOOP_PERIOD_MS));
	}
}