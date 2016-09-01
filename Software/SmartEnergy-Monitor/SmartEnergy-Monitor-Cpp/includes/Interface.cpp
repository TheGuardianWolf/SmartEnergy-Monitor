/*
 * Interface.c
 *
 * Created: 18/08/2016 6:47:54 AM
 *  Author: Jerry Fan
 */

 #include "Interface.h"
 
 #include <avr/io.h>

 Interface::Interface() 
 {
	DDRB |= (1 << DDB5);
	PORTB |= (1 << DDB5);

	DDRB &= ~(1 << DDB7);
	PORTB |= 1 << DDB7;

	this->ledOff();
 }

void Interface::ledOn()
{
	PORTB |= (1 << PORTB5);
}

void Interface::ledOff()
{
	PORTB &= ~(1 << PORTB5);
}

void Interface::ledToggle()
{
	PORTB ^= (1 << PORTB5);
}

bool Interface::buttonIsPressed()
{
	return !(PINB & (1 << PINB7));
}