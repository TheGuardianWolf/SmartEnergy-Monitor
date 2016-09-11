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
#include <string.h>
#include <util/atomic.h>

#define ARRAY_COUNT_MAX 20

static uint8_t arrayCount = 0;
static int32_t voltageSquaredSum = 0;
static int32_t voltageSquaredSumArray[ARRAY_COUNT_MAX];
static int32_t currentMaxSum = 0;
static int16_t currentMaxSumArray[ARRAY_COUNT_MAX];
static int32_t powerSum = 0;
static int32_t powerSumArray[ARRAY_COUNT_MAX];
static uint32_t periodDifferenceSum = 0;
static uint16_t periodDifferenceSumArray[ARRAY_COUNT_MAX];
static uint32_t periodSum = 0;
static uint16_t periodSumArray[ARRAY_COUNT_MAX];
static uint16_t sampleCountSum = 0;
static uint16_t sampleCountSumArray[ARRAY_COUNT_MAX];
static float sampleCountAverage = 0;
static const float vScale = 12.0;
static const float iScale = 0.30356;

static struct SignalData lastVoltageCopy;
static struct SignalData lastCurrentCopy;
static struct PowerData lastPowerCopy;
static uint16_t lastPeriodTimeSumCopy;
static int16_t lastVCTDSumCopy;

static void runningAverageClear()
{
	voltageSquaredSum = 0;
	currentMaxSum = 0;
	powerSum = 0;
	periodDifferenceSum = 0;
	periodSum = 0;
	memset(voltageSquaredSumArray, 0, sizeof(voltageSquaredSumArray));
	memset(currentMaxSumArray, 0, sizeof(currentMaxSumArray));
	memset(powerSumArray, 0, sizeof(powerSumArray));
	memset(periodDifferenceSumArray, 0, sizeof(periodDifferenceSumArray));
	memset(periodSumArray, 0, sizeof(periodSumArray));
}

static void runningAverageFill()
{
	Signal_mainDataReady = 0;
	lastVoltageCopy = lastVoltage;
	lastCurrentCopy = lastCurrent;
	lastPowerCopy = lastPower;
	lastPeriodTimeSumCopy = lastPeriodTimeSum;
	lastVCTDSumCopy = lastVCTDSum;

	voltageSquaredSum -= voltageSquaredSumArray[arrayCount];
	voltageSquaredSum += lastVoltageCopy.squared;
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
	sampleCountSum += lastPowerCopy.sampleCount;
	sampleCountSumArray[arrayCount] = lastPowerCopy.sampleCount;

	arrayCount++;

	if(arrayCount >= ARRAY_COUNT_MAX)
	{
		arrayCount = 0;
	}
}

void runningAverageSetDisplay()
{
	sampleCountAverage = (float) sampleCountSum / ARRAY_COUNT_MAX;
	Display_values[vRMS] = ADC_convertToVoltage(sqrt((float) voltageSquaredSum / (ARRAY_COUNT_MAX * sampleCountAverage))) * vScale; 
	Display_values[iMAX] = ADC_convertToVoltage((float) currentMaxSum / (ARRAY_COUNT_MAX * sampleCountAverage)) * iScale; 
	Display_values[pAVG] = ADC_convertToVoltage((float) powerSum / (ARRAY_COUNT_MAX * sampleCountAverage)) * vScale * iScale; 
	Display_values[frequency] = (float) (periodCountMax * ARRAY_COUNT_MAX) / periodSum;
	Display_values[phaseDifference] = ((float) periodSum / (periodCountMax * ARRAY_COUNT_MAX)) * 360 / ((float) periodDifferenceSum / (periodCountMax * ARRAY_COUNT_MAX));
}

int main(void)
{
	runningAverageClear();
	System_init();

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
