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

#include "driver/gpio.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "sensors.h"
#include "mqtt.h"
#include "wifi.h"
#include "gps.h"

// Include for testing the stacks of the tasks
#ifndef INCLUDE_uxTaskGetStackHighWaterMark
#define INCLUDE_uxTaskGetStackHighWaterMark
#endif

#define TASK_STACK_SIZE 2500

void vMQTTTask(void *pvParameters)
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. */
    configASSERT(((uint32_t)pvParameters) == 1);

    // Check stack
    UBaseType_t uxHighWaterMark;

    printf("vMQTTTask started\n");
    mqtt_app_start();

    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

    // Infinite loop
    for (;;)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        printf("Stack left on vMQTTTask: %d\n", uxHighWaterMark);
    }
}

void vTempTask(void *pvParameters)
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. */
    configASSERT(((uint32_t)pvParameters) == 1);

    // Check stack
    UBaseType_t uxHighWaterMark;

    printf("vTempTask started\n");

    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

    init_sensor(NTC_GPIO);

    // Infinite loop
    for (;;)
    {
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        char *reading = get_NTC_temp(NTC_GPIO);
        printf("Temperature (C): %s\n", reading);
        esp_mqtt_client_publish(client, SENSOR_TEMP_TOPIC, reading, 0, 1, 0);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        printf("Stack left on vTempTask: %d\n", uxHighWaterMark);
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
    TaskHandle_t xHandleMQTTTask = NULL;
    TaskHandle_t xHandleTempTask = NULL;
    TaskHandle_t xHandleGPSTask = NULL;

    xReturned = xTaskCreate(vMQTTTask, "MQTT Task", TASK_STACK_SIZE, (void *)1, tskIDLE_PRIORITY, &xHandleMQTTTask);
    configASSERT(xHandleMQTTTask);

    if (xReturned != pdPASS)
    {
        printf("Error creating vMQTTTask!\n");
        vTaskDelete(xHandleMQTTTask);
    }

    xReturned = xTaskCreate(vTempTask, "Task Test 2", TASK_STACK_SIZE, (void *)1, tskIDLE_PRIORITY, &xHandleTempTask);
    configASSERT(xHandleTempTask);

    if (xReturned != pdPASS)
    {
        printf("Error creating vTempTask!\n");
        vTaskDelete(xHandleTempTask);
    }

    xReturned = xTaskCreate(vGPSTask, "GPS Task", TASK_STACK_SIZE, (void *)1, tskIDLE_PRIORITY, &xHandleGPSTask);
    configASSERT(xHandleGPSTask);

    if (xReturned != pdPASS)
    {
        printf("Error creating GPS Task!\n");
        vTaskDelete(xHandleGPSTask);
    }
}


void app_main(void)
{
    nvs_flash_init();
    wifi_connection();
    configure_leds();
    init_1Hz_led();

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n");

    vTaskStarter();
}