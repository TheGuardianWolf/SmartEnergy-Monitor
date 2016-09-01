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

// #define USING_COMPARATOR 1
#define USING_ADC 1
#define USING_UART 1

#include <util/atomic.h>
#include "includes/A2D.h"
#include "includes/Interface.h"
#include "includes/Display.h"
#include "lib/AVRTools/new.h"
#include "includes/UART.h"
#include "includes/System.h"
#include "includes/Signal.h"

int main(void)
{
	System::init();
	static A2D::A2DData data;
	static unsigned long time;
	Display* display = new Display;
	Signal* voltage = new Signal(0);
	while(true)
	{
		if (voltage->isDataAvailable() == 1)
		{
			data = voltage->getData();
		}
		time = System::getTimeMicro();
	}
}