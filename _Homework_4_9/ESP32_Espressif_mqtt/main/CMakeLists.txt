# idf_component_register(SRCS "main.c"
#                     INCLUDE_DIRS ".")

idf_component_register(SRCS "main.c" "wifi_connect.c"
                    INCLUDE_DIRS "."
                    REQUIRES mqtt esp_wifi esp_netif esp_event esp_driver_gpio nvs_flash)