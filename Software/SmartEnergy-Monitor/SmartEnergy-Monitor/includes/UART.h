/*
 * Uart.h
 *
 * Created: 12/08/2016 12:39:01 PM
 *  Author: Jerry Fan
 */

#ifndef UART_H_
#define UART_H_

// F_CPU should be defined in compile flags
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Set the baud rate and calculate prescaler
#define BAUD 9600
#define BAUDRATE (((F_CPU / (BAUD * 16UL))) - 1)

#include <stdbool.h>
#include <stdint.h>

void Buffer_setSync(uint8_t syncPacket);

void Buffer_fill(uint8_t *data);

/**
 * Initialises the UART registers with the appropriate bits for 8-bit data, 2
 * stop bits and an odd parity bit.
 * @param baud Calculated BAUDRATE
 */
void UART_init();

/**
 * [DEPRECIATED] Transmits data
 * @param data 8 bit data to be transmitted
 */
void UART_transmit(uint8_t data);

/**
 * Transmits an array of data values
 * @param data       The array containing 8-bit data values
 * @param array_size The size of the array
 */
void UART_transmitArray(uint8_t *data);

#endif /* UART_H_ */
