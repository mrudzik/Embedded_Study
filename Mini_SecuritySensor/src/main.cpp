#include <Arduino.h>

// Pins
#define LED_GREEN 4
#define LED_RED 5
#define BUTTON_PIN 15
#define PHOTO_R_PIN 16


// Modes
#define MODE_DISABLED 0
#define MODE_ACTIVE 1
#define MODE_ALARM 2

int currentMode = MODE_DISABLED;

#define SENSOR_CHECK_FREQUENCY 500



void checkPhotoR();
void checkButton();
void displayCurrentMode();



void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT); 
  pinMode(LED_RED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Wiring up Photoresistor
  analogReadResolution(12);
  analogSetPinAttenuation(PHOTO_R_PIN, ADC_11db);
}

void loop() {
  checkPhotoR();
  checkButton();
  displayCurrentMode();

  delay(SENSOR_CHECK_FREQUENCY);

}




// put function definitions here:
void checkPhotoR() {
  if (currentMode != MODE_ACTIVE)
    return;

  int raw = analogRead(PHOTO_R_PIN);
  float volt = raw * 3.3 / 4096 ;

  Serial.printf("Raw: %d, Voltage: %.3f\n", raw, volt);
  if (raw < 2000)
    currentMode = MODE_ALARM;

}

bool pressedButton = false;

void checkButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading && !pressedButton) {
    pressedButton = true;

    if (currentMode == MODE_ALARM) {
      currentMode = MODE_DISABLED;
    } else if (currentMode == MODE_DISABLED) {
      currentMode = MODE_ACTIVE;
    } else if (currentMode == MODE_ACTIVE) {
      currentMode = MODE_DISABLED;
    }

    Serial.printf("Current Mode - %d\n", currentMode);
  }

  if (!reading) {
    pressedButton = false;
  }
}


void displayCurrentMode(){
  switch (currentMode)
  {
  case MODE_ACTIVE: // Checking for treaspassers
    digitalWrite(LED_GREEN, false);
    digitalWrite(LED_RED, true);
    delay(SENSOR_CHECK_FREQUENCY);
    digitalWrite(LED_RED, false);
    break;

  case MODE_ALARM: // ALARM active
    digitalWrite(LED_GREEN, false);
    digitalWrite(LED_RED, true);
    break;

  default: // MODE_DISABLED: // Its safe to go trough
    digitalWrite(LED_GREEN, true);
    digitalWrite(LED_RED, false);
    break;
  }
}
