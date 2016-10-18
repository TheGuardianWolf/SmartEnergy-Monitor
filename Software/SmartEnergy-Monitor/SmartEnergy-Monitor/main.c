/*
* SmartEnergy-Monitor.c
* Code for the wireless energy monitor. Odd parity is enabled for transmission.
* Code was ported over from a C++ implementation by using static in place of
* private namespaces as ISRs were too difficult to use in C++.
* Group 10
*
* Created: 12/08/2016 12:38:02 PM
* Author : Jerry Fan
*/

#include "includes/System.h"
#include "includes/ADC.h"
#include "includes/UART.h"
#include "includes/Display.h"
#include "includes/Interface.h"

#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <util/atomic.h>

// Secondary averaging maximum samples.
#define ARRAY_COUNT_MAX 50
#define RATED_POWER 8.5

// Scale values load values / circuit output. Last revised 17/10/16.
static const double vScale = 14.89; // Ratio of ADC input rms voltage to load rms voltage. Adjust by +- 0.1.
static const double iScale = 0.71; // Ratio of ADC input max current to load max current. Adjust by +- 0.01.

// Button VScale adjust.
static uint8_t scaleAdjustState = 0;
static double vScaleAdjust = 0.0; 
static double iScaleAdjust = 0.00; 

// Optimized copy of data (Non volatile).
static struct SignalData lastVoltageCopy;
static struct SignalData lastCurrentCopy;
static struct PowerData lastPowerCopy;
static uint16_t lastPeriodTimeSumCopy;
static int16_t lastVCTDSumCopy;

// Secondary averaging values.
static uint8_t arrayCount = 0;
static uint32_t voltageSquaredSum = 0;
static uint32_t voltageSquaredSumArray[ARRAY_COUNT_MAX];
static int32_t currentMaxSum = 0;
static int16_t currentMaxSumArray[ARRAY_COUNT_MAX];
static int64_t powerSum = 0;
static int32_t powerSumArray[ARRAY_COUNT_MAX];
static uint32_t periodDifferenceSum = 0;
static uint16_t periodDifferenceSumArray[ARRAY_COUNT_MAX];
static uint32_t periodSum = 0;
static uint16_t periodSumArray[ARRAY_COUNT_MAX];
static uint16_t sampleCountSum = 0;
static uint16_t sampleCountSumArray[ARRAY_COUNT_MAX];

/**
 * Initialise the system.
 * Turns on ADC, Display, Interface, and UART.
 * Extends the System namespace.
 */
void System_init()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		System_initClock();
		ADC_init();
		ADC_initComparators();
		Interface_init();
		UART_init();
		Display_init();
	}
}

/**
* Clears the secondary averaging arrays and variables.
*/
void runningAverageClear()
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

/**
* Copies over volatile variables to optimised variables and processes the
* secondary averaging. Smoothens display transitions and any outlying
* measurement samples.
*/
void runningAverageFill()
{
	// Copy volatile variables to optimised variables for R/W.
	lastVoltageCopy = lastVoltage;
	lastCurrentCopy = lastCurrent;
	lastPowerCopy = lastPower;
	lastPeriodTimeSumCopy = lastPeriodTimeSum;
	lastVCTDSumCopy = lastVCTDSum;

	// Average the voltage squared values over each measurement sample (20 periods or 512 samples depending on AC/DC mode).
	voltageSquaredSum -= voltageSquaredSumArray[arrayCount];
	voltageSquaredSum += lastVoltageCopy.squared;
	voltageSquaredSumArray[arrayCount] = lastVoltageCopy.squared;

	// Average the current max values over each measurement sample (20 periods or 512 samples depending on AC/DC mode).
	currentMaxSum -= currentMaxSumArray[arrayCount];
	currentMaxSumArray[arrayCount] = lastCurrentCopy.max - lastCurrentCopy.min; // Make use of the min samples to smooth out circuit effects.
	currentMaxSum += currentMaxSumArray[arrayCount]; 

	// Average the voltage squared values over each measurement sample (20 periods or 512 samples depending on AC/DC mode).
	powerSum -= powerSumArray[arrayCount];
	powerSum += lastPowerCopy.sum;
	powerSumArray[arrayCount] = lastPowerCopy.sum;

	// Calculation of period not required and is not working correctly anyway.
	// periodDifferenceSum -= periodDifferenceSumArray[arrayCount];
	// periodDifferenceSum += lastVCTDSumCopy;
	// periodDifferenceSumArray[arrayCount] = lastVCTDSumCopy;
	//
	// periodSum -= periodSumArray[arrayCount];
	// periodSum += lastPeriodTimeSum;
	// periodSumArray[arrayCount] = lastPeriodTimeSum;

	// Sample counts may differ slightly over measurement samples, so to smooth over values we can take the average amount of samples.
	sampleCountSum -= sampleCountSumArray[arrayCount];
	sampleCountSum += lastPowerCopy.sampleCount;
	sampleCountSumArray[arrayCount] = lastPowerCopy.sampleCount;

	// Cycle through the averaging array for a running average (hence the name).
	arrayCount++;
	if (arrayCount >= ARRAY_COUNT_MAX)
	{
		arrayCount = 0;
	}
}

