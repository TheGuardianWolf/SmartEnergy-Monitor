/*
 * Interface.c
 *
 * Created: 18/08/2016 6:47:54 AM
 *  Author: Jerry Fan
 */

 #include "Interface.h"
 
 Interface::Interface() 
 {

 }
 
 void Interface::init()
 {
	setGpioPinModeOutput(LED);
	setGpioPinModeInput(SWITCH);
	
	setGpioPinLow(LED);
 }
