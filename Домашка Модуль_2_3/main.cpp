#include <Arduino.h>

#define LED_PIN1 15
#define LED_PIN2 16
#define LED_PIN3 17

constexpr uint32_t led1_freq = 200;
constexpr uint32_t led2_freq = 500;
constexpr uint32_t led3_freq = 1000;

uint32_t led1_lastTime = 0;
uint32_t led2_lastTime = 0;
uint32_t led3_lastTime = 0;


void checkLed(const uint32_t frequency, uint32_t *ledLast, uint32_t pin){
  uint32_t now = millis();
  if (now - *ledLast < frequency)
    return;
  
  *ledLast = now;
  digitalWrite(pin, !digitalRead(pin));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32-S3!");

  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
}

void loop() {

  checkLed(led1_freq, &led1_lastTime, LED_PIN1);
  checkLed(led2_freq, &led2_lastTime, LED_PIN2);
  checkLed(led3_freq, &led3_lastTime, LED_PIN3);

}
