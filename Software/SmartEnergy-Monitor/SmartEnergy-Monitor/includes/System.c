/*
* System.c
*
* Created: 24/08/2016 6:22:34 PM
*  Author: lichk
*/

#include "System.h"
#include "ADC.h"
#include "UART.h"
#include "Display.h"
#include "Interface.h"

#include <avr/interrupt.h>
#include <util/atomic.h>

#define clockCyclesPerMicro F_CPU / 1000000

static uint32_t clockOverflowCount = 0;
static const uint8_t clockScaleFactorMicro = clockCyclesPerMicro / 8;
static const uint16_t clockScaleFactorMilli = (clockCyclesPerMicro / 8) * 1000;

void System_init()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		System_initClock();
		ADC_init();
		//ADC_initComparators();
		Interface_init();
		UART_init();
		Display_init();
	}
}

void System_initClock()
{
	TCCR1A = 0;
	TCCR1B = (1 << CS11);
	TIMSK1 = (1 << TOIE1);
	TCNT1  = 0;
}

void System_resetClock()
{
	TCNT1 = 0;
}

uint32_t System_getTimeMicro()
{
	uint32_t time;
	NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
	{
	}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		time = (((uint32_t)TCNT1 + clockOverflowCount * ((uint32_t)((uint16_t) -1) + 1)) / clockScaleFactorMicro);
	}
	return time;
}

uint32_t System_getTimeMilli()
{
	uint32_t time;
	NONATOMIC_BLOCK(NONATOMIC_FORCEOFF)
	{
		(void) time;
	}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		time = (((uint32_t)TCNT1 + clockOverflowCount * ((uint32_t)((uint16_t) -1) + 1)) * clockScaleFactorMilli);
	}
	return time;
}

ISR( TIMER1_OVF_vect )
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		clockOverflowCount++;
	}
}
