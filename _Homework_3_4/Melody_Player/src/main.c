

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define BUZZER_GPIO     16
#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_DUTY_RES   LEDC_TIMER_10_BIT

#define R 0

#define G2   98
#define A2  110
#define B2  123
#define C2  262
#define D2  294
#define E2  82

#define D3  147
#define E3  165
#define G3  196
#define A3  220

#define C4       262
#define CS4      277
#define D4       294
#define DS4      311
#define E4       330
#define F4       349
#define FS4      370
#define G4       392
#define GS4      415
#define A4       440       /* еталон міжнародного настроювання */
#define AS4      466
#define B4       494
 
/* ---------------------------------------------------------------- октава 5 */
#define C5       523
#define CS5      554
#define D5       587
#define DS5      622
#define E5       659
#define F5       698
#define FS5      740
#define G5       784
#define GS5      831
#define A5       880
#define AS5      932
#define B5       988
 
/* ---------------------------------------------------------------- октава 6 */
#define C6      1047
#define CS6     1109
#define D6      1175
#define DS6     1245
#define E6      1319
#define F6      1397
#define FS6     1480
#define G6      1568
#define GS6     1661
#define A6      1760
#define AS6     1865
#define B6      1976
 
/* ---------------------------------------------------------------- октава 7 */
#define C7      2093
#define CS7     2217
#define D7      2349
#define DS7     2489
#define E7      2637
#define F7      2794
#define FS7     2960
#define G7      3136
#define GS7     3322
#define A7      3520
#define AS7     3729
#define B7      3951
 

#define NOTE_C7  2093
#define NOTE_D7  2349
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_G7  3136
#define NOTE_A7  3520
#define NOTE_B7  3951
#define NOTE_C8  4186

typedef struct {
    uint16_t freq;     
    uint16_t duration;  
} note_t;


static const note_t melody[] = {
    {E5,200},{B4,100},{C5,100},{D5,200},{C5,100},{B4,100},
    {A4,200},{A4,100},{C5,100},{E5,200},{D5,100},{C5,100},
    {B4,300},        {C5,100},{D5,200},{E5,200},
    {C5,200},{A4,200},{A4,200},{R, 200},
 
    {D5,300},        {F5,100},{A5,200},{G5,100},{F5,100},
    {E5,300},        {C5,100},{E5,200},{D5,100},{C5,100},
    {B4,200},{B4,100},{C5,100},{D5,200},{E5,200},
    {C5,200},{A4,200},{A4,200},{R, 200},
 
    {E5,400},{C5,400},
    {D5,400},{B4,400},
    {C5,400},{A4,400},
    {GS4,400},{B4,200},{R,200},
 
    {E5,400},{C5,400},
    {D5,400},{B4,400},
    {C5,200},{E5,200},{A5,400},
    {GS5,400},{R,400},
};
#define MELODY_LEN (sizeof(melody) / sizeof(note_t))

static void buzzer_init(void)
{
    ledc_timer_config_t t = {
        .speed_mode      = LEDC_MODE,
        .timer_num       = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz         = 2700,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&t));

    ledc_channel_config_t c = {
        .gpio_num   = BUZZER_GPIO,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL,
        .timer_sel  = LEDC_TIMER,
        .intr_type  = LEDC_INTR_DISABLE,
        .duty       = 0,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&c));
}

static void tone_on(uint16_t freq)
{
    ledc_set_freq(LEDC_MODE, LEDC_TIMER, freq);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 512);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

static void tone_off(void)
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

static void play_note(uint16_t freq, uint16_t duration)
{
    uint16_t play = duration  * 85 / 100 + 50;
    uint16_t gap  = duration - play +50;

    if (freq == R) {
        tone_off();
        vTaskDelay(pdMS_TO_TICKS(duration));
        return;
    }
    tone_on(freq);
    vTaskDelay(pdMS_TO_TICKS(play));
    tone_off();
    vTaskDelay(pdMS_TO_TICKS(gap));
}

void app_main(void)
{
    buzzer_init();

    while (1) {
        for (int i = 0; i < MELODY_LEN; i++) {
            play_note(melody[i].freq, melody[i].duration);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}