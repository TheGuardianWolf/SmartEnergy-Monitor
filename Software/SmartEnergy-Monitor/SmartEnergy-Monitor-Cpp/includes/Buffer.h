/* 
* Buffer.h
*
* Created: 2/09/2016 3:29:52 AM
* Author: lichk
*/


#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>

class Buffer
{
public:
	Buffer(uint8_t len);

	~Buffer();

	bool push(uint8_t);
	
	uint8_t pull();
	
	uint8_t peek();
	
	void clear();
	
	bool isFull();

	bool isEmpty();

private:
	uint8_t length;
	uint8_t *storage;
	volatile uint8_t head;
	volatile uint8_t tail;
	volatile uint8_t index;
};

#endif //__BUFFER_H__
