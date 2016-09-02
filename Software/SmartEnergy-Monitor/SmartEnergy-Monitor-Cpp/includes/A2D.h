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

	virtual ~A2D();

	struct A2DData
	{
		unsigned long timestamp;
		uint16_t value;
	};

	static void init();

	bool isDataAvailable();

	void collect(uint16_t adc_data);

	A2DData getData();

	virtual bool processData(int16_t nullValue);

	void enableChannel();

	void disableChannel();

	protected:
	A2DData a2dData;

	bool dataAvailable;

	uint8_t channel;
};

#endif /* A2D_H_ */
