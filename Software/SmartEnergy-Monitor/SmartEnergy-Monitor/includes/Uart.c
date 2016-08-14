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
	UBRR0H = (uint8_t) (baud>>8);
	UBRR0L = (uint8_t) baud;

	_delay_ms(20);
	
	//UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
	//UCSR0C &= ~(1<<UMSEL00) & ~(1<<UMSEL01) & ~(1<<UPM01) & ~(1<<UPM00) & ~(1<<USBS0) & ~(1<<UCSZ02);
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
		uart_transmit(data[i]);
	}
}