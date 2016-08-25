/*
 * SmartEnergy-Monitor.c
 * Code for the wireless energy monitor. Odd parity is enabled for transmission.
 * Group 10
 *
 * Created: 12/08/2016 12:38:02 PM
 * Author : Jerry Fan
 */

#define __AVR_ATmega328P__ 1
#define TRUE (1==1)
#define FALSE (!TRUE)


#include "lib/AVRTools/InitSystem.h"
#include "lib/AVRTools/SystemClock.h"
#include "includes/Sensors.h"
#include "includes/Calc.h"
#include "includes/Interactive.h"
#include "includes/Uart.h"
#include "includes/Display.h"

void initialise()
{
	io_init();
	adc_init();
	uart_init();
}



int main(void)
{
	initialise();
	while(true)
	{
		if (SWITCH_DOWN)
		{
			LED_ON;
		}
	}
//
	//static uint16_t adc_result[3];
	//while(1)
	//{
		//for (uint8_t i = 0; i < 3; i++)
		//{
			////adc_result[i] = adc_get_data(i);
		//}
	//}
}
