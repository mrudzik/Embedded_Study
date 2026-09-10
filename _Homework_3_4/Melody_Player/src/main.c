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


/* ---------------------------------------------------------------- октава 0 */
#define C0        16
#define CS0       17
#define D0        18
#define DS0       19
#define E0        21
#define F0        22
#define FS0       23
#define G0        25
#define GS0       26
#define A0        28
#define AS0       29
#define B0        31
 
/* ---------------------------------------------------------------- октава 1 */
#define C1        33
#define CS1       35
#define D1        37
#define DS1       39
#define E1        41
#define F1        44
#define FS1       46
#define G1        49
#define GS1       52
#define A1        55
#define AS1       58
#define B1        62
 
/* ---------------------------------------------------------------- октава 2 */
#define C2        65
#define CS2       69
#define D2        73
#define DS2       78
#define E2        82
#define F2        87
#define FS2       92
#define G2        98
#define GS2      104
#define A2       110
#define AS2      117
#define B2       123
 
/* ---------------------------------------------------------------- октава 3 */
#define C3       131
#define CS3      139
#define D3       147
#define DS3      156
#define E3       165
#define F3       175
#define FS3      185
#define G3       196
#define GS3      208
#define A3       220
#define AS3      233
#define B3       247

/* ---------------------------------------------------------------- октава 4 */

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
    {E3,1},{E3,1},{E3,1},{G3,1},{E3,1},{E3,1},{AS3,1},{E3,1},
    {E3,1},{E3,1},{E3,1},{G3,1},{AS3,1},{A3,1},{G3,1},{FS3,1},

    {E3,1},{E3,1},{E3,1},{G3,1},{E3,1},{E3,1},{AS3,1},{E3,1},
    {E3,2},{G3,2},{AS3,2},{B3,2},

    {G3,1},{G3,1},{G3,1},{AS3,1},{G3,1},{G3,1},{CS4,1},{G3,1},
    {FS3,1},{FS3,1},{FS3,1},{A3,1},{FS3,1},{FS3,1},{C4,1},{FS3,1},

    {E3,1},{E3,1},{G3,1},{AS3,1},{B3,1},{AS3,1},{G3,1},{E3,1},
    {E3,6},{R,2},

    {R,20},

    {D4,1},{D4,1},{D4,1},{A4,1},{D4,1},{D4,1},{AS4,1},{A4,1},
    {D4,1},{D4,1},{D4,1},{A4,1},{C5,1},{AS4,1},{A4,1},{G4,1},

    {F4,2},{E4,2},{D4,2},{CS4,2},
    {D4,2},{F4,2},{A4,2},{D5,2},

    {D5,1},{C5,1},{AS4,1},{A4,1},{G4,1},{F4,1},{E4,1},{D4,1},
    {CS4,2},{D4,2},{CS4,2},{D4,2},

    {D4,4},{R,4},

    {R,20},

    {E4,4},{R,1},{F4,3},{E4,4},{AS3,4},
    {E4,4},{R,1},{G4,3},{FS4,4},{E4,4},

    {C4,4},{B3,4},{AS3,4},{B3,4},
    {E4,6},{DS4,2},{E4,8},

    {A4,4},{GS4,4},{G4,4},{FS4,4},
    {F4,6},{E4,2},{AS3,8},

    {E4,8},{R,4},
    {R,20},
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
    uint16_t play = duration  * 85 / 100;
    uint16_t gap  = duration - play;

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
            play_note(melody[i].freq, melody[i].duration*170);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}