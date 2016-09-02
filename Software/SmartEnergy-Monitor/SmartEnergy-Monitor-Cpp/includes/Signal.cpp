/*
* Signal.cpp
*
* Created: 26/08/2016 12:29:22 PM
*  Author: lichk
*/

#include "Signal.h"

#include <stddef.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "System.h"

#define DEFAULT_NULL 338 // 1024/5*1.65

// #define USING_COMPARATOR 1

Signal::Signal(uint8_t ch = 0)
:A2D(ch),
sampleCount(0),
upperThreshold(10),
lowerThreshold(10),
lastPeriod(0),
waveDirection(false)
{
	currentData.sum = 0;
	currentData.squared = 0;
	currentData.max = -1024;
	currentData.min = 1024;
	storedData.sum = 0;
	storedData.squared = 0;
	storedData.max = -1024;
	storedData.min = 1024;
}

Signal::~Signal()
{
	A2D::disableChannel();
}


bool Signal::processData(int16_t nullValue = 338)
{
	// TODO: System time overflow protection
	// Simply find maximum microsecond count and subtract from start time
	// and add end time if overflow is detected.

	this->sampleCount++;
	int16_t relVal = this->a2dData.value - nullValue;

	// Signal measurements
	this->currentData.sum += relVal;
	this->currentData.squared += (uint32_t) ((int32_t) relVal * (int32_t) relVal);

	if(relVal > this->currentData.max)
	{
		this->currentData.max = relVal;
	}
	if(relVal < this->currentData.min)
	{
		this->currentData.min = relVal;
	}

	#ifndef USING_COMPARATOR

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
	this->lastPeriod = System::getTimeMicro();
	return true;
}