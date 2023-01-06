#ifndef GPS_FILE
#define GPS_FILE

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nmea_parser.h"
#include "mqtt.h"

#define TIME_ZONE (+1)   // Time Zone
#define YEAR_BASE (2000) // date in GPS starts from 2000

void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

#endif