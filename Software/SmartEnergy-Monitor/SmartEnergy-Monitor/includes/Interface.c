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
volatile uint8_t Interface_buttonState = 0;
volatile uint8_t Interface_ledState = 0;
volatile bool Interface_ledIsOn = false;
volatile uint8_t Interface_buttonDebounceState = 0; // 0: Not pressed, 1: Pressed, 2: Released

// Declare locals.
static uint64_t ledDelayStart = 0;

void Interface_init()
{
	DDRB |= (1 << DDB5); // Port B5 as output.
	PORTB |= (1 << PORTB5); // Enable pullup resistor on B5.

	DDRB &= ~(1 << DDB7); // Port B7 as input.
	PORTB |= (1 << PORTB7); // Enable pullup resistor on B7.

	Interface_ledOff(); // Start with LED off.
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
	uint64_t delay = System_getTimeMilli() - ledDelayStart; // Get the current delay value.
	if (Interface_ledIsOn)
	{
		// If LED is on, hold for 'hold'  before resetting the delay start time.
		if (delay >= hold)
		{
			ledDelayStart = System_getTimeMilli();
			Interface_ledOff();
		}
	}
	else {
		// If LED is off, hold for 'hold' before resetting the delay start time.
		if (delay >= off)
		{
			ledDelayStart = System_getTimeMilli();
			Interface_ledOn();
		}
	}
}

void Interface_runStateMachine()
{
	// Controls LED blinking.
	switch (Interface_ledState)
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

	// Button debouncing technique
	if (Interface_buttonIsPressed)
	{
		Interface_buttonDebounceState = 1;
	}
	else if (Interface_buttonDebounceState == 1)
	{
		Interface_buttonDebounceState = 2;
	}
}
