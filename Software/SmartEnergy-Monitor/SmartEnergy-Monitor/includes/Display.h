/*
 * Display.h
 *
 * Created: 13/08/2016 2:36:25 AM
 *  Author: Jerry Fan
 */


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

// Enums to identify values in the display values array.
enum DisplayParam
{
	vRMS,
	iMAX,
	pAVG,
	frequency,
	phaseDifference
};

// Declare globals
volatile uint8_t Display_state;
float Display_values[5];

/**
 * Initialise display registers.
 * Change default buffer values to "On", update sync packet, enable timer0 and
 * set prescale to 1024, enable timer0 interrupt, update term packet.
 */
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

uint8_t Display_encodeTerm();

/**
 * Encodes an array of up to 4 characters inplace using encode char.
 * @param characters    The pointer to the array to be encoded
 * @param decimal_index The index where the decimal should be placed.
 */
void Display_encode(uint8_t *characters, uint8_t decimal_index);

/**
 * Converts a float to a 4 character array.
 * @param value        Float to be converted.
 * @param result       Pointer to array storage of size 4+.
 * @param decimalIndex Pointer to a 8-bit int.
 */
void Display_floatToChar(float value, uint8_t *result, uint8_t *decimalIndex);

/**
 * State machine intended to be run in the main loop. Controls all Display
 * outputs.
 */
void Display_runStateMachine();

#endif /* DISPLAY_H_ */
