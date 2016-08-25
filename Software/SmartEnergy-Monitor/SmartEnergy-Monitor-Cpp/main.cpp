/*
 * SmartEnergy-Monitor.c
 * Code for the wireless energy monitor. Odd parity is enabled for transmission.
 * Group 10
 *
 * Created: 12/08/2016 12:38:02 PM
 * Author : Jerry Fan
 */

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include "lib/protothreads/Protothread.h"
#include "lib/AVRTools/InitSystem.h"
#include "lib/AVRTools/SystemClock.h"
#include "lib/AVRTools/ArduinoPins.h"
#include "lib/AVRTools/Analog2Digital.h"
#include "lib/AVRTools/new.h"
#include "includes/A2D.h"
#include "includes/Calc.h"
#include "includes/Interface.h"
#include "includes/Uart.h"
#include "includes/Display.h"

void initialise()
{
	Interface::init();
	A2D::init();
	Display::init();
}



int main(void)
{
	initialise();
	
	A2D* adc_0 = new A2D(0);
	
	static uint16_t* adc_result;
	while(1)
	{
		adc_0->read();
	}
}
