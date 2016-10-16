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

// Initialise constants
const uint8_t periodCountMax = 25;
const double ADC_sensitivity = 4.882813e-3;

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

// Power approximation arrays.
static int16_t previousVoltages[3] = {0, 0, 0};
static int16_t previousCurrents[3] = {0, 0, 0};
static int32_t previousPower[2] = {0, 0};
static uint8_t powerInitCount = 0;

void ADC_init()
{
	ADMUX = (1 << REFS0) | (1 << MUX1);
	ADCSRB = 0;
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1);
}

void ADC_initComparators()
{
	DDRD &= ~(1 << DDD2) | (1 << DDD3);
	PORTD |= (1 << PORTD2) | (1 << PORTD3);
	EICRA = (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00);
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

void ADC_processData(struct ADCData *storage, int16_t *storageArray, int16_t data)
{
	storage->timestamp = System_getTimeMicro(); // Store the current time of the sample.
	storage->value = data;

	// Recording previous signal values for use in power approximation.
	// Push back other values.
	for (uint8_t i = 2; i > 0; i--)
	{
		storageArray[i - 1] = storageArray[i];
	}
	storageArray[2] = data;
}

double ADC_convertToVoltage(double adcValue)
{
	return adcValue * ADC_sensitivity;
}

void ADC_setDataReady()
{
	// Temporary storage of last read values.
	lastVoltage = voltage;
	lastCurrent = current;
	lastPower = power;
	lastPeriodTimeSum = periodTimeSum;
	lastVCTDSum = VCTDSum;

	// Turn on flag to indicate data ready.
	ADC_dataReady = true;

	// Reset all arrays, structs, variables.
	periodTimeSum = 0;
	VCTDSum = 0;
	periodCount = 0;
	powerInitCount = 0;
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
	// Power approximation
	previousPower[0] = previousPower[1];
	previousPower[1] = ((previousVoltages[0] + previousVoltages[2]) / 2) * ((previousCurrents[0] + previousCurrents[2]) / 2);

	// There needs to be 3 power 'readings' before the approximate power actually reflects the real power.
	// The arrays need to first fill up.
	if (powerInitCount > 3)
	{
		int32_t approxPower = (previousPower[0] + previousPower[1]) / 2;

		power.sum += approxPower;
		if (approxPower > power.max)
		{
			power.max = approxPower;
		}
		if (approxPower < power.min)
		{
			power.min = approxPower;
		}

		power.sampleCount++;
	}
	else
	{
		powerInitCount++;
	}
}

void Power_clear()
{
	power.sampleCount = 0;
	power.max = INT_MIN;
	power.min = INT_MAX;
	power.sum = 0;
	powerInitCount = 0;
}

void ADC_runStateMachine()
{

}

// To simplify timings, a lot of code was placed in this ISR. Due to ISR priority,
// other interrupts can still fire so timing is still preserved for timing
// critical operations in other ISRs.
ISR(ADC_vect)
{
	// Block when reading the ADC value.
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
		//nullVal = 338;
		nullVal = rawData; // 1.65 V pin is our reference.
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
				ADC_processData(&voltageData, &previousVoltages, data);
				Signal_processData(&voltage, data);
			}
			else if (ADC_channel == 1)
			{
				ADC_processData(&currentData, &previousCurrents data);
				Signal_processData(&current, data);
				Power_processData();

				if(current.sampleCount >= DCSampleCountMax)
				{
					ADC_setChannel(2);					
					ADC_state = 0;
					ADC_setDataReady();
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

				// AC mode will end when the max period count is reached, rather than
				// being based on a sample count. This is more accurate for AC
				// measurements.
				if(current.sampleCount >= DCSampleCountMax || periodCount >= periodCountMax)
				{
					ADC_setChannel(2);
					ADC_state = 0;
					ADC_setDataReady();
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
	// Block when reading time.
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		currentTime = System_getTimeMicro();
	}

	// If in DC mode, clear everything except latest reading and switch to AC mode.
	if (ADC_state == 2)
	{
		ADC_state = 3;
		// The following has to be done immediately on mode switch, so it goes into this ISR. There are better ways to go about this
		// but this is what works at the moment.
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) // ADC interrupt must NOT fire before this.
		{
			Signal_clear(&voltage);
			Signal_clear(&current);
			Power_clear();

			Signal_processData(&voltage, voltageData.value);
			Signal_processData(&current, currentData.value);
			Power_processData();
		}
	}
	// If in AC mode, increase the period count.
	else if (ADC_state == 3)
	{
		periodCount++;

		// Calculate the time of one period and the phase difference.
		periodTimeSum += currentTime - current.lastPeriod;
		VCTDSum += currentTime - voltage.lastPeriod;
	}
	// Record the current time as the last period time.
	current.lastPeriod = currentTime;
}
