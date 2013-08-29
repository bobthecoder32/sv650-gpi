/*
 * 7seg.c
 *
 * Created: 28/07/2013 9:52:01 PM
 *  Author: Racine
 */ 

#include "7seg.h"

uint8_t _displayMask = 0b00000000;


void ClearDisplay()
{
	_displayMask = 0b00000000;
}

/*
	Takes a character argument as input, and sets the display mask to draw that 
	character on the seven segment display, the actual drawing is handled by the ISR on overflow Timer0
*/
void SetDisplay(char ch)
{
	switch(ch)
	{
		case '-':
			_displayMask = 0b10000000;		// Display '-'
			break;
		case 'n':
			_displayMask = 0b10101000;		// Display 'n'
			break;
		case '0':
			_displayMask = 0b01111110;		// Display Digit '0'
			break;
		case '1':
			_displayMask = 0b00001100;		// Display Digit '1'
			break;
		case '2':
			_displayMask = 0b10110110;		// Display Digit '2'
			break;
		case '3':
			_displayMask = 0b10011110;		// Display Digit '3'
			break;
		case '4':
			_displayMask = 0b11001100;		// Display Number 4
			break;
		case '5':
			_displayMask = 0b11011010;		// Display Number 5
			break;
		case '6':
			_displayMask = 0b11111010;		// Display Number 6
			break;
		//case '7':
			//_displayMask = 0b00001110;		// Display Number 7
			//break;
		//case '8':
			//_displayMask = 0b11111110;		// Display Number 8
			//break;
		//case '9':
			//_displayMask = 0b11001110;		// Display Number 9
			//break;
		case 'H':
			_displayMask = 0b11101100;		// Display H
			break;
		case 'L':
			_displayMask = 0b01110000;		// Display L
			break;
	}
}

/* Count the number of times the ISR has run */
volatile uint32_t ISRcounter = 0;

/*
	This chip doesn't come with a millis() function? overload the timer we have now for the display to perform this 
*/
uint32_t millis(void)
{
	uint32_t x;
	cli();
	x = ISRcounter/4;
	sei();
	return x;
}

/*
* To avoid undesired wiggle on the PWM, the ISR updates every channel's pin
* every time it runs. It should take the same number of cycles every time.
* The duty period of the PWM is the first section. Thus the LED goes ON then OFF.
*/
ISR(TIM0_OVF_vect)
{
	//determine which pins turn it is in this clock cycle
	int pinSlice = ISRcounter%8;
	ISRcounter++;
	
	if(pinSlice == 0 && (_displayMask & 0b00000010) == 0b00000010)
		DISP_A_PORT &= ~DISP_A_PIN; //Write the ch1 pin high
	else
		DISP_A_PORT |= DISP_A_PIN; //Write the ch1 pin low
	
	if(pinSlice == 1 && (_displayMask & 0b00000100) == 0b00000100)
		DISP_B_PORT &= ~DISP_B_PIN;
	else
		DISP_B_PORT |= DISP_B_PIN;
	
	if(pinSlice == 2 && (_displayMask & 0b00001000) == 0b00001000)
		DISP_C_PORT &= ~DISP_C_PIN;
	else
		DISP_C_PORT |= DISP_C_PIN;
	
	if(pinSlice == 3 && (_displayMask & 0b00010000) == 0b00010000)
		DISP_D_PORT &= ~DISP_D_PIN;
	else
		DISP_D_PORT |= DISP_D_PIN;
	
	if(pinSlice == 4 && (_displayMask & 0b00100000) == 0b00100000)
		DISP_E_PORT &= ~DISP_E_PIN;
	else
		DISP_E_PORT |= DISP_E_PIN;
	
	if(pinSlice == 5 && (_displayMask & 0b01000000) == 0b01000000)
		DISP_F_PORT &= ~DISP_F_PIN;
	else
		DISP_F_PORT |= DISP_F_PIN;
	
	if(pinSlice == 6 && (_displayMask & 0b10000000) == 0b10000000)
		DISP_G_PORT &= ~DISP_G_PIN;
	else
		DISP_G_PORT |= DISP_G_PIN;	
}

/*
	Light each digit of the display in turn, this is pretty and useful for debugging bad connections
*/
void TestDisplay()
{	
	_displayMask = 0b00000010;		
	_delay_ms(75);
	_displayMask = 0b00000100;
	_delay_ms(75);
	_displayMask = 0b00001000;
	_delay_ms(75);
	_displayMask = 0b00010000;
	_delay_ms(75);
	_displayMask = 0b00100000;
	_delay_ms(75);
	_displayMask = 0b01000000;
	_delay_ms(75);
	_displayMask = 0b10000000;
	_delay_ms(75);
}