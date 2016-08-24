/*
* Uart.c
*
* Created: 12/08/2016 12:38:52 PM
*  Author: Jerry Fan
*/

#include "uart.h"

void uart_init()
{
	UCSR0B = (1 << TXEN0); // Enable transmitter
	UCSR0C = (1 << UPM00) | (1 << UPM01) | (1 << USBS0) | (1 << UCSZ00) |
					 (1 << UCSZ01); // 8-bit data, 2 stop bits, odd parity
	UBRR0H = (BAUDRATE >> 8); // Set the prescaler on high
	UBRR0L = BAUDRATE; // Set the prescaler on low

	_delay_ms(1); // Pause before resuming
}

void uart_transmit(int8_t data)
{
	while(!((1<<UDRE0) && UCSR0A)); // Wait for empty transmit buffer
	UDR0 = data; // Transmit
}

void uart_transmit_array(int8_t *data, uint8_t array_size)
{
	// Transmit in reverse over the array as reciever recieves rightmost digit
	// first
	for (uint8_t i = 0; i < array_size; i++)
	{
		uart_transmit(data[(array_size - 1) - i]);
	}
}
