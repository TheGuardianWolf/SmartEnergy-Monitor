/*
 * Display.h
 *
 * Created: 13/08/2016 2:36:25 AM
 *  Author: lichk
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>

int8_t display_encode_char(char character);

void display_encode(char &characters);

int8_t display_sync();

#endif /* DISPLAY_H_ */