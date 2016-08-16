/*
* Uart.c
*
* Created: 12/08/2016 12:38:52 PM
*  Author: lichk
*/

#include "uart.h"

void uart_init(uint16_t baud)
{
	UCSR0B |= (1<<TXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);	// use 8-bit character sizes
	UBRR0H = (baud >> 8);
	UBRR0L = baud;

	_delay_ms(1);
}

void uart_transmit(int8_t data)
{
	while(!((1<<UDRE0) && UCSR0A));
	UDR0 = data;
}

void uart_transmit_array(int8_t *data, uint8_t array_size)
{
	for (uint8_t i = 0; i < array_size; i++)
	{
		uart_transmit(data[(array_size - 1) - i]);
		_delay_ms(1);
	}
}