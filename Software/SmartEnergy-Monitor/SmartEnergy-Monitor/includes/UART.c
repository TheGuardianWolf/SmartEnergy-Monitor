/*
* Uart.c
*
* Created: 12/08/2016 12:38:52 PM
*  Author: Jerry Fan
*/

#include "UART.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

// BUFFER and UART

static uint8_t transmitIndex = 0;
static uint8_t Buffer_buffer[6] = {0, 0, 0, 0, 0, 0}; // Pre-fill values to display "On"

void Buffer_setSync(uint8_t syncPacket)
{
	Buffer_buffer[0] = syncPacket;
}

void Buffer_setTerm(uint8_t termPacket)
{
	Buffer_buffer[5] = termPacket;
}

void Buffer_fill(uint8_t *data)
{
	uint8_t j = 4;
	for (uint8_t i = 1; i < 5; i++)
	{
		j--;
		Buffer_buffer[i] = data[j];
	}
}

void UART_init()
{
	UCSR0C = (1 << UPM00) | (1 << UPM01) | (1 << USBS0) | (1 << UCSZ00) |
			 (1 << UCSZ01);
	UBRR0H = (BAUDRATE >> 8);
	UBRR0L = BAUDRATE;
	UCSR0B = (1 << TXEN0) | (1 << UDRIE0);
}

void UART_transmit(uint8_t data)
{
}

void UART_transmitArray(uint8_t *data)
{
	Buffer_fill(data);
}

ISR(USART_UDRE_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// Load buffer and switch to next character.
		UDR0 = Buffer_buffer[transmitIndex];
		transmitIndex++;
		if (transmitIndex > 5)
		{
			transmitIndex = 0;
		}
	}
}
