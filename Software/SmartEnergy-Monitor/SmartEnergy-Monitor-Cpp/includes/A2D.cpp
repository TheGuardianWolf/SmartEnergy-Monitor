/*
* A2D.c
*
* Created: 24/08/2016 6:22:34 PM
*  Author: lichk
*/

#include "A2D.h"

#include "System.h"
#include <avr/interrupt.h>

using namespace System;

namespace
{
	volatile uint8_t currentChannel = 0;
	A2D *activeA2Ds[8] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
}

A2D::A2D(uint8_t ch = 0) :
	dataAvailable(false),
	channel(ch)
{
	this->enableChannel();
	a2dData.timestamp = 0;
	a2dData.value = 0;
}

A2D::~A2D()
{
	this->disableChannel();
}

void A2D::init()
{
	ADMUX = (1 << REFS0);
	ADCSRB = 0;
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1);
}

void A2D::collect(uint16_t adcData)
{
	this->a2dData.timestamp = getTimeMicro();
	this->a2dData.value =  adcData;
	this->dataAvailable = true;
}

bool A2D::isDataAvailable()
{
	return dataAvailable;
}

A2D::A2DData A2D::getData()
{
	dataAvailable = false;
	return a2dData;
}

bool A2D::processData(int16_t nullValue = 338)
{
	return false;
}

void A2D::enableChannel()
{
	activeA2Ds[this->channel] = this;
}

void A2D::disableChannel()
{
	activeA2Ds[this->channel] = NULL;
}

ISR(ADC_vect)
{
	uint16_t adcData = ADCL;
	uint8_t channel = currentChannel;
	adcData += (ADCH << 8);
	activeA2Ds[currentChannel]->collect(adcData);

	while (currentChannel < 8 && !activeA2Ds[currentChannel])
	{
		currentChannel++;
		if (activeA2Ds[currentChannel])
		{
			break;
		}
	}
	currentChannel &= 7;
}
