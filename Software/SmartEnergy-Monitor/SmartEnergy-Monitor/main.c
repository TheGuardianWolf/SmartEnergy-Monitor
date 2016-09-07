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

static void runningAverageClear()
{
	voltageSquaredSum = 0;
	currentMaxSum = 0;
	powerSum = 0;
	periodDifferenceSum = 0;
	periodSum = 0;
	arrayClear(voltageSquaredSumArray);
	arrayClear(currentMaxSumArray);
	arrayClear(powerSumArray);
	uArrayClear(periodDifferenceSumArray);
	uArrayClear(periodSumArray);
}

static void runningAverageFill()
{
	Signal_mainDataReady = 0;
	struct SignalData lastVoltageCopy = lastVoltage;
	struct SignalData lastCurrentCopy = lastCurrent;
	struct PowerData lastPowerCopy = lastPower;
	int16_t lastVCTDSumCopy = lastVCTDSum;

	voltageSquaredSum -= voltageSquaredSumArray[arrayCount];
	voltageSquaredSum += lastVoltageCopy.squared / lastVoltageCopy.sampleCount;
	voltageSquaredSumArray[arrayCount] = lastVoltageCopy.squared;

	currentMaxSum -= currentMaxSumArray[arrayCount];
	currentMaxSum += lastCurrentCopy.max;
	currentMaxSumArray[arrayCount] = lastCurrentCopy.max;

	powerSum -= powerSumArray[arrayCount];
	powerSum += lastPowerCopy.sum;
	powerSumArray[arrayCount] = lastPowerCopy.sum;

	periodDifferenceSum -= periodDifferenceSumArray[arrayCount];
	periodDifferenceSum += lastVCTDSumCopy;
	periodDifferenceSumArray[arrayCount] = lastVCTDSumCopy;

	periodSum -= periodSumArray[arrayCount];
	periodSum += lastPeriodTimeSum;
	periodSumArray[arrayCount] = lastPeriodTimeSum;

	sampleCountSum -= sampleCountSumArray[arrayCount];
	sampleCountSum += lastCurrentCopy.sampleCount;
	sampleCountSumArray[arrayCount] = lastCurrentCopy.sampleCount;

	arrayCount++;

	if(arrayCount >= arrayCountMax)
	{
		arrayCount = 0;
	}
}

void runningAverageSetDisplay()
{
	sampleCountAverage = (float) sampleCountSum / arrayCountMax;
	Display_values.vRMS = ADC_convertToVoltage(sqrt((float) voltageSquaredSum / (arrayCountMax * sampleCountAverage))) * vScale; 
	Display_values.iMAX = ADC_convertToVoltage((float) currentMaxSum / (arrayCountMax * sampleCountAverage)) * iScale; 
	Display_values.pAVG = ADC_convertToVoltage((float) powerSum / (arrayCountMax * sampleCountAverage)) * vScale * iScale; 
	Display_values.frequency = (float) (periodCountMax * arrayCountMax) / periodSum;
	Display_values.phaseDifference = ((float) periodSum / (periodCountMax * arrayCountMax)) * 360 / ((float) periodDifferenceSum / (periodCountMax * arrayCountMax));
}

int main(void)
{
	System_init();

	runningAverageClear();

	// The Loop
	while(1)
	{
		if(Signal_mainDataReady)
		{
			runningAverageFill();
			runningAverageSetDisplay();
		}

	}
}
