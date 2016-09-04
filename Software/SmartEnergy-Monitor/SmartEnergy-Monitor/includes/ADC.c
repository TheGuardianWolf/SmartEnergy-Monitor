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
#define WAVEFORM_UP 0
#define WAVEFORM_DOWN 1
#define WAVEFORM_UNDETERMINED 2

// ADC, SIGNAL, POWER

uint8_t ADC_state = 0;
bool Signal_mainDataReady = false;
uint32_t Signal_acSamplingStartTime = 0;
uint32_t Signal_acSamplingStopTime = 0;

const static float adcSensitivity = 4.882813e-3;

static uint8_t ADC_channel = 2;
static int16_t nullVal = 338;
static uint16_t sampleCount = 0;
static const uint16_t sampleCountMax = 2048;
static uint8_t periodCount = 0;
static uint8_t periodCountMax = 3;
static uint8_t initialACWaveDirection = WAVEFORM_UNDETERMINED;
static struct ADCData voltageData = {0, 0};
static struct ADCData currentData = {0, 0};
static struct SignalData voltage = {0, -1024, 1024, 0, WAVEFORM_UNDETERMINED, 0};
static struct SignalData current = {0, -1024, 1024, 0, WAVEFORM_UNDETERMINED, 0};
struct SignalData lastVoltage = {0, -1024, 1024, 0, WAVEFORM_UNDETERMINED, 0};
struct SignalData lastCurrent = {0, -1024, 1024, 0, WAVEFORM_UNDETERMINED, 0};
static uint8_t instantPower = 0;
static struct PowerData power = {0, -128, 127, 0};
struct PowerData lastPower = {0, -128, 127, 0};

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

//float ADC_convertToValue(int16_t adcValue)
//{
	//return adcValue * adcSensitivity;
//}

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
		
		// initialization
		if (ADC_state == 0)
		{
			ADC_channel = 0;
			ADMUX &= 0b11100000;

			nullVal = 338;//rawData;
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

				if(current.waveDirection == WAVEFORM_DOWN && data > SIGNAL_THRESHOLD) {
					Signal_acSamplingStartTime = System_getTimeMicro();
					current.waveDirection = WAVEFORM_UP;
					current.lastPeriod = currentData.timestamp;
					ADC_state = 2;
					Signal_clear(&voltage);
					Signal_clear(&current);
					Power_clear();

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

					current.squared += (uint32_t) ((int32_t) data * (int32_t) data);
					if(data > current.max)
					{
						current.max = data;
					}
					if(data < current.min)
					{
						current.min = data;
					}
				}
				else if(current.waveDirection == WAVEFORM_UP && data < -SIGNAL_THRESHOLD) {
					Signal_acSamplingStartTime = System_getTimeMicro();
					current.waveDirection = WAVEFORM_DOWN;
					current.lastPeriod = currentData.timestamp;
					ADC_state = 2;
					Signal_clear(&voltage);
					Signal_clear(&current);
					Power_clear();

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

					current.squared += (uint32_t) ((int32_t) data * (int32_t) data);
					if(data > current.max)
					{
						current.max = data;
					}
					if(data < current.min)
					{
						current.min = data;
					}
				}
				else {
					if(data > SIGNAL_THRESHOLD)
					{
						current.waveDirection = WAVEFORM_UP;
					}
					else if(data < -SIGNAL_THRESHOLD)
					{
						voltage.waveDirection = WAVEFORM_DOWN;
					}
				}
				current.sum += data;
							
				Power_processData(data);
							
				sampleCount++;
				if(sampleCount > sampleCountMax)
				{
					lastVoltage = voltage;
					lastCurrent = current;
					lastPower = power;
					Signal_mainDataReady = true;
					sampleCount = 0;
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
		// AC mode
		else if (ADC_state == 2)
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
				voltage.squared += (uint32_t) ((int32_t) data * (int32_t) data);

				if(data > voltage.max)
				{
					voltage.max = data;
				}
				if(data < voltage.min)
				{
					voltage.min = data;
				}

				// Zero crossing check, remove if using comparator
				if(voltage.waveDirection == WAVEFORM_DOWN && data > SIGNAL_THRESHOLD) {
					voltage.waveDirection = WAVEFORM_UP;
					if(initialACWaveDirection == WAVEFORM_UP)
					{
						voltage.lastPeriod = voltageData.timestamp;
					}
				}
				else if(voltage.waveDirection == WAVEFORM_UP && data < -SIGNAL_THRESHOLD) {
					voltage.waveDirection = WAVEFORM_DOWN;
					if(initialACWaveDirection == WAVEFORM_DOWN)
					{
						voltage.lastPeriod = voltageData.timestamp;
					}
				}
			}
			else if (ADC_channel == 1)
			{
				ADC_processData(&currentData, data);
				
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

				// Zero crossing check, remove if using comparator
				if(current.waveDirection == WAVEFORM_DOWN && data > SIGNAL_THRESHOLD) {
					current.waveDirection = WAVEFORM_UP;
					if(initialACWaveDirection == WAVEFORM_UP)
					{
						current.lastPeriod = voltageData.timestamp;
						periodCount++;
					}
				}
				else if(current.waveDirection == WAVEFORM_UP && data < -SIGNAL_THRESHOLD) {
					current.waveDirection = WAVEFORM_DOWN;
					if(initialACWaveDirection == WAVEFORM_DOWN)
					{
						current.lastPeriod = voltageData.timestamp;
						periodCount++;
					}
				}
				
				Power_processData(data);
				
				sampleCount++;
				if((sampleCount > sampleCountMax) || (periodCount > periodCountMax))
				{
					Signal_acSamplingStopTime = System_getTimeMicro();
					lastVoltage = voltage;
					lastCurrent = current;
					lastPower = power;
					Signal_mainDataReady = true;
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