/*
 * Display.h
 *
 * Created: 13/08/2016 2:36:25 AM
 *  Author: Jerry Fan
 */


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

void Display_init();

/**
 * Encode one character for displaying on the LCD using the special UART
 * encoding scheme.
 * @param  character The character to be encoded
 * @return           The encoded character
 */
uint8_t Display_encodeChar(uint8_t character);

/**
 * Returns the currently set sync packet.
 * @return The sync packet
 */
uint8_t Display_encodeSync();

/**
 * Encodes an array of up to 4 characters inplace using encode char.
 * @param characters    The pointer to the array to be encoded
 * @param decimal_index The index where the decimal should be placed.
 */
void Display_encode(uint8_t *characters, uint8_t decimal_index);

#endif /* DISPLAY_H_ */
