/*
 * Interactive.c
 *
 * Created: 18/08/2016 6:47:54 AM
 *  Author: Jerry Fan
 */

 #include "Interactive.h"

 void io_init()
 {
	setGpioPinModeOutput(LED);
	setGpioPinModeInput(SWITCH);
	
	setGpioPinLow(LED);
 }
