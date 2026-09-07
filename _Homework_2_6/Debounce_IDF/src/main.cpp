#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#include <inttypes.h>
// #include "driver/ledc.h"
// #include "esp_err.h"

#define PIN_BUTTON GPIO_NUM_4 // Будем використовувати одну кнопку, просто порівнюватимемо результати різної обробки


volatile uint32_t press1 = 0; // 1 FALLING Interrupt без debounce
volatile uint32_t press2 = 0; // 2 — Software debounce
constexpr uint32_t debounceTime = 50 * 1000;

uint32_t press3 = 0; // 3 — State-based debounce
volatile bool pressedEvent = false;

uint32_t press4 = 0; // 4 — Polling + debounce
uint32_t lastTimePolled = 0;
constexpr uint32_t pollDelay = 5 * 1000;
bool buttonPressed = false;


constexpr uint32_t logDelay = 100 * 1000;




static void IRAM_ATTR interruptButtonPress(void *arg){
  press1 = press1 + 1; 

  static uint32_t lastTimePressed = 0;
  uint32_t now = esp_timer_get_time();
  if (now > lastTimePressed + debounceTime) {
    lastTimePressed = now;
    press2 = press2 + 1; 
  }

  pressedEvent = true; // 3. Для події треба
}


void initialize_gpio(){
	gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_BUTTON), // Bit mask of the pins you want to configure
        .mode = GPIO_MODE_INPUT,             // Set as input mode
        .pull_up_en = GPIO_PULLUP_ENABLE,    // Enable internal pull-up resistor
        .pull_down_en = GPIO_PULLDOWN_DISABLE,// Disable internal pull-down resistor
        .intr_type = GPIO_INTR_NEGEDGE      
    };
	gpio_config(&io_conf);


	gpio_install_isr_service(ESP_INTR_FLAG_IRAM);

	gpio_isr_handler_add(PIN_BUTTON, interruptButtonPress, (void*) PIN_BUTTON);
}



void measure_debounce(){
	static uint32_t lastTimeLogged = 0;
	uint32_t now = esp_timer_get_time();

	if (pressedEvent) {
		if (!gpio_get_level(PIN_BUTTON)) { // Still pressed
		press3 = press3 + 1; 
		pressedEvent = false;
		}
	}


	
	if (now > lastTimePolled + pollDelay) {
		lastTimePolled = now;
		bool buttonState = !gpio_get_level(PIN_BUTTON);
		if (buttonState != buttonPressed){
		buttonPressed = buttonState;
		if (buttonPressed)
			press4 = press4 + 1; // Increment only when pressed not released
		}
	}


	if (now > lastTimeLogged + logDelay) {
		lastTimeLogged = now;
		printf("\n===================");
		printf("\nBUTTON       | %d", gpio_get_level(PIN_BUTTON));
		printf("\nNo debounce: | %lu", press1);
		printf("\nTime-based:  | %lu", press2);
		printf("\nState-based: | %lu", press3);
		printf("\nPolling:     | %lu  | PRESSED : %d", press4, buttonPressed);
		printf("\nHardware RC — ON");
	}
}



extern "C" void app_main() {
	// Initialization
	initialize_gpio();

	// int pin_level = 0;
	// Loop
	while(1) {
		// pin_level = gpio_get_level(PIN_BUTTON);
		// printf("GPIO %d level: %d\n", PIN_BUTTON, pin_level);
		// vTaskDelay(pdMS_TO_TICKS(10));
		measure_debounce();


	}

}


