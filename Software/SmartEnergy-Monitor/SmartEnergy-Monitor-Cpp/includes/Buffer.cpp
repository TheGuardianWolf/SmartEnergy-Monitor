/* 
* Buffer.cpp
*
* Created: 2/09/2016 3:29:52 AM
* Author: lichk
*/


#include "Buffer.h"
#include "lib/AVRTools/new.h"

// default constructor
Buffer::Buffer() :
length(16),
head(0),
tail(0),
index(0)
{
}

Buffer::~Buffer()
{
	delete this->storage;
}

bool Buffer::push(uint8_t data)
{
	if (!this->isFull())
	{
		this->storage[(this->head + this->index) & (this->length - 1)] = data;
		this->index++;
		return true;
	}
	return false;
}

uint8_t Buffer::pull()
{
	if (!this->isEmpty())
	{
		uint8_t data = this->storage[this->head];
		this->index--;
		this->head++;
		return data;
	}
}

uint8_t Buffer::peek()
{
	return this->storage[this->head];
}

void Buffer::clear()
{
	this->index = 0;
}

bool Buffer::isFull()
{
	if ((this->length  - this->index) == this->length)
	{
		return true;
	}
	return false;
}

bool Buffer::isEmpty()
{
	if ((this->length  - this->index) == 0)
	{
		return true;	
	}
	return false;
}
