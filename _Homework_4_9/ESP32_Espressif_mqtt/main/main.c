#include <stdio.h>                  // printf(), snprintf()
#include <string.h>                 // memcmp()
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"         // vTaskDelay()
#include "freertos/event_groups.h" // Прапорець «MQTT з'єднано»
#include "driver/gpio.h"           // Світлодіод
#include "mqtt_client.h"           // MQTT-клієнт ESP-IDF
#include "nvs_flash.h"
#include "wifi_connect.h"          // Наша готова функція Wi-Fi

// 1. Визначаємо адресу брокера та Topic.
#define BROKER "mqtt://broker.emqx.io:1883"
#define TOPIC_PREFIX "emb15/mrudzik"
#define TOPIC_DATA   TOPIC_PREFIX "/telemetry"
#define TOPIC_CMD    TOPIC_PREFIX "/cmd"
#define LED_PIN GPIO_NUM_2

static esp_mqtt_client_handle_t mqtt; // Посилання на MQTT-клієнт.
static EventGroupHandle_t flags;      // Біт BIT0 = MQTT підключений.

// 2. Цю функцію викликає бібліотека, коли відбувається MQTT-подія.
static void mqtt_handler(void *arg, esp_event_base_t base,
                         int32_t id, void *data)
{
    esp_mqtt_event_handle_t event = data; // Дані поточної події.

    switch (id) {
        case MQTT_EVENT_CONNECTED:
            xEventGroupSetBits(flags, BIT0);   // З'єднання активне.
            // Підписуємось на команди щоразу після підключення.
            esp_mqtt_client_subscribe(mqtt, TOPIC_CMD, 0); // QoS 0

            esp_mqtt_client_publish(mqtt,TOPIC_PREFIX "/status","online",0, 1, 1);
            
            printf("MQTT connected\n");
            break;

        case MQTT_EVENT_DISCONNECTED:
            xEventGroupClearBits(flags, BIT0); // З'єднання втрачено.
            printf("MQTT disconnected\n");
            break;

        case MQTT_EVENT_DATA:
            // MQTT передає байти; data не завжди закінчується '\0'.
            // Тут очікуємо тільки короткі команди ON і OFF.
            if (event->data_len == 2 && memcmp(event->data, "ON", 2) == 0) {
                gpio_set_level(LED_PIN, 1);
                printf("LED ON\n");
            }
            else if (event->data_len == 3 &&
                     memcmp(event->data, "OFF", 3) == 0) {
                gpio_set_level(LED_PIN, 0);
                printf("LED OFF\n");
            }
            break;

        default:
            break; // Інші події поки пропускаємо.
    }
}

// 3. Точка входу: ініціалізація, а потім публікація раз на 3 с.
void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init()); // NVS потрібна Wi-Fi.
    flags = xEventGroupCreate();

    gpio_reset_pin(LED_PIN);
    // Вхід + вихід: так gpio_get_level() може читати рівень на LED_PIN.
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_level(LED_PIN, 0);

    wifi_connect(); // Готова функція: підключається й чекає отримання IP.

    // Налаштування MQTT-клієнта. Без TLS — тільки лабораторна робота.
    esp_mqtt_client_config_t config = {
        .broker.address.uri = BROKER,

        
    .session.last_will.topic = TOPIC_PREFIX "/status",
    .session.last_will.msg = "offline",
    .session.last_will.qos = 1,
    .session.last_will.retain = 1,

    // Keep Alive: 15 секунд
    .session.keepalive = 15
    };
    mqtt = esp_mqtt_client_init(&config); // Створити клієнта.
    esp_mqtt_client_register_event(mqtt, ESP_EVENT_ANY_ID,
                                   mqtt_handler, NULL); // Призначити callback.
    esp_mqtt_client_start(mqtt);          // Почати MQTT-підключення.
    printf("Topic prefix: %s\n", TOPIC_PREFIX);

    unsigned counter = 0;
    char message[64];

    while (1) {
        if (xEventGroupGetBits(flags) & BIT0) { // MQTT працює?
            // Пакуємо лічильник і стан LED у JSON.
            snprintf(message, sizeof(message),
                     "{\"counter\":%u,\"led\":%s}", ++counter,
                     gpio_get_level(LED_PIN) ? "true" : "false");

            // PUBLISH(client, topic, data, length, qos, retain).
            esp_mqtt_client_publish(mqtt, TOPIC_DATA, message, 0, 0, 0);
          
            printf("Published: %s\n", message);
        }
        vTaskDelay(pdMS_TO_TICKS(3000)); // Пауза 3 секунди.
    }
}