#include "sensors.h"

// Initializes the ADC of a sensor defined by channel, 12-bits, 11dB attenuation
// @param channel Channel to be read from
void init_sensor(adc_channel_t channel){
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, ADC_ATTEN_11db);
}

// Returns the temp begin read in the channel.
// @param channel Channel to be read from
temperature get_NTC_temp(adc_channel_t channel){
    temperature temp;

    uint32_t adc_reading = 0;

    int ThermistorPin = 34;
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

        temp.K = T;
        temp.C = Tc;
        temp.F = Tf;

        return temp;
    }
}