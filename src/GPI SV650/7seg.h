/*
 * 7seg.h
 *
 * Created: 29/07/2013 9:31:28 PM
 *  Author: Racine
 */ 
#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "Main.h"
#include <stdint.h>       // needed for uint8_t
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DISP_A_PORT		PORTB
#define DISP_A_PIN		_BV(PB2)
#define DISP_B_PORT		PORTA
#define DISP_B_PIN		_BV(PA2)
#define DISP_C_PORT		PORTA
#define DISP_C_PIN		_BV(PA3)
#define DISP_D_PORT		PORTA
#define DISP_D_PIN		_BV(PA4)
#define DISP_E_PORT		PORTA
#define DISP_E_PIN		_BV(PA5)
#define DISP_F_PORT		PORTA
#define DISP_F_PIN		_BV(PA6)
#define DISP_G_PORT		PORTA
#define DISP_G_PIN		_BV(PA7)
#define DISP_ALERT_PORT		PORTB
#define DISP_ALERT_PIN		_BV(PB1)

void SetDisplay(char ch);
void ClearDisplay();
void TestDisplay();

#endif /* DISPLAY_H_ */