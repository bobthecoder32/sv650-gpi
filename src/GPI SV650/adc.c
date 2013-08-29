/*
 * adc.c
 *
 * Created: 24/08/2013 9:19:17 PM
 *  Author: Racine
 */ 
#include "Main.h"
#include "adc.h"
#include <avr/io.h>
#include <avr/interrupt.h>


volatile uint8_t samplesReadCounter = 0;
volatile uint8_t channel = 0;

uint8_t _idx;
uint8_t _cnt;
int _sum;

void ClearGearAverage()
{
	_cnt = 0;
	_idx = 0;
	_sum = 0;
	for (int i=0; i< SAMPLE_SETS; i++) 
		_gearSamples[i] = 0;
}
void AddValue(uint8_t newval)
{
	_sum -= _gearSamples[_idx];
	_gearSamples[_idx] = newval;
	_sum += _gearSamples[_idx];
	_idx = (_idx + 1) % SAMPLE_SETS;
	if (_cnt < SAMPLE_SETS) _cnt++;
}

//interrupt service routine for ADC
// here we read the value as 8 bit precision and then copy that value to a memory location
ISR(ADC_vect)
{
	samplesReadCounter++;

	//only want the high val ... don't need 10bit precision here
	uint8_t newestVal = ADCH;

	if(channel == BATT_ADC_CH)
	{
		_battSamples[samplesReadCounter-1] = newestVal;
	}
	else
	{
		// for some reason the bike doesn't maintain the gear voltage when the engine is not running
		// in this case we get a voltage level for one second followed by ground.. to work around this we throw away any readings that
		// are below a threshold
		if(newestVal > MIN_GEAR_LEVEL_THRESHOLD)
		{
			//if levels between gears has changed radically then retake the average from scratch, this should keep us from
			// gradually floating through the intervening space between gears in our averages
			// this will have the effect of making the level changes more integer like
		
			uint8_t currAvg = AverageGearVoltage();
			if ((currAvg > newestVal && (currAvg-newestVal) > MAX_GEAR_LEVEL_TRANSIENT) || 
				(newestVal > currAvg && (newestVal-currAvg) > MAX_GEAR_LEVEL_TRANSIENT))
				ClearGearAverage();
			//_gearSamples[samplesReadCounter-1] = newestVal;				
			AddValue(newestVal);
		}
	}
	
	// test for a complete number of low samples to avg together
	if(samplesReadCounter == SAMPLE_SETS)
	{
		samplesReadCounter = 0;
		//select channel
		if(channel == GEAR_ADC_CH)
		{
			channel = BATT_ADC_CH;
		}
		else
		{
			channel = GEAR_ADC_CH;
		}
		ADMUX = channel; //pick the channel to mux
	}
	
	ADCSRA |= (1<<ADSC); // start next conversion
}

uint8_t AverageBattVoltage()
{
	uint16_t total = 0;
	for(int i = 0; i < SAMPLE_SETS; i++)
	{
		total += _battSamples[i];
	}
	uint8_t avg = (total/SAMPLE_SETS);
	return avg;
}

uint8_t AverageGearVoltage()
{	
	if (_cnt == 0) return 0;
	return _sum / _cnt;
	//uint16_t total = 0;
	//for(int i = 0; i < SAMPLE_SETS; i++)
	//{
		//total += _gearSamples[i];
	//}
	//uint8_t avg = (total/SAMPLE_SETS);
	//return avg;
}
