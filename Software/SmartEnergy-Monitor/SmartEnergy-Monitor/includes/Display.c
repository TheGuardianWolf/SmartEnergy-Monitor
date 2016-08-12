/*
* Display.c
*
* Created: 13/08/2016 2:36:17 AM
*  Author: lichk
*/

int8_t display_encode_char(char character)
{
	switch (character)
	{
		case '0':
		return 0b10000001;
		case '1':
		return 0b11001111;
		case '2':
		return 0b10010010;
		case '3':
		return 0b10000110;
		case '4':
		return 0b11001100;
		case '5':
		return 0b10100100;
		case '6':
		return 0b10100000;
		case '7':
		return 0b10011111;
		case '8':
		return 0b10000000;
		case '9':
		return 0b10001100;
		case 'A':
		return 0b10001000;
		case 'b':
		return 0b11100000;
		case 'C':
		return 0b10110001;
		case 'd':
		return 0b11000010;
		case 'E':
		return 0b10110000;
		case 'F':
		return 0b10111000;
		case 'H':
		return 0b11001000;
		case 'I':
		return 0b11111001;
		case 'l':
		return 0b11111001;
		case 'L':
		return 0b11110001;
		case 'n':
		return 0b11101010;
		case 'o':
		return 0b11101010;
		case 'P':
		return 0b10011000;
		case 'r':
		return 0b11111010;
		case 'S':
		return 0b10100100;
		case 'U':
		return 0b11000001;
		case 'y':
		return 0b11000100;
		case 'Z':
		return 0b10010010;
		default:
		return 0b10000001;
	}
}

int8_t display_sync();
{
	return 0b11111111;
}

void display_encode(char &characters, uint8_t decimal_index)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		*characters[i] = display_encode_char(*characters[i]);
		if (i == decimal_index)
		{
			*characters[i] |= (1 << 7);
		}
	}
}

