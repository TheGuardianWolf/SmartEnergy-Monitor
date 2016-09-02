/*
 * Sensors.h
 *
 * Created: 24/08/2016 6:22:22 PM
 *  Author: lichk
 */ 


#ifndef SENSORS_H_
#define SENSORS_H_

// F_CPU should be defined in compile flags
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>

void adc_init();

uint16_t adc_read(uint8_t ch);


#endif /* SENSORS_H_ */