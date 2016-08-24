/*
 * SmartEnergy-Monitor.c
 * Code for the wireless energy monitor. Odd parity is enabled for transmission.
 * Group 10
 *
 * Created: 12/08/2016 12:38:02 PM
 * Author : Jerry Fan
 */

#define TRUE (1==1)
#define FALSE (!TRUE)

#include "protothreads/pt.h"
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

	static uint16_t adc_result[3];
	while(1)
	{
		for (uint8_t i = 0; i < 3; i++)
		{
			adc_result[i] = adc_read(i);
		}
	}
}
