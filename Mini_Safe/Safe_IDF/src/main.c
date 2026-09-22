#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "../src/sound_notes.h"


// PIN digit selection LEDs
#define PIN_LED1 	GPIO_NUM_4
#define PIN_LED2 	GPIO_NUM_5
#define PIN_LED3 	GPIO_NUM_6
#define PIN_LED4 	GPIO_NUM_7

// Buzzer
#define PIN_BUZZER				GPIO_NUM_18




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




void app_main() {
	init_pin_for_led(PIN_LED1);
	init_pin_for_led(PIN_LED2);
	init_pin_for_led(PIN_LED3);
	init_pin_for_led(PIN_LED4);

	init_buzzer(PIN_BUZZER);



	bool blinkTest = false;

	uint32_t now = millis_rtos();

	// Music
	uint32_t timeToPlay = now;
	int noteIndex = 0;
	bool notePlaying = false;
	while(1)
	{
		now = millis_rtos();
		if (noteIndex < MELODY_LEN && now >= timeToPlay) {
			if (!notePlaying){
				play_note(melody[noteIndex].freq, melody[noteIndex].duration);
				timeToPlay = now + melody[noteIndex].duration;
				noteIndex++;
				notePlaying = true;
				// ESP_LOGI("\nPlay note", "");
			} else { // GAP
				timeToPlay = now + 50;
				notePlaying = false;
				tone_off();
				// ESP_LOGI("\nDisable note", "");
			}
        }
		if (noteIndex >= MELODY_LEN) {
			noteIndex = 0;
		}
		
        // vTaskDelay(pdMS_TO_TICKS(10));


		// gpio_set_level(PIN_LED1, blinkTest);
		// gpio_set_level(PIN_LED2, blinkTest);
		// gpio_set_level(PIN_LED3, blinkTest);
		// gpio_set_level(PIN_LED4, blinkTest);

		// blinkTest = !blinkTest;
		// ESP_LOGI("\nLED_check", "");
		// vTaskDelay(pdMS_TO_TICKS(1000));
	}
}