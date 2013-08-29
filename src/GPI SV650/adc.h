/*
 * adc.h
 *
 * Created: 24/08/2013 9:43:06 PM
 *  Author: Racine
 */ 

#ifndef ADC_H_
#define ADC_H_

#define BATT_ADC_CH 0   // use VCC for AREF, use ADC0 for input (PA0), MUX bit 0:0:0:0:0
#define GEAR_ADC_CH 1	// use VCC for AREF, use ADC1 for input (PA1), MUX bit 0:0:0:0:0

// defines the number of times to take a sampling of voltages from the GPS switch higher numbers increase accurancy at the expense of speed
// both values must be powers of two
#define SAMPLE_SETS 16 //avg this many samples together

uint8_t AverageBattVoltage();
uint8_t AverageGearVoltage();

volatile uint8_t _gearSamples[SAMPLE_SETS];
volatile uint8_t _battSamples[SAMPLE_SETS];

#endif /* ADC_H_ */