/**
* Populates display values array with their respective values.
*/
void runningAverageSetDisplay()
{
	// RMS Voltage calculation by dividing the squared voltage by the number of 
	// individual voltage samples taken over the 50 secondary averaging samples.
	// This is then converted to a floating point input voltage at the ADC then
	// adjusted for circuit gain using vScale.
	Display_values[vRMS] = ADC_convertToVoltage(sqrt((double) voltageSquaredSum / sampleCountSum)) * (vScale + vScaleAdjust);

	// Max current calculation by dividing the max current by the number of
	// secondary samples as each secondary sample only contributes one max value
	// sample. This is then converted to a floating point input voltage at the ADC 
	// then adjusted for circuit gain using iScale.
	Display_values[iMAX] = ADC_convertToVoltage((double) currentMaxSum / (ARRAY_COUNT_MAX) / 2) * (iScale + iScaleAdjust);

	// Average power calculation by dividing the total power sum by the number of
	// individual power samples taken over the 50 secondary averaging samples.
	// This is then converted to a floating point input voltage twice as we need
	// to reverse both the voltage AND current reading's scaling. This is then
	// adjusted for circuit gain of both the voltage sensing and current sensing
	// parts.
	Display_values[pAVG] = ADC_convertToVoltage(ADC_convertToVoltage((double) powerSum / sampleCountSum)) * (vScale + vScaleAdjust) * (iScale+ iScaleAdjust);

	// Timing based measurements disabled (WONTFIX).
	// Display_values[frequency] = ((float) (periodCountMax * ARRAY_COUNT_MAX) / periodSum) / 1000000;
	// Display_values[phaseDifference] = ((float) periodSum / (periodCountMax * ARRAY_COUNT_MAX)) * 360 / ((float) periodDifferenceSum / (periodCountMax * ARRAY_COUNT_MAX));
}

/**
 * Controls the LED blinking based on the current power value. Based on a state
 * machine model. This is one part of the 'datapath' of the Interface state
 * machine.
 */
void runningAverageSetLED()
{
	double power = Display_values[pAVG];

  // Convert power to an absolute value.
	if (power < 0)
	{
		power = -power;
	}

  // If power is <= 25%
	if (power <= (RATED_POWER * 0.25))
	{
		Interface_ledState = 0;
	}
  // If power is > 25% but <= 50%
	else if (power <= (RATED_POWER * 0.5))
	{
		Interface_ledState = 1;
	}
  // If power is > 50% but <= 75%
	else if (power <= (RATED_POWER * 0.75))
	{
		Interface_ledState = 2;
	}
	else
	{
		Interface_ledState = 3;
	}
}

/**
 * This is for convenience in debugging, testing scaling values slightly higher or 
 * lower than the defined value is tedious as we have to re-program the microcontroller
 * each time.
 */

void buttonAdjustScale()
{
	if (Interface_buttonDebounceState == 2)
	{
		scaleAdjustState++;
		Interface_buttonDebounceState = 0;
	}
	if (scaleAdjustState == 0)
	{
		vScaleAdjust = 0.0;
		iScaleAdjust = 0.00;
	}
	else if (scaleAdjustState == 1)
	{
		vScaleAdjust = 0.2;
		iScaleAdjust = 0.02;
	}
	else if (scaleAdjustState == 2)
	{
		vScaleAdjust = -0.2;
		iScaleAdjust = -0.02;
	}
	else 
	{
		scaleAdjustState = 0;
	}	
}


int main(void)
{
	// Clear secondary averaging arrays and initialise the system.
	runningAverageClear();
	System_init();

	// The Loop.
	while(1)
	{
		// Process recieved data when it is ready.
		if(ADC_dataReady)
		{
			ADC_dataReady = 0; // Remove the ready flag.

			runningAverageFill();
			runningAverageSetDisplay();
			runningAverageSetLED();
		}

		// Run the state machines.
		Interface_runStateMachine();
		Display_runStateMachine();

		// Debug assistants.
		buttonAdjustScale();
	}
}
