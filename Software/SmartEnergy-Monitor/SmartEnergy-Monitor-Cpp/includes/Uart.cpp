/*
* Uart.c
*
* Created: 12/08/2016 12:38:52 PM
*  Author: Jerry Fan
*/

#include "UART.h"

#include <stddef.h>
#include <avr/interrupt.h>
#include <avr/io.h>

// F_CPU should be defined in compile flags
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Set the baud rate and calculate prescaler
#define BAUD 9600
#define BAUDRATE (((F_CPU / (BAUD * 16UL))) - 1)

namespace
{
	UART *activeUART = NULL;
}

UART::UART()
{
	activeUART = this;
}

void UART::init()
{
	UCSR0C = (1 << UPM00) | (1 << UPM01) | (1 << USBS0) | (1 << UCSZ00) |
			 (1 << UCSZ01); // 8-bit data, 2 stop bits, odd parity
	UBRR0H = (BAUDRATE >> 8); // Set the prescaler on high
	UBRR0L = BAUDRATE; // Set the prescaler on low
	UCSR0B = (1 << UDRIE0) | (1 << TXEN0); // Enable transmitter and tx buffer interrupt
}

void UART::transmit(uint8_t data)
{
	UCSR0B |= (1 << UDRIE0);
	while ( this->buffer->isFull() );
	this->buffer->push(data);
}

void UART::transmitArray(uint8_t *data, uint8_t arraySize)
{
	// Transmit in reverse over the array as reciever recieves rightmost digit
	// first
	for (uint8_t i = 0; i < arraySize; i++)
	{
		this->transmit(data[(arraySize - 1) - i]);
	}
}


void UART::idle()
{
	UCSR0B &= ~( 1 << UDRIE0 );
}

ISR( USART_UDRE_vect )
{
	if ( !activeUART->buffer->isEmpty() )
	{
		// Send the next byte
		UDR0 = activeUART->buffer->pull();
	}
	else
	{
		activeUART->idle();
	}
}