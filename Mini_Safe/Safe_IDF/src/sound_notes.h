#include <stdio.h>


#define BUZZER_LEDC_TIMER		LEDC_TIMER_0
#define BUZZER_LEDC_MODE		LEDC_LOW_SPEED_MODE
#define BUZZER_LEDC_CHANNEL		LEDC_CHANNEL_0
#define BUZZER_LEDC_DUTY_RES	LEDC_TIMER_10_BIT
 
// Усі ноти від C0 до B7. Символ 'S' означає дієз (#), наприклад CS -> C#
enum {
    
    /* Октава 0 */  C0 = 16,   CS0 = 17,   D0 = 18,   DS0 = 19,   E0 = 21,   F0 = 22,   FS0 = 23,   G0 = 24,   GS0 = 26,   A0 = 28,   AS0 = 29,   B0 = 31,
    /* Октава 1 */  C1 = 33,   CS1 = 35,   D1 = 37,   DS1 = 39,   E1 = 41,   F1 = 44,   FS1 = 46,   G1 = 49,   GS1 = 52,   A1 = 55,   AS1 = 58,   B1 = 62,
    /* Октава 2 */  C2 = 65,   CS2 = 69,   D2 = 73,   DS2 = 78,   E2 = 82,   F2 = 87,   FS2 = 93,   G2 = 98,   GS2 = 104,  A2 = 110,  AS2 = 117,  B2 = 123,
    /* Октава 3 */  C3 = 131,  CS3 = 139,  D3 = 147,  DS3 = 156,  E3 = 165,  F3 = 175,  FS3 = 185,  G3 = 196,  GS3 = 208,  A3 = 220,  AS3 = 233,  B3 = 247,
    /* Октава 4 */  C4 = 262,  CS4 = 277,  D4 = 294,  DS4 = 311,  E4 = 330,  F4 = 349,  FS4 = 370,  G4 = 392,  GS4 = 415,  A4 = 440,  AS4 = 466,  B4 = 494,
    /* Октава 5 */  C5 = 523,  CS5 = 554,  D5 = 587,  DS5 = 622,  E5 = 659,  F5 = 698,  FS5 = 740,  G5 = 784,  GS5 = 831,  A5 = 880,  AS5 = 932,  B5 = 988,
    /* Октава 6 */  C6 = 1047, CS6 = 1109, D6 = 1175, DS6 = 1245, E6 = 1319, F6 = 1397, FS6 = 1480, G6 = 1568, GS6 = 1661, A6 = 1760, AS6 = 1865, B6 = 1976,
    /* Октава 7 */  C7 = 2093, CS7 = 2217, D7 = 2349, DS7 = 2489, E7 = 2637, F7 = 2794, FS7 = 2960, G7 = 3136, GS7 = 3322, A7 = 3520, AS7 = 3729, B7 = 3951,

    // Пауза (якщо потрібна тиша у мелодії)
    REST = 0, END = -1
};



typedef struct {
    uint16_t freq;     
    uint16_t duration;  
} note_t;

static bool finishedPlaying = true;
// typedef struct {
//     note_t* soundArray;
//     int size;
// } melody_t;


// static note_t melody[] = {
//     {E3,200},{E3,200},{E3,200},{G3,200},{E3,200},{E3,200},{AS3,200},{E3,200},
//     {E3,200},{E3,200},{E3,200},{G3,200},{AS3,200},{A3,200},{G3,200},{FS3,200},

//     {E3,200},{E3,200},{E3,200},{G3,200},{E3,200},{E3,200},{AS3,200},{E3,200},
//     {E3,400},{G3,400},{AS3,400},{B3,400},

//     {G3,200},{G3,200},{G3,200},{AS3,200},{G3,200},{G3,200},{CS4,200},{G3,200},
//     {FS3,200},{FS3,200},{FS3,200},{A3,200},{FS3,200},{FS3,200},{C4,200},{FS3,200},

//     {E3,200},{E3,200},{G3,200},{AS3,200},{B3,200},{AS3,200},{G3,200},{E3,200},
//     {E3,1200},{REST,400},

//     {REST,2000}
// };


static note_t start_melody[] = {
    {C3,150}, {E3,150}, {G3,150}, {C4,150}, {REST,50},
    {G3,200}, {C4,350}, {REST,100}, {END,1}
}; 

static note_t next_sound[] = {
    {C3,150}, {E3,150}, {G3,150}, {END,1}
};

static note_t previous_sound[] = {
    {G3,150}, {E3,150}, {C3,150}, {END,1}
};

static note_t* current_melody = start_melody;
static int32_t current_melody_size = 0;

 //[] = {
//     {C3,150}, {E3,150}, {G3,150}, {C4,150}, {REST,50},
//     {G3,200}, {C4,350}, {REST,100}
// };


//  #define MELODY_LEN (sizeof(*current_melody) / sizeof(note_t))
#define MELODY_SIZE(melody) (sizeof(melody) / sizeof(note_t))

static void init_buzzer(gpio_num_t pin_number)
{
    ledc_timer_config_t t = {
        .speed_mode      = BUZZER_LEDC_MODE,
        .timer_num       = BUZZER_LEDC_TIMER,
        .duty_resolution = BUZZER_LEDC_DUTY_RES,
        .freq_hz         = 2700,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&t));

    ledc_channel_config_t c = {
        .gpio_num   = pin_number,
        .speed_mode = BUZZER_LEDC_MODE,
        .channel    = BUZZER_LEDC_CHANNEL,
        .timer_sel  = BUZZER_LEDC_TIMER,
        .intr_type  = LEDC_INTR_DISABLE,
        .duty       = 0,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&c));
}

static void tone_on(uint16_t freq)
{
    ledc_set_freq(BUZZER_LEDC_MODE, BUZZER_LEDC_TIMER, freq);
    ledc_set_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL, 512);
    ledc_update_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL);
}

static void tone_off(void)
{
    ledc_set_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL, 0);
    ledc_update_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL);
}

static void play_note(uint16_t freq)
{
    if (freq == REST) {
        tone_off();
        return;
    }
    tone_on(freq);
}


static uint32_t timeToPlay = 0;
static int noteIndex = 0;
static bool notePlaying = false;

static void play_sound(note_t* soundToPlay, uint32_t size){
    timeToPlay = 0;
	noteIndex = 0;
	notePlaying = false;

    current_melody = soundToPlay;
    current_melody_size = size;

    printf("SIZE = %ld\n", size);
}

static void sound_engine(uint32_t now) {
    // Music
    if (noteIndex < current_melody_size && now >= timeToPlay) {
		if (!notePlaying){
			play_note(current_melody[noteIndex].freq);
			timeToPlay = now + current_melody[noteIndex].duration;
			noteIndex++;
			notePlaying = true;
		} else { // GAP
			timeToPlay = now + 50;
			notePlaying = false;
			tone_off();
		}
    }

	if (noteIndex >= current_melody_size) {
		// noteIndex = 0;
        finishedPlaying = true;
	}

    // // Testing
    // static bool isFirstTime = true;
    // static uint32_t timeTest1 = 0;
    // if (isFirstTime) {
    //     isFirstTime = false;

    // }
    

}


