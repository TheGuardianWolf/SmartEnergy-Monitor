/*
* Interface.c
*
* Created: 18/08/2016 6:47:54 AM
*  Author: Jerry Fan
*/

#include "Interface.h"
#include "System.h"

#include <stdbool.h>
#include <avr/io.h>

// Initialise globals.
volatile uint8_t Interface_state = 0;
volatile bool Interface_ledIsOn = false;

// Declare locals.
static uint64_t delayStart = 0;

void Interface_init()
{
	DDRB |= (1<<DDB5);
	PORTB |= (1<<PORTB5);

	DDRB &= ~(1<<DDB7);
	PORTB |= 1<<PORTB7;

	Interface_ledOff();
//
	//TCCR2A = 0;
	//TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
	//TIMSK2 = (1 << TOIE2);
	//TCNT2 = 0;
}

void Interface_ledOn()
{
	PORTB |= (1 << PORTB5);
	Interface_ledIsOn = true;
}

void Interface_ledOff()
{
	PORTB &= ~(1 << PORTB5);
	Interface_ledIsOn = false;
}

void Interface_ledToggle()
{
	PORTB ^= (1 << PORTB5);
}

bool Interface_buttonIsPressed()
{
	return !(PINB & (1 << PINB7));
}

void Interface_ledBlink(uint16_t off, uint16_t hold)
{
	uint64_t delay = System_getTimeMilli() - delayStart;
	if (Interface_ledIsOn)
	{
		if (delay >= hold)
		{
			delayStart = System_getTimeMilli();
			Interface_ledOff();
		}
	}
	else {
		if (delay >= off)
		{
			delayStart = System_getTimeMilli();
			Interface_ledOn();
		}
	}
}

void Interface_runStateMachine()
{
	switch (Interface_state)
	{
		case 0:
		Interface_ledBlink(900, 100);
		break;

		case 1:
		Interface_ledBlink(400, 100);
		break;

		case 2:
		Interface_ledBlink(233, 100);
		break;

		default:
		Interface_ledOn();
		break;
	}
}
			