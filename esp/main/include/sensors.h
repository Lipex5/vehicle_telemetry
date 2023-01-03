#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <driver/adc.h>

#define NO_OF_SAMPLES 64        // Multisampling

void init_sensor(adc_channel_t channel);
char *get_NTC_temp(adc_channel_t channel);