# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

# idf_component_register(SRCS ${app_sources})

idf_component_register(SRCS "main.c" "wifi_connect.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_wifi esp_netif esp_event nvs_flash mqtt esp_driver_gpio)

# idf_component_register(SRCS "main.c" "wifi_connect.c"
#                     INCLUDE_DIRS ""
#                     REQUIRES mqtt)