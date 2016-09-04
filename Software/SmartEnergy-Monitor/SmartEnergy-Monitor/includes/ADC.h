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

struct ADCData
{
	uint32_t timestamp;
	int16_t value;
};

struct SignalData
{
	int16_t sum;
	int16_t max;
	int16_t min;
	uint32_t squared;
	bool waveDirection;
	uint32_t lastPeriod;
};

struct PowerData
{
	int16_t sum;
	int8_t max;
	int8_t min;
	uint32_t periodDifferenceSum;
};

void ADC_init();

void ADC_processData(struct ADCData *storage, int16_t data);

void Power_processData(int16_t data);


#endif /* ADC_H_ */