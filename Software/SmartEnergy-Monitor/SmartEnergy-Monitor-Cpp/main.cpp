/*
* SmartEnergy-Monitor.c
* Code for the wireless energy monitor. Odd parity is enabled for transmission.
* Group 10
*
* Created: 12/08/2016 12:38:02 PM
* Author : Jerry Fan
*/

#define F_CPU 16000000UL

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#define USART0_RX_BUFFER_SIZE   0
#define USART0_TX_BUFFER_SIZE   4
#define ADC_BUFFER_SIZE 8
#define ADC_USE_INTERRUPTS true

#define clockCyclesPerMicrosecond()         ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds( a )      ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles( a )      ( (a) * clockCyclesPerMicrosecond() )

#include <util/atomic.h>
#include "lib/AVRTools/InitSystem.h"
#include "lib/AVRTools/SystemClock.h"
#include "lib/AVRTools/ArduinoPins.h"
#include "lib/AVRTools/Analog2Digital.h"
#include "lib/AVRTools/new.h"
#include "includes/A2D.h"
#include "includes/Interface.h"
#include "includes/Display.h"

#ifdef __USING_COMPARATOR__


#endif

void initialise()
{
	ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
	{
		Interface::init();
		A2D::init();
		Display::init();
		initSystemClock();
	}
}

Display* display = new Display;
Signal* voltage = new Signal(0);

int main(void)
{
	cli();

	initialise();
	static A2D::A2DData data;
	static unsigned long time;

	sei();

	while(true)
	{
		if (voltage->dataAvailable == 1)
		{
			//LED_ON;
			data = voltage->getData();
		}
		time = micros();
	}
}

ISR(INT0_vect)
{
	voltage->comparatorSetPeriod();
}