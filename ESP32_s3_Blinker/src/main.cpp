#include <Arduino.h>

// Мікроконтроллер видає 3.3 Вольта
// Зелений світлодіод
// Падіння 1.6-4.0 Вольта 
//    — 7.7мА — резистор 220 Ом
#define LED_OUT 4 // Schematic functionality

// Червоні світлодіоди
// Падіння 1.5-2.0 Вольта
//    — 7.7мА — резистор 220 Ом
#define LED_RED1 5
#define LED_RED2 6

// Сині світлодіоди
// Падіння 2.5-3.7 Вольта
//    — 7мА — резистор 100 Ом
#define LED_BLUE1 15
#define LED_BLUE2 16

#define SPEED 500
#define PAUSE 200

// // put function declarations here:
void lightUp(bool red1, bool red2, bool blue1, bool blue2, int waitTime, int pause);

void blinkPattern1(int speed);
void blinkPattern2(int speed);
void blinkPattern3(int speed);


void setup() {
  pinMode(LED_OUT, OUTPUT); // Ініціюєм зелений, щоб показував чи працює схема
  digitalWrite(LED_OUT, HIGH);

  pinMode(LED_RED1, OUTPUT);
  pinMode(LED_RED2, OUTPUT);
  pinMode(LED_BLUE1, OUTPUT);
  pinMode(LED_BLUE2, OUTPUT);
}

void loop() {
  lightUp(0, 0, 0, 0, SPEED, 0);
  
  blinkPattern1(SPEED);
  blinkPattern1(SPEED);
  blinkPattern1(SPEED);
  lightUp(0, 0, 0, 0, SPEED, 0);

  blinkPattern2(SPEED);
  blinkPattern2(SPEED);
  blinkPattern2(SPEED);
  lightUp(0, 0, 0, 0, SPEED, 0);
  
  blinkPattern3(SPEED);
  blinkPattern3(SPEED);
  blinkPattern3(SPEED);
}




void lightUp(bool red1, bool red2, bool blue1, bool blue2, int waitTime, int pause){
  digitalWrite(LED_RED1, red1);
  digitalWrite(LED_RED2, red2);
  digitalWrite(LED_BLUE1, blue1);
  digitalWrite(LED_BLUE2, blue2);
  delay(waitTime);

  if (pause > 0) {
    digitalWrite(LED_RED1, LOW);
    digitalWrite(LED_RED2, LOW);
    digitalWrite(LED_BLUE1, LOW);
    digitalWrite(LED_BLUE2, LOW);
    delay(pause);
  }
}


void blinkPattern1(int speed){
  lightUp(1, 1, 0, 0, speed, PAUSE);
  lightUp(0, 0, 1, 1, speed, PAUSE);
}

void blinkPattern2(int speed){
  lightUp(1, 1, 0, 0, speed / 2, PAUSE);
  lightUp(1, 1, 0, 0, speed / 2, PAUSE);

  lightUp(0, 0, 1, 1, speed / 2, PAUSE);
  lightUp(0, 0, 1, 1, speed / 2, PAUSE);  
}

void blinkPattern3(int speed){
  lightUp(1, 0, 0, 0, speed, 0);
  lightUp(0, 1, 0, 0, speed, 0);
  lightUp(0, 0, 1, 0, speed, 0);
  lightUp(0, 0, 0, 1, speed, 0);
  lightUp(0, 0, 1, 0, speed, 0);
  lightUp(0, 1, 0, 0, speed, 0);
  
}

