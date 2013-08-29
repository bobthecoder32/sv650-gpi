/*
* Main.c
*
* Created: 16/07/2013 8:19:03 PM
*  Author: Racine
*/

#include "main.h"
#include "7seg.h"
#include "adc.h"
#include "button.h"
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//default the position to neutral on boot
volatile uint8_t _gearPosTested = INDETERMINATE;
volatile uint8_t _gearPosDisplay = INDETERMINATE;
uint32_t _transientGearStartMillis;


// Input a voltage level from 0-255 and get a gear back
//
uint8_t DetermineGearPos(uint8_t avgVal)
{
	if (avgVal >= SIXTH_LO_V && avgVal <= SIXTH_HI_V) 
	{
		return SIXTH;
	}

	if (avgVal >= FIFTH_LO_V && avgVal <= FIFTH_HI_V) 
	{
		return FIFTH;
	}

	if (avgVal >= FOURTH_LO_V && avgVal <= FOURTH_HI_V) 
	{
		return FOURTH;
	}
	
	if (avgVal >= THIRD_LO_V && avgVal <= THIRD_HI_V) 
	{
		return THIRD;
	}
	
	if (avgVal >= SECOND_LO_V && avgVal <= SECOND_HI_V) 
	{
		return SECOND; 
	}
	
	if (avgVal >= FIRST_LO_V && avgVal <= FIRST_HI_V) 
	{
		return FIRST;
	}
	
	if (avgVal >= NEUTRAL_LO_V && avgVal <= NEUTRAL_HI_V)
	{	
		return NEUTRAL;		
	}
	
	return INDETERMINATE;
}

void init()
{
	// set port direction registers high is OUT
	DDRA |= (1 << DDA2) | (1 << DDA3) | (1 << DDA4) | (1 << DDA5) | (1 << DDA6) | (1 << DDA7); 
	DDRB |= (1 << DDB2) | (1 << DDB1);
	
	PORTA |= DISP_B_PIN | DISP_C_PIN | DISP_D_PIN | DISP_E_PIN | DISP_F_PIN | DISP_G_PIN;//start high
	PORTB |= DISP_A_PIN | DISP_ALERT_PIN;

	BUTTON_PORT |= _BV(BUTTON_BIT); //turn on internal pull-up resistor for the switch

	//
	ADMUX = BATT_ADC_CH;
	ADCSRA =
		(1 << ADEN)  |     // Enable ADC
		(1 << ADSC)  |     // start ADC
		(0 << ADATE)  |    // disable auto triggering of ADC
		(0 << ADIF)  |     // clear ADC interrupt flag
		(1 << ADIE)  |     // Enable ADC interrupt
		(1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);    // set prescaler to /64 1:1:0
	ADCSRB = (1 << ADLAR);     // left shift result, only interested in 8 bit precision

	//this timer drives our display
	TCCR0B |= (1 << CS00);// disable timer pre-scale (=clock rate)
	TIMSK0 |= (1 << TOIE0); // enable timer overflow interrupt specifically
	
	//turn off unused parts
	power_usi_disable();
	power_timer1_disable();
}


int main(void)
{
	//initialize the chip registers
	init();

	// enable interrupt service routines, we need these before the display code 
	sei(); 

	// run the display through its paces
	TestDisplay();
	
	// blank the screen
	ClearDisplay();
	
	while(1)
	{
		uint8_t tempGear = _gearPosTested;
		uint8_t gearVoltage = AverageGearVoltage();
		_gearPosTested = DetermineGearPos(gearVoltage);
		
		// Only accept a gear as changed if it maintains a stability for a defined interval of time
		if (tempGear != _gearPosTested)
		{
			// mark the current time
			_transientGearStartMillis = millis();
		}
		else // Transient gear is the same since last time
		{
			uint32_t deltaTransientMillis = millis() - _transientGearStartMillis;
			// check if it was stable long enough
			if ( deltaTransientMillis >= MIN_TRANSIENTGEAR_INTERVAL )
			{
				_gearPosDisplay = _gearPosTested;
			}
		}

		//when the button is pressed then display voltages else default to gears
		if(button_is_pressed())
		{
			uint8_t battVoltage = AverageBattVoltage();			
			if(battVoltage > 230) // 14.5 zener shifts all volatage -10, so we're really testing for 13.2 min
				SetDisplay('H');
			else if(battVoltage < 160) //13.2
				SetDisplay('L');
			else
				SetDisplay('-');
		}
		else 
		{
			switch(_gearPosDisplay)
			{
				case INDETERMINATE:
				SetDisplay('-');
				break;
				case NEUTRAL:
				SetDisplay('n');
				break;
				case FIRST:
				SetDisplay('1');
				break;
				case SECOND:
				SetDisplay('2');
				break;
				case THIRD:
				SetDisplay('3');
				break;
				case FOURTH:
				SetDisplay('4');
				break;
				case FIFTH:
				SetDisplay('5');
				break;
				case SIXTH:
				SetDisplay('6');
				break;
			}
		}
		
		//delay to slow looping
		_delay_ms(DELAY_MS);
	}
	return 0;
}


