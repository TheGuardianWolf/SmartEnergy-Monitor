/*
 * SmartEnergy-Monitor.c
 *
 * Created: 12/08/2016 12:38:02 PM
 * Author : Jerry Fan
 */ 

#define TRUE (1==1)
#define FALSE (!TRUE)

#include "includes/Interactive.h"
#include "includes/Uart.h"
#include "includes/Display.h"

int main(void)
{

	io_init();
	uart_init(BAUDRATE);

	int8_t characters[4] = "P189";

	display_encode(characters,1);

	while(1)
	{
		uart_transmit(display_sync());

		if (SWITCH_DOWN)
		{
			while(TRUE)
			{
				LED_ON;
				uart_transmit(display_sync());
				uart_transmit_array(characters,4);
			}
		}
	}
}
