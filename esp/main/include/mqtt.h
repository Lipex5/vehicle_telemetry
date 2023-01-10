#ifndef MQTT_FILE
#define MQTT_FILE

#include "esp_system.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "sensors.h"

#define MQTT_BROKER_URI "mqtt://192.168.0.10:1885"
#define MQTT_BROKER_PORT 8883

#define STRING_BUFFER_SIZE 50

// MQTT Topics
#define BLINK_LED_TOPIC    "blink_led"
#define MY_TOPIC           "my_topic"
#define SENSOR_TEMP_TOPIC  "sensor/temp"
#define SENSOR_GPS_TOPIC   "sensor/gps"

esp_mqtt_client_handle_t client;

// All MQTT topics to be subscribed
static char *sub_topics[] = {
                        BLINK_LED_TOPIC,
                        MY_TOPIC,
                        SENSOR_TEMP_TOPIC,
                        SENSOR_GPS_TOPIC
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