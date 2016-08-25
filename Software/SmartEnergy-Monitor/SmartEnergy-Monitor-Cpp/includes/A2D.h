/*
 * Sensors.h
 *
 * Created: 24/08/2016 6:22:22 PM
 *  Author: lichk
 */ 


#ifndef A2D_H_
#define A2D_H_

// F_CPU should be defined in compile flags
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "lib/AVRTools/ArduinoPins.h"
#include "lib/AVRTools/Analog2Digital.h"
#include "lib/timer/timer.h"

class A2D {
public:
	A2D(uint8_t ch);

	struct A2DData
	{
		time_t timestamp;
		uint16_t value;
	};

	static void init();

	void read();

	uint16_t* getData();
private:
	void incrementDataPointer();

	uint8_t channel;
	uint16_t data[20];
	uint8_t dataPointer;
};

#endif /* A2D_H_ */