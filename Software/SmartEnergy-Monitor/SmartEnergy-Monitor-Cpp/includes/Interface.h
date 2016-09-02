/*
 * Interactive.h
 *
 * Created: 18/08/2016 6:48:07 AM
 *  Author: Jerry Fan
 */


#ifndef INTERFACE_H_
#define INTERFACE_H_

class Interface 
{
	public:
		Interface();
		/**
		 * Initialise the IO registers for reading/writing
		 */
		static void init();

		void ledOn();

		void ledOff();

		void ledToggle();

		bool buttonIsPressed();
};



#endif /* INTERFACE_H_ */
