#include <Arduino.h>

// Підчистив файл, це його остання версія

#pragma region Homework_2_1

// Домашка 2.1

// Мікроконтроллер видає 3.3 Вольта
// Зелений світлодіод
// Падіння 1.6-4.0 Вольта 
//    — 7.7мА — резистор 220 Ом
// Червоні світлодіоди
// Падіння 1.5-2.0 Вольта
//    — 7.7мА — резистор 220 Ом
// Сині світлодіоди
// Падіння 2.5-3.7 Вольта
//    — 7мА — резистор 100 Ом

#define CE_INT constexpr uint32_t

CE_INT LED_OUT = 4; // Power display
CE_INT LED_RED1 = 5;
CE_INT LED_RED2 = 6;
CE_INT LED_BLUE1 = 15;
CE_INT LED_BLUE2 = 16;
CE_INT BUTTON_PIN = 18;

// CE_INT SPEED = 400;
// CE_INT PAUSE = 200;

volatile bool buttonPressed = false;
enum LightModes { ON, MODE_1, MODE_2, MODE_3, OFF};
LightModes previousLightMode = ON;
LightModes currentLightMode = ON;

constexpr uint8_t patternAmmount = 3;
constexpr uint8_t patternLength = 10;
constexpr uint8_t patternSize = 4; // DO NOT CHANGE (we only have 4 leds)
constexpr bool blinkON[] = {1,1,1,1};
constexpr bool blinkOFF[] = {0,0,0,0};
constexpr bool blinkPatterns[patternAmmount][patternLength][patternSize] = {
  { {1, 1, 0, 0},
    {0, 0, 1, 1},
    {1, 1, 0, 0},
    {0, 0, 1, 1},
    {1, 1, 0, 0},
    {0, 0, 1, 1},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0}},

  { {1, 1, 0, 0},
    {0, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0}},

  { {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
    {1, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 1, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 1}},
};


// // put function declarations here:
void lights();
void lightsCheckChange();
void lightUp(const bool patternRow[]);

void interruptButtonPressed();

void measureLoop();


void setup() {
  Serial.begin(115200);


  pinMode(LED_OUT, OUTPUT); // Ініціюєм зелений, щоб показував чи працює схема
  digitalWrite(LED_OUT, HIGH);

  pinMode(LED_RED1, OUTPUT);
  pinMode(LED_RED2, OUTPUT);
  pinMode(LED_BLUE1, OUTPUT);
  pinMode(LED_BLUE2, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), interruptButtonPressed, FALLING);
}

void loop() {
  lightsCheckChange();
  lights();

  measureLoop();
  // bool buttonState = digitalRead(BUTTON_PIN);
  // // Serial.printf("Button State = %d \n", buttonState);

}




void lightsCheckChange() {
  if (!buttonPressed) // No need to change Lights
    return;
  // Button pressed, changing light mode
  
  if (currentLightMode == OFF){ // It is the final one
    currentLightMode = ON; // Go to the start
  }
  else {
    currentLightMode = static_cast<LightModes>(static_cast<int>(currentLightMode) + 1);
  }

  buttonPressed = false; // Releasing button
  
}

void lights(){
  static const uint32_t speed = 400;
  static uint32_t lastPatternShift = 0;

  static uint8_t currentPatternPosition = 0;
  static uint8_t currentPattern = 0;
  uint32_t now = millis();

  if (lastPatternShift + speed > now)
    return; // Too soon to do pattern shift;

  switch (currentLightMode)
  {
  case OFF:
    lightUp(blinkOFF);
    break;
  case ON:
    lightUp(blinkON);
    break;

  default: // Some pattern mode
    // Check if using different pattern
    if (currentPattern != currentLightMode-1){ 
      currentPattern = currentLightMode-1;
      currentPatternPosition = 0;
    }

    lightUp(blinkPatterns[currentPattern][currentPatternPosition]);
    // Serial.printf("\nCurrent pattern %d n pos %d",currentPattern, currentPatternPosition);
    break;
  }

  // Doing pattern shift
  lastPatternShift = now;
  currentPatternPosition++;
  if (currentPatternPosition >= patternLength) {
    currentPatternPosition = 0;
  }

}


void lightUp(const bool patternRow[]){
  digitalWrite(LED_RED1, patternRow[0]);
  digitalWrite(LED_RED2, patternRow[1]);
  digitalWrite(LED_BLUE1, patternRow[2]);
  digitalWrite(LED_BLUE2, patternRow[3]);
}






void interruptButtonPressed(){
  static const uint32_t buttonDebounceTime = 300;
  static uint32_t lastTimeButtonPressed = 0;

  // No need to do further checks 
  // cuz we didnt release the button in main program
  if (buttonPressed)
    return; 


  uint32_t now = millis();
  if (lastTimeButtonPressed + buttonDebounceTime < now)
  { // Check debounce, and then trigger logic
    // Serial.printf("\nButton pressed");
    lastTimeButtonPressed = now;
    buttonPressed = true;
  } else {
    // Serial.printf("\nButton DEBOUNCE TRIGGERED");
  }
  
}


void measureLoop(){
  static const uint32_t displayDelay = 1000;
  static uint32_t lastDisplay = 0;
  static uint32_t loops = 0;
  uint32_t now = millis();

  loops++; // Measuring loops

  if (lastDisplay + displayDelay > now)
    return;// Too soon to display
  
  Serial.printf("\nLPS = %d; Average exec time: %f ms", loops, ((float)(now - lastDisplay)/loops));

  lastDisplay = now;
  loops = 0;
}

#pragma endregion

