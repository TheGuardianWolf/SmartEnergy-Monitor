/*
 * Power.h
 *
 * Created: 28/08/2016 5:13:09 PM
 *  Author: lichk
 */ 


#ifndef POWER_H_
#define POWER_H_

#include <stdint.h>
#include "A2D.h"

class Power
{
	public:
	Power();

	void processData();

	protected:
	int16_t instantPower;
	//Signal voltage(0);
	//Signal current(1);
	//Signal nullVoltage(2);
};

#endif /* POWER_H_ */