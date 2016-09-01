/*
* Display.c
*
* Created: 13/08/2016 2:36:17 AM
*  Author: Jerry Fan
*/

#include "Display.h"

#include <avr/io.h>

Display::Display()
{
}

void Display::transmit(uint8_t character)
{
	UART::transmit(this->encode(character));
}

void Display::transmitArray(uint8_t *characters, uint8_t decimal_index)
{
	this->encodeArray(characters, decimal_index);
	UART::transmitArray(characters, 4);
}

void Display::idle()
{
	UDR0 = this->encodeSync();
}

uint8_t Display::encode(uint8_t character)
{
	switch(character)
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
		return 0b01100000;
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
		case 'o':
		return 0b00010101;
		case 'P':
		return 0b01100111;
		case 'r':
		return 0b00000101;
		case 'S':
		return 0b01011011;
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

void Display::encodeArray(uint8_t *characters, uint8_t decimalIndex)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		characters[i] = Display::encode(characters[i]);
		if (i == decimalIndex)
		{
			characters[i] |= (1 << 7); 				// Bit twiddling to enable the dp bit
		}
	}
}

uint8_t Display::encodeSync()
{
	return 0b00000000;
}