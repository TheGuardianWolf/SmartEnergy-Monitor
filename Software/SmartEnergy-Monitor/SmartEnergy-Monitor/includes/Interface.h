/*
 * Interactive.h
 *
 * Created: 18/08/2016 6:48:07 AM
 *  Author: Jerry Fan
 */


#ifndef INTERACTIVE_H_
#define INTERACTIVE_H_

#include <stdbool.h>

/**
 * Initialise the IO registers for reading/writing.
 * Enable pullup resistors on Port B (5,7), set pins as output and input
 * respectively. Ensure LED is off.
 */
void Interface_init();

/**
 * Turn on the LED.
 */
void Interface_ledOn();

/**
 * Turn off the LED.
 */
void Interface_ledOff();

/**
 * Toggles the LED.
 */
void Interface_ledToggle();

/**
 * Returns the state of the button.
 * @return True if button is down.
 */
bool Interface_buttonIsPressed();

#endif /* INTERACTIVE_H_ */
