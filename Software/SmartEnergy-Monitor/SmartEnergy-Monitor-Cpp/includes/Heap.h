/*
 * Heap.h
 *
 * Created: 30/08/2016 11:18:41 PM
 *  Author: lichk
 */ 

#ifndef HEAP_H_
#define HEAP_H_

#include <stdlib.h>

void* operator new(size_t size);

void* operator new[](size_t size);

void operator delete(void* pointer);

void operator delete[](void* pointer);


#endif /* HEAP_H_ */