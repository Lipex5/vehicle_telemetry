#ifndef WIFI_FILE
#define WIFI_FILE

#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#define HOME

// Wifi Credentials
// Home
#ifdef HOME
#define SSID "NOWO-BF01"
#define PASS "BBA8637A187C5E7E"
#endif

// Hotspot
#ifdef PHONE
#define SSID "POCO-X3-Pro"
#define PASS "teresa123"
#endif

// @brief Handles the wifi events from the connection. All params default from the ESP32 WiFi library
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

// @ brief Starts the WiFi connection with the configurations defined
void wifi_connection();

#endif