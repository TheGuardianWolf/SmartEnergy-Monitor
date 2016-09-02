/*
 * Interactive.h
 *
 * Created: 18/08/2016 6:48:07 AM
 *  Author: Jerry Fan
 */


#ifndef INTERACTIVE_H_
#define INTERACTIVE_H_

#define LED_ON PORTB |=  (1<<PORTB5)
#define LED_OFF PORTB &= ~(1<<PORTB5)
#define SWITCH_DOWN !(PINB & (1<<PB7))

/**
 * Initialise the IO registers for reading/writing
 */
void Interface_init();



#endif /* INTERACTIVE_H_ */
