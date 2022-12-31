#include <math.h>
#include <driver/adc.h>

#define NO_OF_SAMPLES 64        // Multisampling

typedef struct Temperature {
    int C;
    int F;
    int K;
}temperature;

void init_sensor(adc_channel_t channel);
uint32_t get_NTC_temp(adc_channel_t channel);