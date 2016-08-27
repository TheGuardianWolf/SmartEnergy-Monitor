/*
* Signal.cpp
*
* Created: 26/08/2016 12:29:22 PM
*  Author: lichk
*/

#include "Signal.h"
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "lib/AVRTools/SystemClock.h"

#define DEFAULT_NULL 338 // 1024/5*1.65

// Uncomment to use comparator
// #define __USING_COMPARATOR__ true

Signal::Signal(uint8_t ch = 0)
:A2D(ch),
sampleCount(0),
upperThreshold(10),
lowerThreshold(10),
lastPeriod(ULONG_MAX),
waveDirection(false)
{
	this->enableChannel();
	currentData.max = -1024;
	currentData.min = 1024;
}

Signal::~Signal()
{
	this->disableChannel();
};


bool Signal::processData(int16_t nullValue = 338)
{
	// TODO: System time overflow protection
	// Simply find maximum microsecond count and subtract from start time
	// and add end time if overflow is detected.

	this->sampleCount++;
	int16_t relVal = this->a2dData.value - nullValue;

	// Signal measurements
	this->currentData.sum += relVal;
	this->currentData.squared += (uint16_t)(relVal * relVal);

	if(relVal > this->currentData.max)
	{
		this->currentData.max = relVal;
	}
	if(relVal < this->currentData.min)
	{
		this->currentData.min = relVal;
	}
	
	#ifndef __USING_COMPARATOR__

	// Zero crossing check
	if(this->waveDirection == true && relVal > this->upperThreshold) {
		this->waveDirection = false;
		this->lastPeriod = this->a2dData.timestamp;
	}
	else if(this->waveDirection == false && relVal < this->lowerThreshold) {
		this->waveDirection = true;
	}
	return true;

	#endif
}

bool Signal::comparatorSetPeriod()
{
	//this->lastPeriod = micros();
	return true;
}

ISR(ADC_vect)
{
	uint16_t adcData = ADCL;
	adcData += (ADCH << 8);
	activeSignals[currentChannel]->collect(adcData);

	while (currentChannel < 8 && !activeSignals[currentChannel])
	{
		currentChannel++;
		if (activeSignals[currentChannel])
		{
			break;
		}
	}
	currentChannel &= 0x08;
}