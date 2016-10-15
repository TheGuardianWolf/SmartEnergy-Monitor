/*
 * Interactive.h
 *
 * Created: 18/08/2016 6:48:07 AM
 *  Author: Jerry Fan
 */


#ifndef INTERACTIVE_H_
#define INTERACTIVE_H_

#include <stdbool.h>
#include <stdint.h>

// Declare globals.
volatile uint8_t Interface_ledState;
volatile bool Interface_ledIsOn;
volatile bool Interface_ledIsOn;
volatile uint8_t Interface_buttonDebounceState; // 0: Not pressed, 1: Pressed, 2: Released

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

/**
 * An LED blink is composed of 2 components, the on time, and the off time.
 * A short blink has a short on time, whilst a long blink has a long on time.
 * To calculate the blink timing, use blink interval - hold on time = off.
 * @param off  Delay to hold LED off.
 * @param hold Delay to hold LED on.
 */
void Interface_ledBlink(uint16_t off, uint16_t hold);

/**
 * State machine intended to be run in the main loop. Controls all Interface
 * outputs.
 */
void Interface_runStateMachine();
#endif /* INTERACTIVE_H_ */
