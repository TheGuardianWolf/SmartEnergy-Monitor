/*
* ADC.h
*
* Created: 3/09/2016 2:21:14 AM
*  Author: lichk
*/


#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Structure for holding data from ADC.
 */
struct ADCData
{
	uint64_t timestamp;
	int16_t value;
};

/**
 * Structure for holding processed signal data.
 */
struct SignalData
{
	uint16_t sampleCount;
	int32_t sum;
	int16_t max;
	int16_t min;
	uint32_t squared;
	uint8_t periodCount;
	uint64_t lastPeriod;
};

/**
 * Structure for holding processed power data.
 */
struct PowerData
{
	uint16_t sampleCount;
	int32_t sum;
	int32_t max;
	int32_t min;
};

// Declare constants
const uint8_t periodCountMax;
const double ADC_sensitivity;

// Volatile variables for use in tranporting data between ISR and elsewhere.
volatile uint8_t ADC_state;
volatile bool ADC_dataReady;
volatile uint16_t lastPeriodTimeSum;
volatile int16_t lastVCTDSum;
volatile struct PowerData lastPower;
volatile struct SignalData lastVoltage;
volatile struct SignalData lastCurrent;

/**
 * Initialise the ADC.
 * VCC reference, read channel 2, interrupt enabled, auto trigger, 64 prescale.
 */
void ADC_init();

/**
 * Initialise the external interrupts.
 * Port D (2,3) to input, enable pull-up resistors, enable interrupts on rising
 * edge.
 */
void ADC_initComparators();

/**
 * Sets the ADC channel for next cycle.
 * @param ch Channel number (0-7).
 */
void ADC_setChannel(uint8_t ch);

/**
 * Stores the ADC data into the specified struct.
 * @param storage Pointer to data struct.
 * @param data    Adjusted ADC value.
 */
void ADC_processData(struct ADCData *storage, int16_t data);

/**
 * Converts an ADC data value into a voltage (0-5V).
 * @param  adcValue The ADC value.
 * @return          A floating point voltage value.
 */
double ADC_convertToVoltage(double adcValue);

/**
 * Signals data is ready to be read by other modules. Sets ADC_dataReady flag.
 */
void ADC_setDataReady();

/**
 * Clears data stored in a SignalData struct.
 * @param storage [description]
 */
void Signal_clear(struct SignalData *storage);

/**
 * Fills in the SignalData struct with specified data.
 * @param storage The specified struct.
 * @param data    The specified data.
 */
void Signal_processData(struct SignalData *storage, int16_t data);

/**
 * Fills in the PowerData struct.
 */
void Power_processData();

/**
 * Clears the PowerData struct.
 */
void Power_clear();

#endif /* ADC_H_ */
