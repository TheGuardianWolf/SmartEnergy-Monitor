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
	 DDRB	|= (1<<DDB5);
	 PORTB |= (1<<PORTB5);

	 DDRB	&= ~(1<<DDB7);
	 PORTB |= 1<<PORTB7;

	 Interface_ledOff();
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
