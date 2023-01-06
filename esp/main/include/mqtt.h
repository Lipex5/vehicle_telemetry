#ifndef MQTT_FILE
#define MQTT_FILE

#include "esp_system.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "sensors.h"

#define MQTT_BROKER_URI "5a45431110f041eb8a03f94392303b13.s2.eu.hivemq.cloud"
#define MQTT_BROKER_PORT 8883

#define STRING_BUFFER_SIZE 50

// MQTT Credentials
#define MQTT_USERNAME "Lipex5"
#define MQTT_PASSWORD "PEIXEfilipe5"

static esp_mqtt_client_handle_t client;

esp_mqtt_client_handle_t *get_mqtt_client();
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void mqtt_app_start(void);
void process_mqtt_data(char *topic, char *data);

#endif