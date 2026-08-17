#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define POTENCIAL_PIN 4 // Отримуєм данні з потенціометра
#define PWM_PIN 15 // Керуєм двіжком 

constexpr uint32_t PWM_FREQ = 5000; // Hz
constexpr uint32_t PWM_RES = 8;



void app_main(void) {
    // 1. Initialization code goes here

    // 2. Main execution loop
    while (1) {
        // Repeated code goes here
        
        // CRITICAL: You MUST yield control to FreeRTOS in infinite loops!
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}