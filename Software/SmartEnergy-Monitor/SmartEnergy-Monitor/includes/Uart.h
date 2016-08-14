/*
 * Uart.h
 *
 * Created: 12/08/2016 12:39:01 PM
 *  Author: lichk
 */ 

#ifndef UART_H_
#define UART_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define BAUD 9600
#define BAUDRATE F_CPU/16/BAUD-1

#include <avr/io.h>
#include <util/delay.h>

void uart_init(uint16_t baud);

void uart_transmit(int8_t data);

void uart_transmit_array(int8_t *data, uint8_t array_size);

#endif /* UART_H_ */