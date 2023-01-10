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

#define VEL_SAMPLE_N 20

static float avg_vel[VEL_SAMPLE_N] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// @brief Creates the handler to parse the GPS NMEA data. All the params are default to the nmea_parser library
void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

// @brief Calculates the mean velocity according to the values stored in param avg_vel
// @param *avg_vel (float) Array of velocities
// @return Returns the average of the summed velocities
float get_avg_vel(float *avg_vel);

#endif