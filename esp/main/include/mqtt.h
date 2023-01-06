#ifndef MQTT_FILE
#define MQTT_FILE

#include "esp_system.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "sensors.h"

#define MQTT_BROKER_URI "mqtt://192.168.31.112:1885"
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

esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void mqtt_app_start(void);
void process_mqtt_data(char *topic, char *data);

#endif