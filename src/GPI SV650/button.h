/*
 * button.h
 *
 * Created: 24/08/2013 9:25:25 PM
 *  Author: Racine
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_

#define DEBOUNCE_TIME 25        /* time to wait while "de-bouncing" button */

#define BUTTON_PORT PORTB       /* PORTx - register for button output */
#define BUTTON_PIN PINB         /* PINx - register for button input */
#define BUTTON_BIT PB0          /* bit for button input/output */

int button_is_pressed();

#endif /* BUTTON_H_ */