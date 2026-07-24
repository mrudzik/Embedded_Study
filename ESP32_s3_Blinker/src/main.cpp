#include <Arduino.h>
















// #define ADC_PIN 4

// void setup() {
//   // pinMode(ADC_PIN, INPUT);
//   Serial.begin(115200);
//   analogReadResolution(12);
//   analogSetPinAttenuation(ADC_PIN, ADC_11db);
// }

// void loop() {
//   int raw = analogRead(ADC_PIN);
//   float volt = raw * 3.3 / 4096 ;

//   Serial.printf("Raw: %d, Voltage: %.3f\n", raw, volt);
//   delay(500);
// }

#pragma region Homework_1_4

// Домашка 1.4

// // Урок з кнопками
#define BUTTON_BOOT_PIN 0
#define BUTTON_EXTERNAL_PIN 15
#define LED_1 4
#define LED_2 5

const int debounceDelay = 50; // in miliseconds
int blinkSpeed = 100;


struct buttonData {
  int buttonPin = -1;
  bool currentState = false;
  bool lastState = false;
  unsigned long lastDebounceTime = 0;

  void initButtonData(int newButtonPin){
    buttonPin = newButtonPin;
    currentState = digitalRead(buttonPin);
    lastState = currentState;
  }

  bool checkStateChange() {
    if (buttonPin < 0) return false; // Not initialized

    int reading = digitalRead(buttonPin);
    // Якщо змінився стан кнопки
    if (reading != lastState) {
      lastDebounceTime = millis(); // Оновлюєм час останньої зміни
    }

    // Якщо пройшло достатньо часу після зміни стану
    if ((millis() - lastDebounceTime) > debounceDelay) {
      // Якщо стан змінився, оновлюємо поточний стан кнопки
      if (reading != currentState) {
        currentState = reading;

        lastState = currentState;
        return true; // State changed
      }
    }

    lastState = reading;
    return false; // State not changed
  }
};

buttonData buttonBOOT;
buttonData buttonEXTERNAL;


void setup(){
  Serial.begin(115200);

  pinMode(LED_1, OUTPUT); 
  pinMode(LED_2, OUTPUT);
  pinMode(BUTTON_BOOT_PIN, INPUT);
  pinMode(BUTTON_EXTERNAL_PIN, INPUT);

  buttonBOOT.initButtonData(BUTTON_BOOT_PIN);
  buttonEXTERNAL.initButtonData(BUTTON_EXTERNAL_PIN);

}


void checkActiveButtons(){
  Serial.printf("Button states BOOT - %d, EXTERNAL - %d\n",buttonBOOT.currentState, buttonEXTERNAL.currentState);

  if (!buttonBOOT.currentState && buttonEXTERNAL.currentState){ // Active only BOOT
    Serial.printf("BOOT mode\n");
    blinkSpeed = 2000;
  } else if (buttonBOOT.currentState && !buttonEXTERNAL.currentState){ // Active only EXTERNAL
    Serial.printf("EXTERNAL mode\n");
    blinkSpeed = 200;
  } else { // Active both
    Serial.printf("BOTH mode\n");
    blinkSpeed = 1000;
  }
}

void blink(){
  digitalWrite(LED_1, true);
  digitalWrite(LED_2, false);
  delay(blinkSpeed / 2);
  digitalWrite(LED_1, false);
  digitalWrite(LED_2, true);
  delay(blinkSpeed / 2);
}





void loop(){
    

    if (buttonBOOT.checkStateChange()){
      if (!buttonBOOT.currentState)
        checkActiveButtons();
    }

    if (buttonEXTERNAL.checkStateChange()){
      if (!buttonEXTERNAL.currentState)
        checkActiveButtons();
    }



  blink();
//   // bool state = digitalRead(BUTTON_PIN);
//   // if (!state) {
//   //   blink();
//   // }
//   // Serial.printf("BUTTON STATE: %d\n", state);
  delay(5);
}




#pragma endregion

#pragma region Homework_1_3

// Домашка 1.3

// // Мікроконтроллер видає 3.3 Вольта
// // Зелений світлодіод
// // Падіння 1.6-4.0 Вольта 
// //    — 7.7мА — резистор 220 Ом
// #define LED_OUT 4 // Schematic functionality

// // Червоні світлодіоди
// // Падіння 1.5-2.0 Вольта
// //    — 7.7мА — резистор 220 Ом
// #define LED_RED1 5
// #define LED_RED2 6

// // Сині світлодіоди
// // Падіння 2.5-3.7 Вольта
// //    — 7мА — резистор 100 Ом
// #define LED_BLUE1 15
// #define LED_BLUE2 16

// #define SPEED 500
// #define PAUSE 200


// // // put function declarations here:
// void lightUp(bool red1, bool red2, bool blue1, bool blue2, int waitTime, int pause);

// void blinkPattern1(int speed);
// void blinkPattern2(int speed);
// void blinkPattern3(int speed);


// void setup() {
//   pinMode(LED_OUT, OUTPUT); // Ініціюєм зелений, щоб показував чи працює схема
//   digitalWrite(LED_OUT, HIGH);

//   pinMode(LED_RED1, OUTPUT);
//   pinMode(LED_RED2, OUTPUT);
//   pinMode(LED_BLUE1, OUTPUT);
//   pinMode(LED_BLUE2, OUTPUT);
// }

// void loop() {
//   lightUp(0, 0, 0, 0, SPEED, 0);
  
//   blinkPattern1(SPEED);
//   blinkPattern1(SPEED);
//   blinkPattern1(SPEED);
//   lightUp(0, 0, 0, 0, SPEED, 0);

//   blinkPattern2(SPEED);
//   blinkPattern2(SPEED);
//   blinkPattern2(SPEED);
//   lightUp(0, 0, 0, 0, SPEED, 0);
  
//   blinkPattern3(SPEED);
//   blinkPattern3(SPEED);
//   blinkPattern3(SPEED);
// }




// void lightUp(bool red1, bool red2, bool blue1, bool blue2, int waitTime, int pause){
//   digitalWrite(LED_RED1, red1);
//   digitalWrite(LED_RED2, red2);
//   digitalWrite(LED_BLUE1, blue1);
//   digitalWrite(LED_BLUE2, blue2);
//   delay(waitTime);

//   if (pause > 0) {
//     digitalWrite(LED_RED1, LOW);
//     digitalWrite(LED_RED2, LOW);
//     digitalWrite(LED_BLUE1, LOW);
//     digitalWrite(LED_BLUE2, LOW);
//     delay(pause);
//   }
// }


// void blinkPattern1(int speed){
//   lightUp(1, 1, 0, 0, speed, PAUSE);
//   lightUp(0, 0, 1, 1, speed, PAUSE);
// }

// void blinkPattern2(int speed){
//   lightUp(1, 1, 0, 0, speed / 2, PAUSE);
//   lightUp(1, 1, 0, 0, speed / 2, PAUSE);

//   lightUp(0, 0, 1, 1, speed / 2, PAUSE);
//   lightUp(0, 0, 1, 1, speed / 2, PAUSE);  
// }

// void blinkPattern3(int speed){
//   lightUp(1, 0, 0, 0, speed, 0);
//   lightUp(0, 1, 0, 0, speed, 0);
//   lightUp(0, 0, 1, 0, speed, 0);
//   lightUp(0, 0, 0, 1, speed, 0);
//   lightUp(0, 0, 1, 0, speed, 0);
//   lightUp(0, 1, 0, 0, speed, 0);
  
// }

#pragma endregion

