/*
 * System.cpp
 *
 * Created: 30/08/2016 11:11:17 PM
 *  Author: lichk
 */ 

 #include "System.h"

 #ifdef USING_ADC
 #include "A2D.h"
 #endif

 #ifdef USING_UART
 #include "UART.h"
 #endif

 #include <util/atomic.h>

 namespace
 {
	volatile uint32_t systemClockOverflowCount = 0;
	const uint8_t clockCyclesPerMicro = F_CPU / 1000000;
	const uint8_t clockScaleFactorMicro = clockCyclesPerMicro / 8;
	const uint16_t clockScaleFactorMilli = (clockCyclesPerMicro / 8) * 1000;
 }

 void System::init()
 {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{

		#ifdef USING_ADC
		A2D::init();
		#endif

		#ifdef USING_UART
		UART::init();
		#endif

		TCCR1A = 0;
		TCCR1B = (1 << CS11) | (1 << CS10);
		TIMSK1 = (1 << TOIE1);
		TCNT1  = 0;
	}	
	sei();
 }

 uint32_t System::getTimeMicro()
 {
	uint32_t time;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		time = (((uint32_t)TCNT1 + systemClockOverflowCount * ((uint32_t)((uint16_t) -1) + 1)) / clockScaleFactorMicro);
	}
	return time;
 }

 uint32_t System::getTimeMilli()
 {
	uint32_t time;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		time = (((uint32_t)TCNT1 + systemClockOverflowCount * ((uint32_t)((uint16_t) -1) + 1)) * clockScaleFactorMilli);
	}
	return time;
 }

 ISR( TIMER1_OVF_vect )
 {
	 systemClockOverflowCount++;
 }