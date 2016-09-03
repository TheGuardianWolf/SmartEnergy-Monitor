/*
 * Interactive.h
 *
 * Created: 18/08/2016 6:48:07 AM
 *  Author: Jerry Fan
 */


#ifndef INTERACTIVE_H_
#define INTERACTIVE_H_

#include <stdbool.h>

/**
 * Initialise the IO registers for reading/writing
 */
void Interface_init();

void Interface_ledOn();

void Interface_ledOff();

void Interface_ledToggle();

bool Interface_buttonIsPressed();

#endif /* INTERACTIVE_H_ */
