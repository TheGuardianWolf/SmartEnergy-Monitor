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
#include <limits.h>

// ADC, SIGNAL, POWER classes ported from C++

// Initialise constants/
const uint8_t periodCountMax = 20;
const float ADC_sensitivity = 4.882813e-3;

// Set global data transfer variables.
volatile uint8_t ADC_state = 0;
volatile bool ADC_dataReady = false;
volatile uint16_t lastPeriodTimeSum = 0;
volatile int16_t lastVCTDSum = 0;
volatile struct PowerData lastPower = {0, 0, INT_MIN, INT_MAX};
volatile struct SignalData lastVoltage = {0, 0, -1024, 1024, 0, 0};
volatile struct SignalData lastCurrent = {0, 0, -1024, 1024, 0, 0};

// Set local variables.
static int16_t rawData = 0;
static int16_t data = 0;
static uint8_t ADC_channel = 2;
static int16_t nullVal = 338;
static const uint16_t DCSampleCountMax = 512;
static uint8_t periodCount = 0;
static uint16_t periodTimeSum = 0;
static int16_t VCTDSum = 0;
static struct ADCData voltageData = {0, 0};
static struct ADCData currentData = {0, 0};
static struct SignalData voltage = {0, 0, -1024, 1024, 0, 0};
static struct SignalData current = {0, 0, -1024, 1024, 0, 0};
static struct PowerData power = {0, 0, INT_MIN, INT_MAX};


void ADC_init()
{
	ADMUX = (1 << REFS0) | (1 << MUX1);
	ADCSRB = 0;
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1);
}

void ADC_initComparators()
{
	DDRD = (1 << DDD2) | (1 << DDD3);
	PORTD = (1 << PORTD2) | (1 << PORTD3);
	EICRA = 1;
	EIMSK = (1 << INT0) | (1<< INT1);
}

void ADC_setChannel(uint8_t ch)
{
	// Check if valid channel number.
	if (ch < 8)
	{
		ADMUX &= 0b11100000;
		ADMUX |= ch;
		ADC_channel = ch;
	}
}

void ADC_processData(struct ADCData *storage, int16_t data)
{
	storage->timestamp = System_getTimeMicro();
	storage->value = data;
}

float ADC_convertToVoltage(float adcValue)
{
	return adcValue * ADC_sensitivity;
}

void ADC_setDataReady()
{
	lastVoltage = voltage;
	lastCurrent = current;
	lastPower = power;
	lastPeriodTimeSum = periodTimeSum;
	lastVCTDSum = VCTDSum;
	ADC_dataReady = true;
	periodTimeSum = 0;
	VCTDSum = 0;
	periodCount = 0;
	Signal_clear(&voltage);
	Signal_clear(&current);
	Power_clear();
}

void Signal_clear(struct SignalData *storage)
{
	storage->sampleCount = 0;
	storage->lastPeriod = 0;
	storage->max = -1024;
	storage->min = 1024;
	storage->squared = 0;
	storage->sum = 0;
}

void Signal_processData(struct SignalData *storage, int16_t data)
{
	storage->sampleCount++;
	storage->sum += data;
	storage->squared += (uint32_t) ((int32_t) data * data);
	if(data > storage->max)
	{
		storage->max = data;
	}
	if(data < storage->min)
	{
		storage->min = data;
	}
}

void Power_processData()
{
	power.sampleCount++;
	int32_t instantPower = (int32_t) voltageData.value * currentData.value;
	power.sum += instantPower;
	if (instantPower > power.max)
	{
		power.max = instantPower;
	}
	if (instantPower < power.min)
	{
		power.min =instantPower;
	}
}

void Power_clear()
{
	power.sampleCount = 0;
	power.max = INT_MIN;
	power.min = INT_MAX;
	power.sum = 0;
}

// To ensure readings are done as soon as possible after they are taken from the ADC, we have a pretty filled up ISR unfortunately.
ISR(ADC_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		rawData = ADCL | (ADCH << 8);
	}

	// Initialization
	if (ADC_state == 0)
	{
		ADC_setChannel(0);
		ADC_state = 1;
	}
	// Read reference and set null
	else if (ADC_state == 1)
	{
		ADC_setChannel(1);
		nullVal = 338;
		//nullVal = rawData;
		ADC_state = 2;
	}
	// Measurement states
	else
	{
		data = rawData - nullVal;

		ADC_setChannel((ADC_channel + 1) & 1);

		// DC mode
		if (ADC_state == 2)
		{
			if (ADC_channel == 0)
			{
				ADC_processData(&voltageData, data);
				Signal_processData(&voltage, data);
			}
			else if (ADC_channel == 1)
			{
				ADC_processData(&currentData, data);
				Signal_processData(&current, data);
				Power_processData();

				if(current.sampleCount >= DCSampleCountMax)
				{
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
					{
						ADC_setChannel(2);
						ADC_state = 0;
						ADC_setDataReady();
					}
				}
			}
		}
		// AC mode
		else if (ADC_state == 3)
		{
			if (ADC_channel == 0)
			{
				ADC_processData(&voltageData, data);
				Signal_processData(&voltage, data);
			}
			else if (ADC_channel == 1)
			{
				ADC_processData(&currentData, data);
				Signal_processData(&current, data);
				Power_processData();

				if(current.sampleCount >= DCSampleCountMax || periodCount >= periodCountMax)
				{
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
					{
						ADC_setChannel(2);
						ADC_state = 0;
						ADC_setDataReady();
					}
				}
			}
		}
	}
}

// Voltage zero crossing
ISR(INT1_vect)
{
	uint64_t currentTime;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		currentTime = System_getTimeMicro();
	}
	voltage.lastPeriod = currentTime;
}

// Current zero crossing
ISR(INT0_vect)
{
	uint64_t currentTime;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		currentTime = System_getTimeMicro();
	}
	if (ADC_state == 2)
	{
		ADC_state = 3;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			Signal_clear(&voltage);
			Signal_clear(&current);
			Power_clear();

			Signal_processData(&voltage, voltageData.value);
			Signal_processData(&current, currentData.value);
			Power_processData();
		}
	}
	else if (ADC_state == 3)
	{
		periodCount++;
		periodTimeSum += currentTime - current.lastPeriod;
		VCTDSum += currentTime - voltage.lastPeriod;
	}
	current.lastPeriod = currentTime;
}
