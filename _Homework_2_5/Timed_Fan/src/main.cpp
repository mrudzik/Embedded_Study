#include <Arduino.h>





hw_timer_t * fanTimer = NULL;
hw_timer_t * enableTimer = NULL;

volatile bool isFaning = false;

constexpr uint32_t FAN_PIN = 13;

constexpr uint32_t TIMER_DIVIDER = 80;
constexpr uint32_t FAN_WORKTIME = 3 * 1000000;
constexpr uint32_t FAN_RESTTIME = 6 * 1000000;

void IRAM_ATTR onFanTimer() {
  // Пора виключати фен
  isFaning = false;
  digitalWrite(FAN_PIN, isFaning);

  // // Виключає себе та включає наступний таймер
  // timerAlarmDisable(fanTimer); 
  // timerAlarmEnable(enableTimer);
}

void IRAM_ATTR onEnableTimer() {
  // Пора включати фен
  isFaning = true;
  digitalWrite(FAN_PIN, isFaning);

  // // Виключає себе та включає наступний таймер
  // timerAlarmDisable(enableTimer); 
  // timerAlarmEnable(fanTimer); 

  // Хотів включати виключати таймери, але зробити так щось не виходить, мигає ледь ледь.
}

void setup() {
  Serial.begin(115200);
  pinMode(FAN_PIN, OUTPUT);

  
  fanTimer = timerBegin(0, TIMER_DIVIDER, true);
  // Мотор буде працювати 3 секунди
  timerAttachInterrupt(fanTimer, &onFanTimer, true); // Приєднання функції обробки переривання
  timerAlarmWrite(fanTimer, FAN_WORKTIME, true); // Встановлення значення для переривання


  enableTimer = timerBegin(1, TIMER_DIVIDER, true);
  // Включим через 6 секунд
  timerAttachInterrupt(enableTimer, &onEnableTimer, true);
  timerAlarmWrite(enableTimer, FAN_RESTTIME + FAN_WORKTIME, true);
  

  timerAlarmEnable(enableTimer); // Запускаєм таймер включення.
  timerAlarmEnable(fanTimer);

}

void loop() {
  static bool lastKnownState = false;
  static uint32_t lastTimeStateChanged = 0;

  if (lastKnownState != isFaning) {
    uint32_t now = millis();
    
    Serial.printf("\nIs faning %d, time passed %d", isFaning, now - lastTimeStateChanged);
    lastKnownState = isFaning;
    lastTimeStateChanged = now;
  }
  
 
}