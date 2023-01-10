#include "sensors.h"

void configure_leds(void)
{
    for (int i = 0; i < (sizeof(led_pins) / sizeof(led_pins[0])); i++){
        gpio_reset_pin(led_pins[i]);
        // Set the GPIO as a push/pull output
        gpio_set_direction(led_pins[i], GPIO_MODE_OUTPUT);
    }
}

void blink_led(led *led)
{
    // Set the GPIO level according to the state (LOW or HIGH)
    gpio_set_level(led->pin, led->state);
    printf("Led State: %d\n", led->state);
    led->state = !led->state;
}

void init_sensor(adc_channel_t channel){
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, ADC_ATTEN_11db);
}

char *get_NTC_temp(adc_channel_t channel){

    uint32_t adc_reading = 0;

    double adcMax = 4095.0;        // ADC resolution 12-bit (0-4095)
    double Vs = 3.3;               // supply voltage

    double R1 = 10000.0;   // voltage divider resistor value
    double Beta = 3950.0;  // Beta value
    double To = 298.15;    // Temperature in Kelvin for 25 degree Celsius
    double Ro = 10000.0;   // Resistance of Thermistor at 25 degree Celsius
    
    double Vout, Rt = 0;
    double T, Tc, Tf = 0;

    while (1) {

        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        }

        adc_reading /= NO_OF_SAMPLES;

        Vout = adc_reading * Vs/adcMax;
        Rt = R1 * Vout / (Vs - Vout);

        T = 1/(1/To + log(Rt/Ro)/Beta);    // Temperature in Kelvin
        Tc = T - 273.15;                   // Celsius
        Tf = Tc * 9 / 5 + 32;              // Fahrenheit

        int length = snprintf( NULL, 0, "%f,%f,%f", Tc, Tf, T);
        char *str = malloc( length + 1 );
        snprintf( str, length + 1, "%.2f,%.2f,%.2f", Tc, Tf, T);
        printf("Temp string: %s\n", str);

        return str;
    }
}

void init_1Hz_led(void){
    ledc_timer_config_t ledpwmconfig = {
        // Timer configuration
        .speed_mode = LEDC_LOW_SPEED_MODE,          // Low Speed Mode
        .duty_resolution = LEDC_TIMER_10_BIT,       // Duty Resolution (2^10 = 1024)
        .timer_num = LEDC_TIMER_0,                  // TIMER 0
        .freq_hz = LEDC_FREQ,                       // PWM frequency
        .clk_cfg = LEDC_AUTO_CLK                    // Clock auto selection
    };
    ledc_timer_config(&ledpwmconfig);

    ledc_channel_config_t channel_LEDC = {
        .gpio_num = LEDC_GPIO,                      // PWM GPIO
        .speed_mode = LEDC_LOW_SPEED_MODE,          // Low Speed Mode
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 50,
        .hpoint = 0
    };
    ledc_channel_config(&channel_LEDC);
}