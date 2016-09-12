/*
* Display.c
*
* Created: 13/08/2016 2:36:17 AM
*  Author: Jerry Fan
*/

#include "Display.h"
#include "UART.h"

#include <stdlib.h>
#include <avr/io.h>
#include <util/atomic.h>

#define DISPLAY_INDEX_MAX 2

volatile uint8_t Display_state = 0;
float Display_values[5] = {0, 0, 0, 0, 0};

static uint8_t stateDelay = 0;
static uint8_t displayUpdateDelay = 0;
static uint8_t displayIndex = 0;
static uint8_t Display_names[5][4] = 
{
	{'U', 'o', 'l', 'A'},
	{'C', 'u', 'r', 'r'},
	{'P', 'o', 'E', 'r'},
	{'F', 'r', 'E', 'E'},
	{'P', 'H', 'A', 'S'}
};

void Display_init()
{
	TCCR0A = 0;
	TCCR0B = (1 << CS02) | (1 << CS00);
	TIMSK0 = (1 << TOIE0);
	TCNT0  = 0;
}

uint8_t Display_encodeChar(uint8_t character)
{
	switch (character)
	{
		case '0':
		return 0b01111110;
		case '1':
		return 0b00110000;
		case '2':
		return 0b01101101;
		case '3':
		return 0b01111001;
		case '4':
		return 0b00110011;
		case '5':
		return 0b01011011;
		case '6':
		return 0b01011111;
		case '7':
		return 0b01110000;
		case '8':
		return 0b01111111;
		case '9':
		return 0b01110011;
		case 'A':
		return 0b01110111;
		case 'b':
		return 0b00011111;
		case 'C':
		return 0b01001110;
		case 'd':
		return 0b00111101;
		case 'E':
		return 0b01001111;
		case 'F':
		return 0b01000111;
		case 'H':
		return 0b00110111;
		case 'I':
		return 0b00000110;
		case 'l':
		return 0b00000110;
		case 'L':
		return 0b00001110;
		case 'n':
		return 0b00010101;
		case 'O':
		return 0b01111110;
		case 'o':
		return 0b00011101;
		case 'P':
		return 0b01100111;
		case 'r':
		return 0b00000101;
		case 'S':
		return 0b01011011;
		case 'u':
		return 0b00011100;
		case 'U':
		return 0b00111110;
		case 'y':
		return 0b00111011;
		case 'Z':
		return 0b01101101;
		case '-':
		return 0b00000001;
		case '_':
		return 0b00001000;
		case '\'':
		return 0b00000010;
		case '"':
		return 0b00100010;
		case '^':
		return 0b01100010;
		case '[':
		return 0b01001110;
		case ']':
		return 0b01111000;
		case ' ':
		return 0b00000000;
		default:
		return 0b01111110;
	}
}

uint8_t Display_encodeSync()
{
	return 0b00000000;
}

void Display_encode(uint8_t *characters, uint8_t decimalIndex)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		characters[i] = Display_encodeChar(characters[i]);
		if (i == decimalIndex)
		{
			characters[i] |= (1 << 7); 				// Bit twiddling to enable the dp bit
		}
	}
}

void Display_floatToChar(float value, uint8_t *result, uint8_t *decimalIndex)
{
	int16_t dec = value;
	if (dec > 9999 || dec < -999)
	{
		result[0] = 'O';
		result[1] = 'F';
		result[2] = 'L';
		result[3] = 'O';
		*decimalIndex = 4;
		return;
	}

	if (value < 0.0005 && value > -0.0005)
	{
		result[0] = '0';
		result[1] = '0';
		result[2] = '0';
		result[3] = '0';
		*decimalIndex = 0;
		return;
	}

	uint8_t charsUsed = 0;

	if (value < 0)
	{
		result[0] = '-';
		dec = abs(dec);
		charsUsed++;
	}
	
	for (uint16_t i = 1000; i >= 1; i /= 10)
	{
		if (dec > i)
		{
			result[charsUsed] = dec / i % 10 + '0';
			charsUsed++;
		}
	}
	
	if (charsUsed == 0)
	{
		result[0] = '0';
		*decimalIndex = 0;
		charsUsed++;
	}

	*decimalIndex = charsUsed - 1;

	while (charsUsed < 3)
	{
		value = value * 10;
		result[charsUsed] = ((uint32_t) value) % 10 + '0';	
		charsUsed++;
	}

	if (charsUsed == 3)
	{
		result[charsUsed] = ((uint32_t) (value + 0.5)) % 10 + '0';
		charsUsed++;
	}
}

ISR( TIMER0_OVF_vect )
{
	if (displayUpdateDelay >= 25)
	{
		displayUpdateDelay = 0;
		uint8_t tempArray[4];
		uint8_t tempDecimalIndex = 0;
		if (Display_state == 0)
		{
			for (uint8_t i = 0; i < 4; i++)
			{
				tempArray[i] = Display_names[displayIndex][i];
			}
			Display_encode(tempArray, 4);
			if (stateDelay >= 4) 
			{
				stateDelay = 0;
				Display_state++;
				Display_state &= 1;	
			}
		}
		else if (Display_state == 1)
		{
			Display_floatToChar(Display_values[displayIndex], tempArray, &tempDecimalIndex);
			Display_encode(tempArray, tempDecimalIndex);
			if (stateDelay >= 16) 
			{
				stateDelay = 0;
				Display_state++;
				Display_state &= 1;	

				displayIndex++;
				if (displayIndex > DISPLAY_INDEX_MAX) 
				{
					displayIndex = 0;
				}
			}
		}
		stateDelay++;
		UART_transmitArray(tempArray);
	}
	displayUpdateDelay++;
	
}