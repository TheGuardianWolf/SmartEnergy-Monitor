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

#include "includes/Interactive.h"
#include "includes/Uart.h"
#include "includes/Display.h"

int main(void)
{
	io_init(); // Prepare the IO registers for writing/reading
	uart_init(BAUDRATE); // Set the correct prescaler and UART options

	int8_t characters[4] = "P189"; // Characters to be transmitted in a char array

	display_encode(characters,1); // Encode the characters and add decimal

	while(1)
	{
		uart_transmit(display_sync()); // Whilst idle, send the sync packet

		if (SWITCH_DOWN)
		{
			while(TRUE)
			{
				LED_ON;
				uart_transmit(display_sync()); // Sync after every 4 digits.
				uart_transmit_array(characters,4); // Transmit the 4 character array
			}
		}
	}
}
