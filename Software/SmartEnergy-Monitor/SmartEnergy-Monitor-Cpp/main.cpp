/*
* SmartEnergy-Monitor.c
* Code for the wireless energy monitor. Odd parity is enabled for transmission.
* Group 10
*
* Created: 12/08/2016 12:38:02 PM
* Author : Jerry Fan
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "includes/Interface.h"
#include "includes/Display.h"
#include "includes/Heap.h"
#include "includes/System.h"
#include "includes/Signal.h"
#include "includes/Power.h"

#include <avr/io.h>

using namespace System;

Power *power = new Power();

int main(void)
{
	init();
	for(;;)
	{
		power->processData();
	}
}