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
static int16_t voltageSquaredSum = 0;
static int16_t voltageSquaredSumArray[50];
static int16_t currentMaxSum = 0;
static int16_t currentMaxSumArray[50];
static int16_t powerSum = 0;
static int16_t powerSumArray[50];
static uint16_t periodDifferenceSum = 0;
static uint16_t periodDifferenceSumArray[50];
static uint16_t periodSum = 0;
static uint16_t periodSumArray[50];
static uint16_t sampleCountSum = 0;
static uint16_t sampleCountSumArray[50];
static float sampleCountAverage = 0;
static const float vScale = 12.0;
static const float iScale = 0.30356;

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

	arrayClear(voltageSquaredSumArray);
	arrayClear(currentMaxSumArray);
	arrayClear(powerSumArray);
	uArrayClear(periodDifferenceSumArray);
	uArrayClear(periodSumArray);

	// The Loop
	while(1)
	{
		if(Signal_mainDataReady)
		{
			voltageSquaredSum -= voltageSquaredSumArray[arrayCount];
			voltageSquaredSum += lastVoltage.squared / lastVoltage.sampleCount;
			voltageSquaredSumArray[arrayCount] = lastVoltage.squared;

			currentMaxSum -= currentMaxSumArray[arrayCount];
			currentMaxSum += lastCurrent.max;
			currentMaxSumArray[arrayCount] = lastCurrent.max;

			powerSum -= powerSumArray[arrayCount];
			powerSum += lastPower.sum;
			powerSumArray[arrayCount] = lastPower.sum;

			periodDifferenceSum -= periodDifferenceSumArray[arrayCount];
			periodDifferenceSum += lastVoltageCurrentTimeDifferenceSum;
			periodDifferenceSumArray[arrayCount] = lastVoltageCurrentTimeDifferenceSum;

			periodSum -= periodSumArray[arrayCount];
			periodSum += lastPeriodTimeSum;
			periodSumArray[arrayCount] = lastPeriodTimeSum;

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
				Display_values.vRMS = ADC_convertToVoltage(sqrt((float) voltageSquaredSum / (arrayCountMax * sampleCountAverage))) * vScale; 
				Display_values.iMAX = ADC_convertToVoltage((float) currentMaxSum / (arrayCountMax * sampleCountAverage)) * iScale; 
				Display_values.pAVG = ADC_convertToVoltage((float) powerSum / (arrayCountMax * sampleCountAverage)) * vScale * iScale; 
				Display_values.frequency = (float) (periodCountMax * arrayCountMax) / periodSum;
				Display_values.phaseDifference = ((float) periodSum / (periodCountMax * arrayCountMax)) * 360 / ((float) periodDifferenceSum / (periodCountMax * arrayCountMax));
			}
			Signal_mainDataReady = 0;

		}

	}
}
