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

static uint8_t Buffer_buffer[16];
static const uint8_t Buffer_size = 16;
static uint8_t Buffer_frontIndex = 0;
static uint8_t Buffer_itemCount = 0;

static bool Buffer_isFull()
{
	if ((Buffer_size  - Buffer_itemCount) == 0)
	{
		return true;
	}
	return false;
}

static bool Buffer_isEmpty()
{
	if ((Buffer_size  - Buffer_itemCount) == Buffer_size)
	{
		return true;
	}
	return false;
}

static bool Buffer_push(uint8_t data)
{
	if (!Buffer_isFull())
	{
		Buffer_buffer[(Buffer_frontIndex + Buffer_itemCount) % Buffer_size] = data;
		Buffer_itemCount++;
		return true;
	}
	return false;
}

static uint8_t Buffer_pull()
{
	if (!Buffer_isEmpty())
	{
		uint8_t data = Buffer_buffer[Buffer_frontIndex];
		Buffer_itemCount--;
		Buffer_frontIndex++;
		return data;
	}
	return -1;
}

//static uint8_t Buffer_peek()
//{
	//return Buffer_buffer[Buffer_frontIndex];
//}
//
//static void Buffer_clear()
//{
	//Buffer_itemCount = 0;
//}

void UART_init()
{
	UCSR0C = (1 << UPM00) | (1 << UPM01) | (1 << USBS0) | (1 << UCSZ00) |
			 (1 << UCSZ01); // 8-bit data, 2 stop bits, odd parity
	UBRR0H = (BAUDRATE >> 8); // Set the prescaler on high
	UBRR0L = BAUDRATE; // Set the prescaler on low
	UCSR0B = (1 << TXEN0); // Enable transmitter and tx buffer interrupt
}

void UART_transmit(uint8_t data)
{
	while(Buffer_isFull())
	{
		if(!((1 << UDRIE0) & 1))
		{
			UCSR0B |= (1 << UDRIE0);
			UCSR0A |= (1 << TXC0);
		}
	}
	Buffer_push(data);
	UCSR0B |= (1 << UDRIE0);
	UCSR0A |= (1 << TXC0);
}

void UART_transmitArray(uint8_t *data, uint8_t array_size)
{
	// Transmit in reverse over the array as reciever recieves rightmost digit
	// first
	for (uint8_t i = 0; i < array_size; i++)
	{
		UART_transmit(data[(array_size - 1) - i]);
	}
}

ISR(USART_UDRE_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (!Buffer_isEmpty())
		{
			UDR0 = Buffer_pull();
		}
		else
		{
			UCSR0B &= ~(1 << UDRIE0);
		}
	}
}