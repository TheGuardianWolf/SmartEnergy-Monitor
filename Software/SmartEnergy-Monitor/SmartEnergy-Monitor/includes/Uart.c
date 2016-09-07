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

#include <includes/Display.h>
#include <includes/Interface.h>

// BUFFER and UART

static uint8_t transmitIndex = 0;
static uint8_t Buffer_buffer[5] = {0b00000000,0,0,0,0};

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
			 (1 << UCSZ01); // 8-bit data, 2 stop bits, odd parity
	UBRR0H = (BAUDRATE >> 8); // Set the prescaler on high
	UBRR0L = BAUDRATE; // Set the prescaler on low
	UCSR0B = (1 << TXEN0) | (1 << UDRIE0); // Enable transmitter and tx buffer interrupt
}

void UART_transmit(uint8_t data)
{
}

void UART_transmitArray(uint8_t *data)
{
	// Transmit in reverse over the array as reciever recieves rightmost digit
	// first

	Buffer_fill(data);
}

ISR(USART_UDRE_vect)
{
	
	UDR0 = Buffer_buffer[transmitIndex];
	transmitIndex++;
	if (transmitIndex > 4)
	{
		transmitIndex = 0;
	}
}