#include <Arduino.h>

#define LED_PIN 4
void led_blink()
{
  static uint32_t last_toogle_led = 0;
  static bool ledState = false;
  uint32_t now = millis();

  if (now - last_toogle_led >= 1000)
  {
    last_toogle_led = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}


void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  led_blink();
}