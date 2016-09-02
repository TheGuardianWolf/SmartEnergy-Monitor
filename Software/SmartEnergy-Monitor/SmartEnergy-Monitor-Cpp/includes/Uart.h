/*
 * Uart.h
 *
 * Created: 12/08/2016 12:39:01 PM
 *  Author: Jerry Fan
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "includes/Heap.h"
#include "includes/Buffer.h"

class UART
{	
	/**
	 * Initialises the UART registers with the appropriate bits for 8-bit data, 2
	 * stop bits and an odd parity bit.
	 * @param baud Calculated BAUDRATE
	 */
	public:

	UART();

	static void init();

	/**
	 * Transmits data
	 * @param data 8 bit data to be transmitted
	 */
	virtual void transmit(uint8_t data);

	/**
	 * Transmits an array of data values
	 * @param data       The array containing 8-bit data values
	 * @param array_size The size of the array
	 */
	virtual void transmitArray(uint8_t *data, uint8_t arraySize);

	virtual void idle();

	Buffer *buffer;

};

#endif /* UART_H_ */
