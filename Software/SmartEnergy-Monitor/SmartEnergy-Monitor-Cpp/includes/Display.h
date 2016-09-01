/*
 * Display.h
 *
 * Created: 13/08/2016 2:36:25 AM
 *  Author: Jerry Fan
 */


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "UART.h"

class Display : public UART
{
	public:
	Display();

	void transmit(uint8_t character);

	void transmitArray(uint8_t *characters, uint8_t decimalIndex);

	void idle();

	uint8_t encode(uint8_t character);

	void encodeArray(uint8_t *characters, uint8_t decimalIndex);

	uint8_t encodeSync();
	
};

#endif /* DISPLAY_H_ */
