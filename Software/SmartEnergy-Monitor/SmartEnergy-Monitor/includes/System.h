/*
 * System.h
 *
 * Created: 24/08/2016 6:22:22 PM
 *  Author: lichk
 */


#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <avr/io.h>

void System_init();

void System_initClock();

uint32_t System_getTimeMicro();

uint32_t System_getTimeMilli();

#endif /* SYSTEM_H_ */
