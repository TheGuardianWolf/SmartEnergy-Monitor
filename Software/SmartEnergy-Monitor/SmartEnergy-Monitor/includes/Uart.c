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

#include <includes/Interface.h>

// BUFFER and UART

static uint8_t Buffer_buffer[64];
static const uint8_t Buffer_size = 16;
static uint8_t Buffer_frontIndex = 0;
static uint8_t Buffer_itemCount = 0;

static bool Buffer_isFull()
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
	    return ( Buffer_size - Buffer_itemCount ) <= 0;
    }
}

static bool Buffer_isEmpty()
{
	return ( Buffer_itemCount == 0 );
}

static bool Buffer_push(uint8_t data)
{
   ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
   {
	   if ( Buffer_itemCount < Buffer_size )
	   {
		   Buffer_buffer[ ( Buffer_frontIndex + Buffer_itemCount ) % Buffer_size ] = data;
		   ++Buffer_itemCount;
		   return 0;
	   }
   }
   // If buffer is full, ignore the push()
   return 1;
}

static uint8_t Buffer_pull()
{
   int data = -1;
   ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
   {
	   if ( Buffer_itemCount )
	   {
		   data = Buffer_buffer[ Buffer_frontIndex ];
		   Buffer_frontIndex++;
		   if ( Buffer_frontIndex >= Buffer_size )
		   {
			   Buffer_frontIndex -= Buffer_size;
		   }
		   --Buffer_itemCount;
	   }
   }
   return data;
}

void UART_init()
{
	UCSR0C = (1 << UPM00) | (1 << UPM01) | (1 << USBS0) | (1 << UCSZ00) |
			 (1 << UCSZ01); // 8-bit data, 2 stop bits, odd parity
	UBRR0H = (BAUDRATE >> 8); // Set the prescaler on high
	UBRR0L = BAUDRATE; // Set the prescaler on low
	UCSR0B = (1 << TXEN0) | (1 << UCSR0B); // Enable transmitter and tx buffer interrupt
}

void UART_transmit(uint8_t data)
{
	while(Buffer_isFull()) {
		Interface_ledOn();
		//while ( !( UCSR0A & (1<<UDRE0) ) );
		//UDR0 = Buffer_pull();
	}
	Interface_ledOff();
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
	if (!Buffer_isEmpty())
	{
		UDR0 = Buffer_pull();
	}
	else
	{
		UCSR0B &= ~(1 << UDRIE0);
	}
}