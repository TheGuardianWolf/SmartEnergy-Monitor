/*
 * SmartEnergy-Monitor.c
 *
 * Created: 12/08/2016 12:38:02 PM
 * Author : Jerry Fan
 */ 

#define TRUE (1==1)
#define FALSE (!TRUE)

#define LED_ON PORTB |=  (1<<PORTB5)
#define LED_OFF PORTB &= ~(1<<PORTB5)
#define SWITCH_DOWN !(PINB & (1<<PB7))

#include <avr/io.h>
#include "includes/Uart.h"
#include "includes/Display.h"

void io_init()
{
	DDRB	|= (1<<DDB5);						// PB5 As Output pin
	PORTB	|= (1<<DDB5);						// PB5 Activate internal pullUp resistor

	DDRB	&= ~(1<<DDB7);						// PB1 As Input pin
	PORTB	|= 1<<DDB7;							// PB1 Activate internal pullUp resistor

	LED_OFF;									// start with the LED off
}

int main(void)
{

	io_init();
	uart_init(BAUDRATE);

	int8_t characters[4] = "P189";

	display_encode(characters,1);

	while(1)
	{
		uart_transmit(display_sync());

		if (SWITCH_DOWN)
		{
			while(TRUE)
			{
				LED_ON;
				uart_transmit(display_sync());
				uart_transmit_array(characters,4);
			}
		}
	}
}
