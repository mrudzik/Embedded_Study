#pragma once

// Змінити перед прошивкою (лише навчальна Wi-Fi мережа).
#define WIFI_SSID "Host_15_2.4"
#define WIFI_PASS "rudzik2012"

// Ініціалізує Wi-Fi і чекає, поки ESP32 отримає IP-адресу.
void wifi_connect(void);