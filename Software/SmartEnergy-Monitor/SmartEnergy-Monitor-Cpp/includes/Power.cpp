/*
 * Power.cpp
 *
 * Created: 28/08/2016 5:13:17 PM
 *  Author: lichk
 */ 

 #include "Power.h"

 #include <util/atomic.h>

 Power::Power() :
 instantPower(0),
 lastNull(338),
 voltage(new Signal(0)),
 current(new Signal(1)),
 null(new Signal(2))
 {
 }

 void Power::processData()
 {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (this->voltage->isDataAvailable())
		{
			this->voltage->processData(this->lastNull);
		}
		if (this->current->isDataAvailable())
		{
			this->current->processData(this->lastNull);
		}
		if (this->null->isDataAvailable())
		{
			lastNull = null->getData().value;
			this->instantPower = this->voltage->getData().value * this->current->getData().value;
		}
	}
 }