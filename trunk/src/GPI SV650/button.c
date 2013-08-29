/*
 * button.c
 *
 * Created: 24/08/2013 9:24:54 PM
 *  Author: Racine
 */ 
#include "main.h"
#include "button.h"
#include <avr/io.h>
#include <util/delay.h>

int button_is_pressed()
{
	// the button is pressed when BUTTON_BIT is clear
	if (bit_is_clear(BUTTON_PIN, BUTTON_BIT))
	{
		_delay_ms(DEBOUNCE_TIME);
		if (bit_is_clear(BUTTON_PIN, BUTTON_BIT)){
			return 1;
		}
	}
	return 0;
}
