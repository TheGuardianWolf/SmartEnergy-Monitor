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

// ADC, SIGNAL, POWER

const uint8_t periodCountMax = 2;
const float ADC_sensitivity = 4.882813e-3;

volatile uint8_t ADC_state = 0;
volatile bool Signal_mainDataReady = false;
volatile uint16_t lastPeriodTimeSum = 0;
volatile int16_t lastVCTDSum = 0;
volatile struct PowerData lastPower = {0, -128, 127};
volatile struct SignalData lastVoltage = {0, 0, -1024, 1024, 0, 0};
volatile struct SignalData lastCurrent = {0, 0, -1024, 1024, 0, 0};

static int16_t data = 0;
static uint8_t ADC_channel = 2;
static int16_t nullVal = 338;
static const uint16_t DCSampleCountMax = 1024;
static uint32_t tempCurrentLastPeriod = 0;
static uint8_t periodCount = 0;
static uint16_t periodTimeSum = 0;
static int16_t voltageCurrentTimeDifferenceSum = 0;
static struct ADCData voltageData = {0, 0};
static struct ADCData currentData = {0, 0};
static struct SignalData voltage = {0, 0, -1024, 1024, 0, 0};
static struct SignalData current = {0, 0, -1024, 1024, 0, 0};
static uint8_t instantPower = 0;
static struct PowerData power = {0, -128, 127};


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

void ADC_processData(struct ADCData *storage, int16_t data)
{
	storage->timestamp = System_getTimeMicro();
	storage->value = data;
}

float ADC_convertToVoltage(int16_t adcValue)
{
	return adcValue * ADC_sensitivity;
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

void Power_processData()
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
}

void Power_clear()
{
	power.max = -128;
	power.min = 127;
	power.sum = 0;
}

ISR(ADC_vect)
{
		int16_t rawData = ADCL | (ADCH << 8);
	
		// initialization
		if (ADC_state == 0)
		{
			ADC_channel = 0;
			ADMUX &= 0b11100000;

			nullVal = 338;
			//nullVal = rawData;
			ADC_state = 1;
		}
		// DC mode
		else if (ADC_state == 1)
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

				voltage.sum += data;
			}
			else if (ADC_channel == 1)
			{
				ADC_processData(&currentData, data);

				current.sampleCount++;
				current.sum += data;
				
				Power_processData();
			}

			if(current.sampleCount > DCSampleCountMax) {
				lastVoltage = voltage;
				lastCurrent = current;
				lastPower = power;
				Signal_mainDataReady = true;
				periodCount = 0;
				Signal_clear(&voltage);
				Signal_clear(&current);
				Power_clear();
				ADC_channel = 2;
				ADC_state = 0;
				ADMUX &= 0b11111000;
				ADMUX |= ADC_channel;
			}
		}
		// AC mode
		else if (ADC_state == 2)
		{
			data = rawData - nullVal;

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
				voltage.sampleCount++;
				voltage.sum += data;
				voltage.squared += (uint32_t) ((int32_t) data * (int32_t) data);

				if(data > voltage.max)
				{
					voltage.max = data;
				}
				if(data < voltage.min)
				{
					voltage.min = data;
				}
			}
			else if (ADC_channel == 1)
			{
				ADC_processData(&currentData, data);
				
				current.sampleCount++;
				current.sum += data;
				current.squared += (uint32_t) ((int32_t) data * (int32_t) data);

				if(data > current.max)
				{
					current.max = data;
				}
				if(data < current.min)
				{
					current.min = data;
				}
				
				Power_processData();
			}
		}
}

ISR(INT1_vect) // Voltage zero crossing
{
	uint32_t currentTime = System_getTimeMicro();
	tempCurrentLastPeriod = voltage.lastPeriod;
	voltage.lastPeriod = currentTime;
}

ISR(INT0_vect) // Current zero crossing
{
	uint32_t currentTime = System_getTimeMicro();
	tempCurrentLastPeriod = current.lastPeriod;
	current.lastPeriod = currentTime;
		
	if (ADC_state == 1)
	{
		ADC_state = 2;

		Signal_clear(&voltage);
		Signal_clear(&current);
		Power_clear();

		voltage.sampleCount++;
		voltage.sum += voltageData.value;
		voltage.squared += (uint32_t) ((int32_t) voltageData.value * (int32_t) voltageData.value);
		if(voltageData.value > voltage.max)
		{
			voltage.max = voltageData.value;
		}
		if(voltageData.value < voltage.min)
		{
			voltage.min = voltageData.value;
		}

		current.sampleCount++;
		current.sum += voltageData.value;
		current.squared += (uint32_t) ((int32_t) currentData.value * (int32_t) currentData.value);
		if(currentData.value > current.max)
		{
			current.max = currentData.value;
		}
		if(currentData.value < current.min)
		{
			current.min = currentData.value;
		}

		Power_processData();
	}
	else if (ADC_state == 2)
	{
		periodCount++;
		periodTimeSum += currentTime - tempCurrentLastPeriod;
		voltageCurrentTimeDifferenceSum += current.lastPeriod - voltage.lastPeriod;
			
		if (periodCount >= periodCountMax)
		{
			lastPeriodTimeSum = periodTimeSum;
			lastVCTDSum = voltageCurrentTimeDifferenceSum;
			lastVoltage = voltage;
			lastCurrent = current;
			lastPower = power;
			Signal_mainDataReady = true;
			periodCount = 0;
			Signal_clear(&voltage);
			Signal_clear(&current);
			Power_clear();
			ADC_channel = 2;
			ADC_state = 0;
			ADMUX &= 0b11111000;
			ADMUX |= ADC_channel;
		}
	}
}