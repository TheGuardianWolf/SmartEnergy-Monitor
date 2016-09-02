/*
 * Power.h
 *
 * Created: 28/08/2016 5:13:09 PM
 *  Author: lichk
 */ 


#ifndef POWER_H_
#define POWER_H_

#include <stdint.h>
#include "includes/Signal.h"

class Power
{
	public:
	Power();

	void processData();

	protected:
	int16_t instantPower;
	uint16_t lastNull;
	Signal *voltage;
	Signal *current;
	Signal *null;
};

#endif /* POWER_H_ */