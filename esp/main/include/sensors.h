#ifndef SENSORS_FILE
#define SENSORS_FILE

#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <driver/adc.h>
#include "driver/gpio.h"


// GPIO Pins
#define BLINK_GPIO 2            // Led by MQTT

#define LEDC_GPIO 23            // Led 1Hz
#define LEDC_RESOLUTION 1024
#define LEDC_FREQ 1

#define NTC_GPIO ADC1_CHANNEL_6

#define NO_OF_SAMPLES 64        // Multisampling

typedef struct {
    int pin;
    uint8_t state;
}led;

static led led_blink = {BLINK_GPIO, 1};

static int led_pins[] = {
                    BLINK_GPIO
                };

// @brief Configures the LED's to be used with the blink function
void configure_leds(void);

// @brief Switches the state of the LED's 
// @param *led gets the address of a led typedef so it can change the state
void blink_led(led *led);

// @brief Initializes the ADC of a sensor defined by channel, 12-bits, 11dB attenuation
// @param channel Channel to be read from
void init_sensor(adc_channel_t channel);

// Returns the temp being read in the channel.
// @param channel Channel to be read from
char *get_NTC_temp(adc_channel_t channel);

#endif