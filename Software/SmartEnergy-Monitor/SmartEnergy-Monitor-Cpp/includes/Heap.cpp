/*
 * Heap.cpp
 *
 * Created: 30/08/2016 11:18:21 PM
 *  Author: lichk
 */ 

 /*
 This class provides some of the basic C++ keywords lacking in Embedded C++.

 Implementation based on the following sources:
 http://www.avrfreaks.net/forum/c-new-delete-operator-confusion?name=PNphpBB2&file=viewtopic&t=44295
 http://igormiktor.github.io/AVRTools/

 */

 #include "Heap.h"

 void* operator new(size_t size)
 {
	 if (size == 0)
	 {
		 size = 1;
	 }
	 return malloc(size);
 }

 void* operator new[](size_t size)
 {
	 if (size == 0)
	 {
		 size = 1;
	 }
	 return malloc(size);
 }

 void operator delete(void* pointer)
 {
	 free(pointer);
 }

 void operator delete[](void* pointer)
 {
	 free(pointer);
 }