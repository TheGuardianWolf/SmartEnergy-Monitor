/*
 * Sensors.h
 *
 * Created: 24/08/2016 6:22:22 PM
 *  Author: lichk
 */ 


#ifndef A2D_H_
#define A2D_H_

#include <stdint.h>
#include <stddef.h>

class A2D 
{
	public:
	A2D(uint8_t ch);

	~A2D();

	struct A2DData
	{
		unsigned long timestamp;
		uint16_t value;
	};

	static void init();

	bool dataAvailable;

	void collect(uint16_t adc_data);

	void setAsZeroReference();

	A2DData getData();

	A2DData getDataBlocking();

	protected:
	A2DData a2dData;

	uint8_t channel;
};

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

class Power
{
	public:
	Power();
	protected:
	int16_t instantPower;
	//Signal voltage(0);
	//Signal current(1);
};

#endif /* A2D_H_ */