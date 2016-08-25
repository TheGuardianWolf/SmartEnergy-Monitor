/*
 * Interactive.h
 *
 * Created: 18/08/2016 6:48:07 AM
 *  Author: Jerry Fan
 */


#ifndef INTERACTIVE_H_
#define INTERACTIVE_H_

#define LED pPin13
#define SWITCH pPin09
#define SWITCH_DOWN !getGpioPIN(SWITCH)
#define LED_ON setGpioPinHigh(LED)
#define LED_OFF setGpioPinLow(LED)

#include "lib/AVRTools/ArduinoPins.h"

/**
 * Initialise the IO registers for reading/writing
 */
void io_init();



#endif /* INTERACTIVE_H_ */
