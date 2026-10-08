#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>
#include <string.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/i2c_master.h"

#include "sound_notes.h"
#include "encoder.h"
#include "ssd1306.h"
#include "servo.h"

// PIN digit selection LEDs
#define PIN_LED1 	GPIO_NUM_4
#define PIN_LED2 	GPIO_NUM_5
#define PIN_LED3 	GPIO_NUM_6
#define PIN_LED4 	GPIO_NUM_7

// Buzzer
#define PIN_BUZZER	GPIO_NUM_18


// I2C
#define PIN_SDA    8
#define PIN_SCL    9
#define OLED_ADDR  0x3C

// Display
#define TEXT_SCALE 8
#define TEXT_X     2
#define TEXT_Y     ((OLED_H - 5 * TEXT_SCALE) / 2) 


#pragma region Initializations


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


void all_init(){
	// Blinking LEDs 
	init_pin_for_led(PIN_LED1);
	init_pin_for_led(PIN_LED2);
	init_pin_for_led(PIN_LED3);
	init_pin_for_led(PIN_LED4);
	// Sound stuff
	init_buzzer(PIN_BUZZER);
	play_sound(start_melody, MELODY_SIZE(start_melody));
	// play_sound(win_melody, MELODY_SIZE(win_melody));

	// Encoder
	encoder_init();
	// Servomotor
	servo_init();
	servo_set_us(400);

	// I2C Bus
	i2c_master_bus_handle_t bus = i2c_bus_init();
	// Display that powered by I2C
    oled_init(bus, OLED_ADDR);
}

#pragma endregion





#pragma region Misc

// Get milliseconds using RTOS ticks
uint32_t millis_rtos() {
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}



static void show_text(const char *str, uint32_t textScale)
{
    fb_clear();
    fb_text(TEXT_X, TEXT_Y, str, textScale);
    oled_flush();
}

static void set_led_light(uint32_t num, bool light){
	switch (num)
	{
	case 0:
		gpio_set_level(PIN_LED1, light);
		break;
	case 1:
		gpio_set_level(PIN_LED2, light);
		break;
	case 2:
		gpio_set_level(PIN_LED3, light);
		break;
	case 3:
		gpio_set_level(PIN_LED4, light);
		break;
	
	default:
		break;
	}
}

static void set_all_led_light(bool light) {
	gpio_set_level(PIN_LED1, light);
	gpio_set_level(PIN_LED2, light);
	gpio_set_level(PIN_LED3, light);
	gpio_set_level(PIN_LED4, light);
}


static const uint32_t ENCODER_DOUBLEPRESS_TIME = 300;
static const uint32_t ENCODER_DEBOUNCE_TIME = 20;
static uint32_t lastPressedEncoderTime = 0;
static bool isDoublePress = false;
static uint32_t lastRecordedClick = 0;

static void check_double_press(uint32_t now){
	if (encoder_pressed) {
		if (now < lastPressedEncoderTime + ENCODER_DOUBLEPRESS_TIME &&
			now > lastPressedEncoderTime + ENCODER_DEBOUNCE_TIME){
			isDoublePress = true;
		}
		lastPressedEncoderTime = now;
	}
}



#pragma endregion





#pragma region Control_Modes

enum controlModes {
	AWAIT, SELECTION, WRONG, CORRECT, ALARM
};

static enum controlModes mode = AWAIT;
static bool initSelection = false;
static bool initWrong = false;

static const uint32_t SELECTION_BLINK_TIME = 400;
static const uint32_t WRONG_BLINK_TIME = 200;
static const uint32_t WRONG_SHOW_TIME = 4000;
static const uint32_t WIN_BLINK_TIME = 100;

static const uint8_t MAX_ATTEMPT_AMMOUNT = 4;

static char correctCombination[5] = "6769";
static int8_t attemptsLeft = MAX_ATTEMPT_AMMOUNT;

void set_control(enum controlModes toSet) {

	mode = toSet;
	switch (toSet)
	{
	case SELECTION:
		set_all_led_light(false);
		servo_target_set(2600);
		show_text("0000", TEXT_SCALE);

		initSelection = true;
		break;

	case WRONG:
		attemptsLeft--;
		if (attemptsLeft <= 0){
			set_control(ALARM);
			break;
		}
		play_sound(wrongCode_melody, MELODY_SIZE(wrongCode_melody));
		initWrong = true;
		break;

	case CORRECT:
		set_all_led_light(true);

		play_sound(win_melody, MELODY_SIZE(win_melody));
		servo_target_set(1500);
		break;
	
	case ALARM:
		set_all_led_light(true);
		show_text("----", TEXT_SCALE);

		play_sound(alarm_melody, MELODY_SIZE(alarm_melody));
		break;
	default:
		break;
	}

	
}


