#ifndef MQTT_FILE
#define MQTT_FILE

#include "esp_system.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "sensors.h"

#define MQTT_BROKER_URI "mqtt://192.168.0.29:1885"
#define MQTT_BROKER_PORT 8883

#define STRING_BUFFER_SIZE 50

esp_mqtt_client_handle_t client;

// All MQTT topics to be subscribed
static char *sub_topics[] = {
                        "blink_led",
                        "my_topic",
                        "sensor/temp"
                        "sensor/gps"
                     };

// @brief Handles the connections event received by MQTT
// @param event Default event handle
// @return ESP_OK on successfull execution
esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);

// @brief Call the handler function. All params default from ESP32 MQTT library
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

// @brief Starts the MQTT client with the configurations defined
void mqtt_app_start(void);

// @brief Parses MQTT to be used by the program.
// @param *topic String with the topic name
// @param *data String with the data from the subscibed topic
void process_mqtt_data(char *topic, char *data);

#endif