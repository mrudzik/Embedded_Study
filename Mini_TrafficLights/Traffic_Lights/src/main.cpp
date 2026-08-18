#include <Arduino.h>

#define PIN_BUTTON 4

#define PIN_LED_RED 15
#define PIN_LED_YELLOW 16
#define PIN_LED_GREEN 17

volatile bool buttonSwitch = false;
constexpr uint32_t buttonDebounceDelay = 200; // ms

constexpr uint32_t lightsBlinkSpeed = 500; // ms
constexpr uint32_t lightsModeLenght = 3000; // ms

constexpr uint32_t lightsModeLenght_RED = 6000;
constexpr uint32_t lightsModeLenght_RED_YELLOW = 2500;
constexpr uint32_t lightsModeLenght_YELLOW = 2500;
constexpr uint32_t lightsModeLenght_GREEN = 4000;
constexpr uint32_t lightsModeLenght_GREEN_BLINK = 3500;


// constexpr uint32_t lightsModeSize

void IRAM_ATTR buttonInterrupt();
void lightsNightMode(bool blink);
void lightsDayMode(bool blink);


void setup() {
  
  Serial.begin(115200);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  attachInterrupt(PIN_BUTTON, buttonInterrupt, FALLING);

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  digitalWrite(PIN_LED_RED, true);
  digitalWrite(PIN_LED_YELLOW, true);
  digitalWrite(PIN_LED_GREEN, true);

}

void IRAM_ATTR buttonInterrupt(){
  static uint32_t lastTimePressed = 0;

  if (buttonSwitch) 
    return; // Не опрацьованна кнопка
  
  uint32_t now = millis();
  if (now < lastTimePressed + buttonDebounceDelay)
    return; // Дебаунс спрацював, ще рано переключати.

  // Переключаєм
  buttonSwitch = true;
  lastTimePressed = now;
}





bool nightMode = false;
bool justSwitched = true;

void loop() {
  static uint32_t lastLightSwitch = 0;
  uint32_t now = millis();
  bool blink = false;
  if (now > lastLightSwitch + lightsBlinkSpeed){
    blink = true;
    lastLightSwitch = now;
  }
  

  if (buttonSwitch) { // Опрацьовуєм подію
    nightMode = !nightMode;
    buttonSwitch = false;
    justSwitched = true;
  }


  if (nightMode)
    lightsNightMode(blink);
  else
    lightsDayMode(blink);

}


void lightsNightMode(bool blink) {
  

  if (justSwitched) {
    digitalWrite(PIN_LED_RED, false);
    digitalWrite(PIN_LED_YELLOW, false);
    digitalWrite(PIN_LED_GREEN, false);

    justSwitched = false;
  }

  if (blink){
    digitalWrite(PIN_LED_YELLOW, !digitalRead(PIN_LED_YELLOW));
  }

}

void lightsDayMode(bool blink) {
  static uint32_t currentMode = 0;
  static uint32_t lastModeSwitch = 0;
  static bool justSwitchedMode = true;

  uint32_t now = millis();


  if (justSwitched) {
    digitalWrite(PIN_LED_RED, false);
    digitalWrite(PIN_LED_YELLOW, false);
    digitalWrite(PIN_LED_GREEN, false);

    justSwitched = false;
    currentMode = 0;
    lastModeSwitch = now;
    justSwitchedMode = true;
  }

  if (blink){
    switch (currentMode){
      case 0: // Зелений — Дозволяє рух
        if (justSwitchedMode) {
          digitalWrite(PIN_LED_YELLOW, false);
          digitalWrite(PIN_LED_RED, false);
          digitalWrite(PIN_LED_GREEN, true);
          justSwitchedMode = false;
        }

        if (now > lastModeSwitch + lightsModeLenght_GREEN){
          lastModeSwitch = now;
          justSwitchedMode = true;
          currentMode++;
        }
        
        break;
      case 1: // Зелений миготливий — попереджає
        if (justSwitchedMode) {
          digitalWrite(PIN_LED_YELLOW, false);
          digitalWrite(PIN_LED_RED, false);
          digitalWrite(PIN_LED_GREEN, true);
          justSwitchedMode = false;
        }
        if (blink) {
          digitalWrite(PIN_LED_GREEN, !digitalRead(PIN_LED_GREEN));
        }


        if (now > lastModeSwitch + lightsModeLenght_GREEN_BLINK){
          lastModeSwitch = now;
          justSwitchedMode = true;
          currentMode++;
        }

        
        break;
      case 2: // Жовтий — забороняє рух і попереджає
        if (justSwitchedMode) {
          digitalWrite(PIN_LED_RED, false);
          digitalWrite(PIN_LED_GREEN, false);
          digitalWrite(PIN_LED_YELLOW, true);
          justSwitchedMode = false;
        }

        if (now > lastModeSwitch + lightsModeLenght_YELLOW){
          lastModeSwitch = now;
          justSwitchedMode = true;
          currentMode++;
        }
        
        break;
      case 3: // Червоний —  Забороняє рух
        if (justSwitchedMode) {
          digitalWrite(PIN_LED_GREEN, false);
          digitalWrite(PIN_LED_YELLOW, false);
          digitalWrite(PIN_LED_RED, true);
          justSwitchedMode = false;
        }

        if (now > lastModeSwitch + lightsModeLenght_RED){
          lastModeSwitch = now;
          justSwitchedMode = true;
          currentMode++;
        }
        
        break;
      case 4: // Червоний та Жовтий одночасно
        if (justSwitchedMode) {
          digitalWrite(PIN_LED_GREEN, false);
          digitalWrite(PIN_LED_YELLOW, true);
          digitalWrite(PIN_LED_RED, true);
          justSwitchedMode = false;
        }

        if (now > lastModeSwitch + lightsModeLenght_RED_YELLOW){
          lastModeSwitch = now;
          justSwitchedMode = true;
          currentMode = 0;
        }
        
        break;


      default:
        justSwitched = true;
        break;
    }



    // digitalWrite(PIN_LED_GREEN, !digitalRead(PIN_LED_GREEN));
  }

}

