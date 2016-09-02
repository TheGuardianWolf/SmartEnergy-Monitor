/*
 * Interface.c
 *
 * Created: 18/08/2016 6:47:54 AM
 *  Author: Jerry Fan
 */

 #include "Interface.h"

 #include <avr/io.h>

 void Interface_init()
 {
	 DDRB	|= (1<<DDB5);						  // PB5 As Output pin
	 PORTB	|= (1<<DDB5);						// PB5 Activate internal pullUp resistor

	 DDRB	&= ~(1<<DDB7);						// PB1 As Input pin
	 PORTB	|= 1<<DDB7;							// PB1 Activate internal pullUp resistor

	 LED_OFF;									      // start with the LED off
 }
