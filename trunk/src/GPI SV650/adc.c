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

uint8_t _gearPosIdx;
uint8_t _gearPosCnt;
uint16_t _gearPosSum;

uint8_t _battIdx;
uint8_t _battCnt;
uint16_t _battSum;

void ClearGearAverage()
{
	_gearPosCnt = 0;
	_gearPosIdx = 0;
	_gearPosSum = 0;
	for (int i=0; i< SAMPLE_SETS; i++) 
		_gearSamples[i] = 0;
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
      _battSum -= _battSamples[_battIdx];
      _battSamples[_battIdx] = newestVal;
      _battSum += _battSamples[_battIdx];
      _battIdx = (_battIdx + 1) % SAMPLE_SETS;
      if (_battCnt < SAMPLE_SETS) _battCnt++;
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
						
      _gearPosSum -= _gearSamples[_gearPosIdx];
      _gearSamples[_gearPosIdx] = newestVal;
      _gearPosSum += _gearSamples[_gearPosIdx];
      _gearPosIdx = (_gearPosIdx + 1) % SAMPLE_SETS;
      if (_gearPosCnt < SAMPLE_SETS) _gearPosCnt++;
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
	if (_battCnt == 0) return 0;
	return _battSum / _battCnt;
}

uint8_t AverageGearVoltage()
{	
	if (_gearPosCnt == 0) return 0;
	return _gearPosSum / _gearPosCnt;
}
