#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include <driver/adc.h>
#include "driver/ledc.h"
#include "driver/gpio.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "nmea_parser.h"

#include "fileheaders.h"
#include "sensors.h"
#include "wifi.h"
// #include "gps.h"



#define TIME_ZONE (+1)   // Time Zone
#define YEAR_BASE (2000) // date in GPS starts from 2000



// Include for testing the stacks of the tasks
#ifndef INCLUDE_uxTaskGetStackHighWaterMark
#define INCLUDE_uxTaskGetStackHighWaterMark
#endif
#define TASK_STACK_SIZE 2500

#define STRING_BUFFER_SIZE 50

#define MQTT_BROKER_URI "mqtt://192.168.0.12:1885"

// GPIO Pins
#define BLINK_GPIO 2

#define LEDC_GPIO 23
#define LEDC_RESOLUTION 1024
#define LEDC_FREQ 1

static uint8_t s_led_state = 0;

static const char *TAG = "MQTT_TCP";

esp_mqtt_client_handle_t client;

// All MQTT topics to be subscribed
char *sub_topics[] = {
                        "blink_led",
                        "my_topic",
                        "sensor/temp"
                     };

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

void process_data(char *topic, char *data)
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
    configASSERT(((uint32_t)pvParameters) == 1);

    // Check stack
    UBaseType_t uxHighWaterMark;

    printf("vTaskTest started\n");
    mqtt_app_start();

    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

    // Infinite loop
    for (;;)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        printf("Stack left on vTaskTest: %d\n", uxHighWaterMark);
    }
}

void vTaskTest2(void *pvParameters)
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. */
    configASSERT(((uint32_t)pvParameters) == 1);

    // Check stack
    UBaseType_t uxHighWaterMark;

    printf("vTaskTest2 started\n");

    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

    init_sensor(ADC1_CHANNEL_6);

    // Infinite loop
    for (;;)
    {
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        char *reading = get_NTC_temp(ADC1_CHANNEL_6);
        printf("Temperature (C): %s\n", reading);
        esp_mqtt_client_publish(client, "sensor/temp", reading, 0, 1, 0);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        printf("Stack left on vTaskTest2: %d\n", uxHighWaterMark);
    }
}

void vLed1Hz(void *pvParameters)
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. */
    configASSERT(((uint32_t)pvParameters) == 1);

    ledc_timer_config_t ledpwmconfig = {
        // Configuração do timer

        .speed_mode = LEDC_LOW_SPEED_MODE,          // Modo de Velocidade -> LOW
        .duty_resolution = LEDC_TIMER_10_BIT,       // Resolução do do ciclo de trabalho (2^10 = 1024 valores)
        .timer_num = LEDC_TIMER_0,                  // Utilizado o TIMER 0
        .freq_hz = LEDC_FREQ,                       // Frequência de operação do sinal PWM
        .clk_cfg = LEDC_AUTO_CLK                    // Seleção automatica da fonte geradora do clock (interna ou externa)
    };
    ledc_timer_config(&ledpwmconfig);

    ledc_channel_config_t channel_LEDC = {
        .gpio_num = LEDC_GPIO,                      // Seleciona o pino para atuar o PWM
        .speed_mode = LEDC_LOW_SPEED_MODE,          // Modo de Velocidade -> LOW
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 50,
        .hpoint = 0
    };
    ledc_channel_config(&channel_LEDC);

    // Check stack
    UBaseType_t uxHighWaterMark;

    printf("vLed1Hz started\n");

    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

    // Infinite loop
    for (;;)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        printf("Stack left on vLed1Hz: %d\n", uxHighWaterMark);
    }
}

void vGPSTask(void *pvParameters)
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. */
    configASSERT(((uint32_t)pvParameters) == 1);

    // Check stack
    UBaseType_t uxHighWaterMark;

    printf("vGPSTask started\n");

    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

    // Infinite loop
    for (;;)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        printf("Stack left on vGPSTask: %d\n", uxHighWaterMark);
    }
}

void vTaskStarter(void)
{
    BaseType_t xReturned;
    TaskHandle_t xHandleTaskTest = NULL;
    TaskHandle_t xHandleTaskTest2 = NULL;
    TaskHandle_t xHandleLed1Hz = NULL;

    xReturned = xTaskCreate(vTaskTest, "Task Test ", TASK_STACK_SIZE, (void *)1, tskIDLE_PRIORITY, &xHandleTaskTest);
    configASSERT(xHandleTaskTest);

    if (xReturned != pdPASS)
    {
        printf("Error creating vTaskTest!\n");
        vTaskDelete(xHandleTaskTest);
    }

    xReturned = xTaskCreate(vTaskTest2, "Task Test 2", TASK_STACK_SIZE, (void *)1, tskIDLE_PRIORITY, &xHandleTaskTest2);
    configASSERT(xHandleTaskTest2);

    if (xReturned != pdPASS)
    {
        printf("Error creating vTaskTest2!\n");
        vTaskDelete(xHandleTaskTest2);
    }

    xReturned = xTaskCreate(vLed1Hz, "Task Test 2", TASK_STACK_SIZE, (void *)1, tskIDLE_PRIORITY, &xHandleLed1Hz);
    configASSERT(xHandleLed1Hz);

    if (xReturned != pdPASS)
    {
        printf("Error creating vTaskTest2!\n");
        vTaskDelete(xHandleLed1Hz);
    }
}

static void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    gps_t *gps = NULL;
    switch (event_id) {
    case GPS_UPDATE:
        gps = (gps_t *)event_data;
        /* print information parsed from GPS statements */
        ESP_LOGI(TAG, "%d/%d/%d %d:%d:%d => \r\n"
                 "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
                 "\t\t\t\t\t\tlongitude = %.05f°E\r\n"
                 "\t\t\t\t\t\taltitude   = %.02fm\r\n"
                 "\t\t\t\t\t\tspeed      = %fm/s",
                 gps->date.year + YEAR_BASE, gps->date.month, gps->date.day,
                 gps->tim.hour + TIME_ZONE, gps->tim.minute, gps->tim.second,
                 gps->latitude, gps->longitude, gps->altitude, gps->speed);
        break;
    case GPS_UNKNOWN:
        /* print unknown statements */
        ESP_LOGW(TAG, "Unknown statement:%s", (char *)event_data);
        break;
    default:
        break;
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

    /* NMEA parser configuration */
    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    /* init NMEA parser library */
    nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
    /* register event handler for NMEA parser library */
    nmea_parser_add_handler(nmea_hdl, gps_event_handler, NULL);

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // /* unregister event handler */
    // nmea_parser_remove_handler(nmea_hdl, gps_event_handler);
    // /* deinit NMEA parser library */
    // nmea_parser_deinit(nmea_hdl);
}