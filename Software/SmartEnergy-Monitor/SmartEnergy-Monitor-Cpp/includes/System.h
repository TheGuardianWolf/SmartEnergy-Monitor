/*
 * System.h
 *
 * Created: 30/08/2016 11:11:03 PM
 *  Author: lichk
 */ 


#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>

namespace System {
	
	void init();

	uint32_t getTimeMicro();

	uint32_t getTimeMilli();

};




#endif /* SYSTEM_H_ */