/*
* A2D.c
*
* Created: 24/08/2016 6:22:34 PM
*  Author: lichk
*/

#include "A2D.h"
#include "lib/AVRTools/Analog2Digital.h"


A2D::A2D(uint8_t ch = 0)
{
	this->channel = ch;
	this->dataPointer = 0;
}

void A2D::init()
{
	initA2D(kA2dReferenceAVCC, kA2dPrescaleDiv64);
}

void A2D::read()
{
	this->data[this->dataPointer] = readA2D(this->channel);
	this->incrementDataPointer();
}

uint16_t* A2D::getData()
{
	return this->data;
}

void A2D::incrementDataPointer()
{
	if (this->dataPointer < 20)
	{
		this->dataPointer++;
	}
	else
	{
		this->dataPointer = 0;
	}
}