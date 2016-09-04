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
	uint16_t sampleCount;
	int16_t sum;
	int16_t max;
	int16_t min;
	uint32_t squared;
	uint8_t periodCount;
	uint16_t lastPeriod;
};

struct PowerData
{
	int16_t sum;
	int8_t max;
	int8_t min;
};

volatile uint8_t ADC_state;
uint8_t periodCountMax;
bool Signal_mainDataReady;
const float ADC_sensitivity;
uint16_t lastPeriodTimeSum;
int16_t lastVoltageCurrentTimeDifferenceSum;
struct PowerData lastPower;
struct SignalData lastVoltage;
struct SignalData lastCurrent;

void ADC_init();

void ADC_initComparators();

void ADC_processData(struct ADCData *storage, int16_t data);

float ADC_convertToValue(int16_t adcValue);

void Signal_clear(struct SignalData *storage);

void Power_processData();


#endif /* ADC_H_ */