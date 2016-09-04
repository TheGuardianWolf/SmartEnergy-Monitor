/*
 * SmartEnergy-Monitor.c
 * Code for the wireless energy monitor. Odd parity is enabled for transmission.
 * Group 10
 *
 * Created: 12/08/2016 12:38:02 PM
 * Author : Jerry Fan
 */

#include "includes/System.h"
#include "includes/ADC.h"
#include "includes/Display.h"

#include <stdbool.h>
#include <math.h>
#include <util/atomic.h>

static uint8_t arrayCount = 0;
static const uint8_t arrayCountMax = 50;
static int16_t voltageSquared = 0;
static int16_t voltageSquaredArray[50];
static int16_t currentSquared = 0;
static int16_t currentSquaredArray[50];
static int16_t powerSum = 0;
static int16_t powerSumArray[50];
static uint16_t periodDifferenceSum = 0;
static uint16_t periodDifferenceSumArray[50];
static uint16_t frequencySum = 0;
static uint16_t frequencySumArray[50];
static uint16_t sampleCountSum = 0;
static uint16_t sampleCountSumArray[50];
static float sampleCountAverage = 0;
static float vScale = 1;
static float iScale = 1;

void arrayClear(int16_t *array)
{
	for (uint8_t i = 0; i < arrayCountMax; i++)
	{
		array[i] = 0;
	}
}

void uArrayClear(uint16_t *array)
{
	for (uint8_t i = 0; i < arrayCountMax; i++)
	{
		array[i] = 0;
	}
}

int main(void)
{
	System_init();

	arrayClear(voltageSquaredArray);
	arrayClear(currentSquaredArray);
	arrayClear(powerSumArray);
	uArrayClear(periodDifferenceSumArray);
	uArrayClear(frequencySumArray);

	// The Loop
	for(;;)
	{
		if(Signal_mainDataReady)
		{
			TCNT1 = 0;

			voltageSquared -= voltageSquaredArray[arrayCount];
			voltageSquared += lastVoltage.squared / lastVoltage.sampleCount;
			voltageSquaredArray[arrayCount] = lastVoltage.squared;

			currentSquared -= currentSquaredArray[arrayCount];
			currentSquared += lastCurrent.squared;
			currentSquaredArray[arrayCount] = lastCurrent.squared;

			powerSum -= powerSumArray[arrayCount];
			powerSum += lastPower.sum;
			powerSumArray[arrayCount] = lastPower.sum;

			periodDifferenceSum -= periodDifferenceSumArray[arrayCount];
			periodDifferenceSum += lastPower.periodDifferenceSum;
			periodDifferenceSumArray[arrayCount] = lastPower.periodDifferenceSum;

			frequencySum -= frequencySumArray[arrayCount];
			frequencySum += lastFrequency;
			frequencySumArray[arrayCount] = lastFrequency;

			sampleCountSum -= sampleCountSumArray[arrayCount];
			sampleCountSum += lastCurrent.sampleCount;
			sampleCountSumArray[arrayCount] = lastCurrent.sampleCount;

			arrayCount++;

			if(arrayCount >= arrayCountMax)
			{
				arrayCount = 0;
			}

			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				sampleCountAverage = (float) sampleCountSum / arrayCountMax;
				Display_values.vRMS = sqrt((float) voltageSquared / (arrayCountMax * sampleCountAverage)) * vScale; 
				Display_values.iRMS = sqrt((float) currentSquared / (arrayCountMax * sampleCountAverage)) * iScale; 
				Display_values.pAVG = (float) powerSum / (arrayCountMax * sampleCountAverage) * vScale * iScale; 
			}

		}

	}
}
