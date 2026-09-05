#include <Arduino.h>

const int PIN_ADC = 4;
const float VREF_NAIVE = 3.3f;

uint32_t t_prev = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_ADC, ADC_11db);
}

void loop() {
  uint32_t now = millis();
  if (now - t_prev >= 200) {          
    t_prev = now;

    int raw = analogRead(PIN_ADC);
    float v_naive = raw * VREF_NAIVE / 4095.0f;
    uint32_t mv   = analogReadMilliVolts(PIN_ADC);

    Serial.printf("raw=%4d  naive=%.3f V  calib=%.3f V  delta=%+.0f mV\n",
                  raw, v_naive, mv / 1000.0f, v_naive * 1000.0f - (float)mv);

    Serial.printf(">naive:%.3f\n>calib:%.3f\n", v_naive, mv / 1000.0f);
  }
}