#ifndef SENSORS_FILE
#define SENSORS_FILE

#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <driver/adc.h>
#include "driver/gpio.h"


// GPIO Pins
#define BLINK_GPIO 2

#define LEDC_GPIO 23
#define LEDC_RESOLUTION 1024
#define LEDC_FREQ 1

#define NO_OF_SAMPLES 64        // Multisampling

static uint8_t s_led_state = 0;

void configure_leds(void);
void blink_led(void);
void init_sensor(adc_channel_t channel);
char *get_NTC_temp(adc_channel_t channel);

#endif