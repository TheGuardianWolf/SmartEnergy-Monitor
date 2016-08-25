/*
 * Display.h
 *
 * Created: 13/08/2016 2:36:25 AM
 *  Author: Jerry Fan
 */


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "lib/AVRTools/USART0.h"

class Display {
public:
	Display();

	static void init();

	static char encodeChar(char character);

	static void encode(char *characters, uint8_t decimal_index);

	static void sync();

	static void sendChar(char character);

	static void send(char *characters, uint8_t decimal_index);
};

#endif /* DISPLAY_H_ */