void mode_await(uint32_t now) {
	static bool blinkTest = false;
	static uint32_t blinkTime = 0;
	static const uint32_t blinkDelay = 1000;

	// static uint32_t servoCatchTime = 0;

	servo_target_set(2600);


	if (now >= blinkTime){
		gpio_set_level(PIN_LED1, blinkTest);
		gpio_set_level(PIN_LED2, blinkTest);
		gpio_set_level(PIN_LED3, blinkTest);
		gpio_set_level(PIN_LED4, blinkTest);

		if (blinkTest){
			// servo_target_set(400);
			show_text("1234", TEXT_SCALE);
		} else {
			// servo_target_set(2600);
			show_text("-:0:-", TEXT_SCALE - 1);
		}

		blinkTest = !blinkTest;
		blinkTime = now+blinkDelay;
	}

	if (encoder_pressed)
		set_control(SELECTION);
}



void mode_selection(uint32_t now) {
	static uint8_t selectedDigit = 0;
	static bool isBlinking = false;
	static uint32_t lastTimeBlink = 0;
	static char combination[5] = "0000";
	static char comToShow[5];

	

	if (initSelection) {
		initSelection = false;
		selectedDigit = 0;
	}

	if (now > lastTimeBlink + SELECTION_BLINK_TIME) 
	{ // Blinking Timer
		lastTimeBlink = now;
		isBlinking = !isBlinking;
	}

	
	

	// Encoder Press
	check_double_press(now);
	if (encoder_pressed) {
		if (isDoublePress){
			isDoublePress = false;
			if (strcmp(combination, correctCombination) == 0) {
				set_control(CORRECT);
			} else {
				set_control(WRONG);
			}
		} else {
			lastRecordedClick = 0;
			encoder_click = 0;

			selectedDigit++;
			if (selectedDigit >= 4) 
				selectedDigit = 0;
		}
	}

	// Encoder Roll
	if (encoder_click != lastRecordedClick){
		combination[selectedDigit] += encoder_click - lastRecordedClick;
		lastRecordedClick = encoder_click;
		
		// Reset blink to true
		lastTimeBlink = now;
		isBlinking = true;

		if (combination[selectedDigit] > 57){
			combination[selectedDigit] -= 10;
		} else if (combination[selectedDigit] < 48) {
			combination[selectedDigit] += 10;
		}
	}






	// LED blink on board
	set_all_led_light(false);	
	set_led_light(selectedDigit, isBlinking);

	// Digit blink on display
	strcpy(comToShow, combination);
	if (!isBlinking){
		comToShow[selectedDigit] = '_';
	}
	show_text(comToShow, TEXT_SCALE);
}

void mode_wrong(uint32_t now) {
	static bool isBlinking = false;
	static uint32_t lastTimeBlink = 0;
	static char resultMsg[6] = "     ";

	static uint32_t timeModeStarted = 0;

	if (initWrong){
		initWrong = false;
		timeModeStarted = now;
	}

	if (now > lastTimeBlink + WRONG_BLINK_TIME){
		isBlinking = !isBlinking;
		lastTimeBlink = now;

		if (isBlinking){
			strcpy(resultMsg, "-: :-");
		} else {
			strcpy(resultMsg, "     ");
		}
		resultMsg[2] = attemptsLeft + 48;
		set_all_led_light(isBlinking);
		if (attemptsLeft < 4) set_led_light(3, false);
		if (attemptsLeft < 3) set_led_light(2, false);
		if (attemptsLeft < 2) set_led_light(1, false);
		show_text(resultMsg, TEXT_SCALE - 1);
	}


	if (now > timeModeStarted + WRONG_SHOW_TIME) {
		set_control(SELECTION);
	}

	

}

void mode_correct(uint32_t now) {
	static bool isBlinking = false;
	static uint32_t lastTimeBlink = 0;
	static uint8_t currLED = 0;

	if (now > lastTimeBlink + WIN_BLINK_TIME) {
		isBlinking = !isBlinking;
		lastTimeBlink = now;

		set_all_led_light(false);
		if (isBlinking){
			set_led_light(currLED, true);
		} else {
			currLED++;
			if (currLED >= 4) currLED = 0;
		}

		show_text("--------", TEXT_SCALE - 4);
	}

	check_double_press(now);
	if (isDoublePress) {
		isDoublePress = false;
		attemptsLeft = MAX_ATTEMPT_AMMOUNT;
		set_control(SELECTION);
	}
}

void mode_alarm(uint32_t now) {
	static bool isBlinking = false;
	static uint32_t lastTimeBlink = 0;

	if (now > lastTimeBlink + WRONG_BLINK_TIME){
		isBlinking = !isBlinking;
		lastTimeBlink = now;

		set_all_led_light(isBlinking);
		show_text("--------", TEXT_SCALE - 4);
	}
}

#pragma endregion









void app_main() {

	all_init();

	uint32_t now = millis_rtos();
	
	

	
	while(1)
	{
		now = millis_rtos();
		sound_engine(now);
		check_encoder();
		servo_target();


        
		switch (mode)
		{
		case AWAIT:
			mode_await(now);
			break;
		
		case SELECTION:
			mode_selection(now);
			break;

		case WRONG:
			mode_wrong(now);
			break;
		case CORRECT:
			mode_correct(now);
			break;
		case ALARM:
			mode_alarm(now);
			break;
		default:
			break;
		}
		
		

		
		
		
		vTaskDelay(pdMS_TO_TICKS(LOOP_PERIOD_MS));
	}
}