/*
 * Display.h
 *
 * Created: 13/08/2016 2:36:25 AM
 *  Author: Jerry Fan
 */


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "lib/AVRTools/USART0.h"

class Display 
{
	public:
	Display();

	static void init();

	char encodeChar(char character);

	void encode(char *characters, uint8_t decimalIndex);

	void sync();

	void sendChar(char character);

	void send(char *characters, uint8_t decimalIndex);

	private:
	Serial0 serial0;
};

#endif /* DISPLAY_H_ */
