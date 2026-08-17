#include <Arduino.h>



#define PIN_IN_POTENTIAL 4
#define PIN_MOTOR_PWN 15
#define PIN_MOTOR_ON 16

constexpr uint32_t PWM_CHANNEL = 0;
constexpr uint32_t PWM_FREQ = 5000;
constexpr uint32_t PWM_RESOLUTION = 12;


void setup() {
  // Init variables
  // pinMode(PIN_IN_POTENTIAL, INPUT);
  // ledcAttachPin(PIN_MOTOR_PWN, MOTOR_FREQUENCY);
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PIN_MOTOR_PWN, PWM_CHANNEL);


  pinMode(PIN_MOTOR_ON, OUTPUT);
  digitalWrite(PIN_MOTOR_ON, true);

  Serial.begin(115200);
}



void loop() {
  // Читаєм данні з потенціометра
  // Налаштування duty cycle, відносно зчитанних данних
  int dutyCycle = analogRead(PIN_IN_POTENTIAL);
  
   

  // Формуєм PWM сигнал
  ledcWrite(PWM_CHANNEL, dutyCycle);
  Serial.printf("\nduty %d ;", dutyCycle);
}

