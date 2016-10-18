/*
 * System.h
 *
 * Created: 24/08/2016 6:22:22 PM
 *  Author: Jerry Fan
 */


#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>

/**
 * Initialise and reset system clock to:
 * - Enable Timer 1
 * - Timer 1 prescaler to 64
 * - Enable Timer 1 interrupt
 */
void System_initClock();

/**
 * Get time in microseconds.
 * @return Time in microseconds.
 */
uint64_t System_getTimeMicro();

/**
 * Get time in milliseconds.
 * @return Time in milliseconds.
 */
uint64_t System_getTimeMilli();

#endif /* SYSTEM_H_ */
