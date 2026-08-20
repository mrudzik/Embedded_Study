#include <Arduino.h>








#define PIN_BUTTON 4 // Будем використовувати одну кнопку, просто порівнюватимемо результати різної обробки

void IRAM_ATTR interruptButtonPress();


volatile uint32_t press1 = 0; // 1 FALLING Interrupt без debounce
volatile uint32_t press2 = 0; // 2 — Software debounce
constexpr uint32_t debounceTime = 50;

uint32_t press3 = 0; // 3 — State-based debounce
volatile bool pressedEvent = false;

uint32_t press4 = 0; // 4 — Polling + debounce
uint32_t lastTimePolled = 0;
constexpr uint32_t pollDelay = 5;
bool buttonPressed = false;

// uint32_t press5 = 0; // 5 — Hardware debounce

constexpr uint32_t logDelay = 100;



void setup() {
  pinMode(PIN_BUTTON, INPUT_PULLDOWN);

  attachInterrupt(PIN_BUTTON, interruptButtonPress, FALLING);

  Serial.begin(115200);
}

void IRAM_ATTR interruptButtonPress(){
  press1++; 

  static uint32_t lastTimePressed = 0;
  uint32_t now = millis();
  if (now > lastTimePressed + debounceTime) {
    lastTimePressed = now;
    press2++; 
  }

  pressedEvent = true; // 3. Для події треба
}


void loop() {
  static uint32_t lastTimeLogged = 0;
  

  uint32_t now = millis();

  if (pressedEvent) {
    if (!digitalRead(PIN_BUTTON)) { // Still pressed
      press3++; 
      pressedEvent = false;
    }
  }


  
  if (now > lastTimePolled + pollDelay) {
    lastTimePolled = now;
    bool buttonState = !digitalRead(PIN_BUTTON);
    if (buttonState != buttonPressed){
      buttonPressed = buttonState;
      if (buttonPressed)
        press4++; // Increment only when pressed not released
    }
  }


  if (now > lastTimeLogged + logDelay) {
    lastTimeLogged = now;
    Serial.printf("\n===================");
    Serial.printf("\nBUTTON       | %d", digitalRead(PIN_BUTTON));
    Serial.printf("\nNo debounce: | %d", press1);
    Serial.printf("\nTime-based:  | %d", press2);
    Serial.printf("\nState-based: | %d", press3);
    Serial.printf("\nPolling:     | %d  | PRESSED : %d", press4, buttonPressed);
    Serial.printf("\nHardware RC — ON");
  }



}

