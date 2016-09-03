/*
* ADC.c
*
* Created: 3/09/2016 2:21:21 AM
*  Author: lichk
*/

#include "includes/ADC.h"

#include "includes/System.h"

#include <stdlib.h>
#include <avr/io.h>
#include <util/atomic.h>

#define SIGNAL_THRESHOLD 10

// ADC, SIGNAL, POWER

volatile static uint8_t ADC_channel = 2;
static int16_t nullVal = 338;
static bool transferred = false;
static uint16_t sampleCount = 0;
static const uint16_t sampleCountMax = 2048;
static uint8_t periodCount = 0;
static uint8_t periodCountMax = 4;
static struct ADCData voltageData = {0, 0};
static struct ADCData currentData = {0, 0};
static struct SignalData voltage = {0, -1024, 1024, 0, false, 0};
static struct SignalData current = {0, -1024, 1024, 0, false, 0};
static struct SignalData lastVoltage = {0, -1024, 1024, 0, false, 0};
static struct SignalData lastCurrent = {0, -1024, 1024, 0, false, 0};
static uint8_t instantPower = 0;
static struct PowerData power = {0, -128, 127, 0};
static struct PowerData lastPower = {0, -128, 127, 0};

void ADC_init()
{
	ADMUX = (1 << REFS0) | (1 << MUX1);
	ADCSRB = 0;
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1);
}

void ADC_processData(struct ADCData *storage, int16_t data)
{
	storage->timestamp = System_getTimeMicro();
	storage->value = data;
}

void Signal_processData(struct SignalData *storage, int16_t data, uint32_t lastTimestamp)
{
	storage->sum += data;
	storage->squared += (uint32_t) ((int32_t) data * (int32_t) data);

	if(data > storage->max)
	{
		storage->max = data;
	}
	if(data < storage->min)
	{
		storage->min = data;
	}

	// Zero crossing check, remove if using comparator
	if(storage->waveDirection == true && data > SIGNAL_THRESHOLD) {
		storage->waveDirection = false;
		storage->lastPeriod = lastTimestamp;
		periodCount++;
	}
	else if(storage->waveDirection == false && data < -SIGNAL_THRESHOLD) {
		storage->waveDirection = true;
	}
}

void Signal_clear(struct SignalData *storage)
{
	storage->lastPeriod = 0;
	storage->max = -1024;
	storage->min = 1024;
	storage->squared = 0;
	storage->sum = 0;
	storage->waveDirection = false;
}

void Power_processData(int16_t data)
{
	instantPower = (int8_t) ((int16_t) voltageData.value * (int16_t) currentData.value);
	power.sum += instantPower;
	if (instantPower > power.max)
	{
		power.max = instantPower;
	}
	if (instantPower < power.min)
	{
		power.min =instantPower;
	}
	power.periodDifferenceSum += (uint32_t) abs((int64_t) current.lastPeriod - (int64_t) voltage.lastPeriod);
}

void Power_clear()
{
	power.max = -128;
	power.min = 127;
	power.periodDifferenceSum = 0;
	power.sum = 0;
}

ISR(ADC_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		int16_t rawData = ADCL | (ADCH << 8);
		
		if (ADC_channel == 2)
		// First run null read
		{
			ADC_channel = 0;
			nullVal = 338;//rawData;
		}
		else
		{
			int16_t data = rawData - nullVal;

			ADC_channel++;
			if (ADC_channel > 1)
			{
				ADC_channel = 0;
			}
			ADMUX &= 0b11100000;
			ADMUX |= ADC_channel;

			if (ADC_channel == 0)
			{
				ADC_processData(&voltageData, data);
				Signal_processData(&voltage, data, voltageData.timestamp);
			}
			else if (ADC_channel == 1)
			{
				ADC_processData(&currentData, data);
				Signal_processData(&current, data, currentData.timestamp);
						
				Power_processData(data);
						
				sampleCount++;
				if((sampleCount > sampleCountMax) || (periodCount > periodCountMax))
				{
					lastVoltage = voltage;
					lastCurrent = current;
					lastPower = power;
					transferred = true;
					sampleCount = 0;
					periodCount = 0;
					Signal_clear(&voltage);
					Signal_clear(&current);
					Power_clear();
					ADCSRA &= ~(1 << ADIE);
					ADC_channel = 2;
					ADMUX &= 0b11111000;
					ADMUX |= ADC_channel;
				}
			}
		}

	}
}