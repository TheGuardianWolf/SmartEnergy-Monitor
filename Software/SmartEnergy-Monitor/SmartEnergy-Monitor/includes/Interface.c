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

	 Interface_ledOff();									      // start with the LED off
 }

 void Interface_ledOn()
 {
	 PORTB |= (1 << PORTB5);
 }

 void Interface_ledOff()
 {
	 PORTB &= ~(1 << PORTB5);
 }

 void Interface_ledToggle()
 {
	 PORTB ^= (1 << PORTB5);
 }

 bool Interface_buttonIsPressed()
 {
	 return !(PINB & (1 << PINB7));
 }