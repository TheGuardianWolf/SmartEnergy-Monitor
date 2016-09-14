/*
 * System.h
 *
 * Created: 24/08/2016 6:22:22 PM
 *  Author: lichk
 */


#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>

/**
 * Initialise the system.
 * Turns on ADC, Display, Interface, and UART.
 */
void System_init();

/**
 * Initialise and reset system clock.
 * Enable timer1, set prescaler to 64, enable timer1 interrupt.
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
