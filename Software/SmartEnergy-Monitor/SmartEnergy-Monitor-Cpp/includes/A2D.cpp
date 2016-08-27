/*
* A2D.c
*
* Created: 24/08/2016 6:22:34 PM
*  Author: lichk
*/

#include "A2D.h"
#include <stddef.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "lib/AVRTools/SystemClock.h"
#include "lib/AVRTools/Analog2Digital.h"
#include "lib/AVRTools/RingBuffer.h"

#ifndef ADC_BUFFER_SIZE
#define ADC_BUFFER_SIZE   8
#endif

// Uncomment to use comparator
// #define __USING_COMPARATOR__ true

namespace
{
	uint8_t currentChannel = 0;
	int16_t lastNullValue = 338;
	Signal *activeSignals[8] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
}

A2D::A2D(uint8_t ch = 0)
	:channel(ch),
	dataAvailable(false)
{
	a2dData.timestamp = 0;
	a2dData.value = 0;
}

void A2D::init()
{
	//initA2D(kA2dReferenceAVCC, kA2dPrescaleDiv64);
	if (true)
	{
		ADMUX &= 0b11011111;
		ADMUX |= 0b01000000;
		ADMUX &= 0b11110000;
		ADCSRA |= 0b10000000;
		ADCSRB &= 0b11111000;
		ADCSRA |= 0b00000111;

		ADCSRA |= (1 << ADIE);
		ADCSRA |= (1 << ADATE);
		ADCSRA |= (1 << ADSC);
	}
}

void A2D::collect(uint16_t adcData)
{
	this->a2dData.timestamp = micros();
	this->a2dData.value =  adcData;
	this->dataAvailable = true;
}

void A2D::setAsZeroReference()
{
	lastNullValue = this->a2dData.value;
}

A2D::A2DData A2D::getData()
{
	dataAvailable = false;
	return a2dData;
}

A2D::A2DData A2D::getDataBlocking()
{
	A2D::A2DData data;
	data.timestamp = micros();
	data.value = readA2D(this->channel);
	return data;
}

Signal::Signal(uint8_t ch = 0)
:A2D(ch),
sampleCount(0),
upperThreshold(10),
lowerThreshold(10),
lastPeriod(0),
waveDirection(false)
{
	this->enableChannel();
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
	this->currentData.squared += (uint32_t) ((int32_t) relVal * (int32_t) relVal);

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
	this->lastPeriod = micros();
	return true;
}

void Signal::enableChannel()
{
	activeSignals[this->channel] = this;
}

void Signal::disableChannel()
{
	activeSignals[this->channel] = NULL;
}

ISR(ADC_vect)
{
	uint16_t adcData = ADCL;
	adcData += (ADCH << 8);
	activeSignals[currentChannel]->collect(adcData);
	activeSignals[currentChannel]->processData();

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