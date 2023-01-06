#include "mqtt.h"

static const char *TAG = "MQTT_TCP";

// All MQTT topics to be subscribed
char *sub_topics[] = {
                        "blink_led",
                        "my_topic",
                        "sensor/temp"
                        "sensor/gps"
                     };

// Returns the client
esp_mqtt_client_handle_t *get_mqtt_client()
{
    return &client;
}

// Handle the MQTT events
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        
        // loop to subscribe all the topics at once
        for (int i = 0; i < (sizeof(sub_topics) / sizeof(sub_topics[0])); i++){
            esp_mqtt_client_subscribe(client, sub_topics[i], 0);
        }

        // esp_mqtt_client_subscribe(client, "my_topic", 0);
        // esp_mqtt_client_subscribe(client, "blink_led", 0);
        esp_mqtt_client_publish(client, "my_topic", "Hi to all from ESP32 .........", 0, 1, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    // This runs every time the handler gets new data
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("\nTOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);

        // Gets the string from events and converts it to be read later
        char topic[STRING_BUFFER_SIZE];
        for (int i = 0; i < event->topic_len; i++)
        {
            topic[i] = *(event->topic++);
        }
        topic[event->topic_len] = '\0';

        char data[STRING_BUFFER_SIZE];
        for (int i = 0; i < event->data_len; i++)
        {
            data[i] = *(event->data++);
        }
        data[event->data_len] = '\0';

        process_mqtt_data(topic, data);

        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URI,
        .port = MQTT_BROKER_PORT,
        .username = MQTT_USERNAME,
        .password = MQTT_PASSWORD
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void process_mqtt_data(char *topic, char *data)
{
    printf("\ntopic=%s\r\n", topic);
    printf("data=%s\r\n", data);

    if (strcmp(topic, "blink_led") == 0)
    {
        if (strcmp(data, "2") == 0)
        {
            blink_led();
            s_led_state = !s_led_state;
            printf("Led state changed from %d to %d\n", !s_led_state, s_led_state);
        }
    }
}