#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "fileheaders.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "driver/gpio.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

// Include for testing the stacks of the tasks
#ifndef INCLUDE_uxTaskGetStackHighWaterMark
    #define INCLUDE_uxTaskGetStackHighWaterMark
#endif
#define TASK_STACK_SIZE 2500

#define STRING_BUFFER_SIZE 50

#define MQTT_BROKER_URI "mqtt://192.168.225.197:1885"

// Wifi Credentials
// Home
/*
#define SSID "NOWO-BF01"
#define PASS "BBA8637A187C5E7E"
*/

// Hotspot
#define SSID "POCO-X3-Pro"
#define PASS "teresa123"

// GPIO Pins
#define BLINK_GPIO 2

static uint8_t s_led_state = 0;

static const char *TAG = "MQTT_TCP";

// Handle the internet connection
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);    // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}

// Handle the MQTT events
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "my_topic", 0);
        esp_mqtt_client_subscribe(client, "blink_led", 0);
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
        for (int i = 0; i < event->topic_len; i++){
            topic[i] = *(event->topic++);
        }
        topic[event->topic_len] = '\0';

        char data[STRING_BUFFER_SIZE];
        for (int i = 0; i < event->data_len; i++){
            data[i] = *(event->data++);
        }
        data[event->data_len] = '\0';


        process_data(topic, data);

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

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URI,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void process_data(char* topic, char* data)
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

static void configure_leds(void)
{
    gpio_reset_pin(BLINK_GPIO);
    // Set the GPIO as a push/pull output
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

static void blink_led(void)
{
    // Set the GPIO level according to the state (LOW or HIGH)
    gpio_set_level(BLINK_GPIO, s_led_state);
}
 
void vTaskTest(void *pvParameters)
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. */
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    // Check stack
    UBaseType_t uxHighWaterMark;

    printf("vTaskTest started\n");
    mqtt_app_start();

    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

    // Infinite loop
    for (;;)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
        printf("Stack left on vTaskTest: %d\n", uxHighWaterMark);
    }
}

void vTaskTest2(void *pvParameters)
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. */
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    // Check stack
    UBaseType_t uxHighWaterMark;

    printf("vTaskTest2 started\n");

    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

    // Infinite loop
    for (;;)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
        printf("Stack left on vTaskTest2: %d\n", uxHighWaterMark);
    }
}


void vTaskStarter(void)
{
    BaseType_t xReturned;
    TaskHandle_t xHandleTaskTest = NULL;
    TaskHandle_t xHandleTaskTest2 = NULL;

    xReturned = xTaskCreate( vTaskTest, "Task Test ", TASK_STACK_SIZE, ( void * ) 1, tskIDLE_PRIORITY, &xHandleTaskTest );
    configASSERT(xHandleTaskTest);

    if( xReturned != pdPASS )
    {
        printf("Error creating vTaskTest!\n");
        vTaskDelete( xHandleTaskTest );
    }

    xReturned = xTaskCreate( vTaskTest2, "Task Test 2", TASK_STACK_SIZE, ( void * ) 1, tskIDLE_PRIORITY, &xHandleTaskTest2 );
    configASSERT(xHandleTaskTest2);

    if( xReturned != pdPASS )
    {
        printf("Error creating vTaskTest2!\n");
        vTaskDelete( xHandleTaskTest2 );
    }
}

void app_main(void)
{
    nvs_flash_init();
    wifi_connection();
    configure_leds();

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n");

    vTaskStarter();
    //mqtt_app_start();

}