/*
 * SmartEnergy-Monitor.c
 *
 * Created: 12/08/2016 12:38:02 PM
 * Author : Jerry Fan
 */ 

#include <avr/io.h>
#include "includes/Uart.h"
#include "includes/Display.h"

int main(void)
{
	int8_t characters[4] = "P189";

	display_encode(characters,1);

	uart_init(BAUDRATE);

	
	while(1) {
		uart_transmit(display_sync());
		uart_transmit_array(characters,4);
	}
}
