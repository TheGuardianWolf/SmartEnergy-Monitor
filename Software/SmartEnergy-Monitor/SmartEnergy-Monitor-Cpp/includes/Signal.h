/*
* Signal.h
*
* Created: 26/08/2016 12:28:54 PM
*  Author: lichk
*/


#ifndef SIGNAL_H_
#define SIGNAL_H_

#include "A2D.h"

class Signal : public A2D
{
	public:
	Signal(uint8_t ch);

	~Signal();

	struct SignalData
	{
		int16_t sum;
		int16_t max;
		int16_t min;
		uint32_t squared;
	};

	virtual bool processData(int16_t nullValue);

	bool comparatorSetPeriod();

	protected:
	uint8_t sampleCount;
	int16_t upperThreshold;
	int16_t lowerThreshold;
	SignalData currentData;
	SignalData storedData;
	unsigned long lastPeriod;
	bool waveDirection;

	private:
	void enableChannel();

	void disableChannel();
};


#endif /* SIGNAL_H_ */
