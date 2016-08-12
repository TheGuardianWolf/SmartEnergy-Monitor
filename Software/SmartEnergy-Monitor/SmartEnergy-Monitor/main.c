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
	uart_init(BAUDRATE);
	uart_transmit(display_encode_char(0));
}
