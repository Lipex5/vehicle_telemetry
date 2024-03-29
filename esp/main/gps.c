#include "gps.h"

static const char *GPSTAG = "GPS";

int counter = 0;
float avg_vel_value = 0;

void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    gps_t *gps = NULL;
    switch (event_id) {
    case GPS_UPDATE:
        gps = (gps_t *)event_data;
        /* print information parsed from GPS statements */
        ESP_LOGI(GPSTAG, "%d/%d/%d %d:%d:%d => \r\n"
                 "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
                 "\t\t\t\t\t\tlongitude = %.05f°E\r\n"
                 "\t\t\t\t\t\taltitude   = %.02fm\r\n"
                 "\t\t\t\t\t\tspeed      = %fm/s",
                 gps->date.year + YEAR_BASE, gps->date.month, gps->date.day,
                 gps->tim.hour + TIME_ZONE, gps->tim.minute, gps->tim.second,
                 gps->latitude, gps->longitude, gps->altitude, gps->speed);
        
        if (counter == VEL_SAMPLE_N) counter = 0;
        avg_vel[counter] = gps->speed;
        avg_vel_value = get_avg_vel(avg_vel);
        counter++;

        int length = snprintf( NULL, 0, "%f,%f,%f,%f", gps->latitude, gps->longitude, gps->speed, avg_vel_value);
        char *str = malloc( length + 1 );
        snprintf( str, length + 1, "%f,%f,%f,%f", gps->latitude, gps->longitude, gps->speed, avg_vel_value);
        printf("GPS string: %s\n", str);

        esp_mqtt_client_publish(client, SENSOR_GPS_TOPIC, str, 0, 1, 0);

        break;
    case GPS_UNKNOWN:
        /* print unknown statements */
        ESP_LOGW(GPSTAG, "Unknown statement:%s", (char *)event_data);
        break;
    default:
        break;
    }
}

float get_avg_vel(float *avg_vel){
    float vel_sum = 0;
    for (int i = 0; i < VEL_SAMPLE_N; i++){
        vel_sum += avg_vel[i];
    }
    return vel_sum / VEL_SAMPLE_N;
}