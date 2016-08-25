/*
    Analog2Digital.cpp - A library for analog-to-digital conversions.
    For AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega).
    This is part of the AVRTools library.
    Copyright (c) 2014 Igor Mikolic-Torreira.  All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/*

	File modified on 25/08/16 by Jerry Fan

*/

#include "Analog2Digital.h"

#include <avr/io.h>
#include <util/delay.h>


namespace
{
    int8_t  sCurrentChannel;
};



void initA2D( uint8_t ref, uint8_t prescale )
{
    // Set default prescaler, and zero the rest of ADCSRA
    ADCSRA = prescale;

    // Zero ADCSRB
    ADCSRB = 0;

    // Set default reference and zero the rest:
    //  - Default reference is AVCC (safe default)
    //  - ADLAR cleared (right adjusted output)
    //  - MUX cleared (default channel ADC0)
    ADMUX = ( ref << 6 );

    // Enable (turn on) ADC
    ADCSRA |= (1 << ADEN);

    sCurrentChannel = 0;
}


void turnOffA2D()
{
    // Clear (turn off) ADC
    ADCSRA &= ~(1 << ADEN);
}



void setA2DVoltageReference( A2DVoltageReference ref )
{
    ADMUX = ( ADMUX & ~0xc0 ) | ( ref << 6 );

    // Need to let ADC system restablize
    _delay_us( 125 );
}


int readA2D( int8_t channel )
{
    // Differentiate between ATmega328p and ATmega2560
#if defined(__AVR_ATmega2560__)

    if ( channel < 0 || channel > 15 )
    {
        // Not a valid ADC channel
        return 0;
    }

    if ( sCurrentChannel != channel )
    {
        // Set MUX5 if channel > 7 (i.e, bit 3 set); otherwise clear it
        ADCSRB = ( ADCSRB & ~(1<< MUX5) ) | ( ( channel & (1 << 3) ) ? ( 1 << MUX5 ) : 0  );
    }

#else  // ATmega328p

    if ( channel < 0 || channel > 7 )
    {
        // Not a valid ADC channel
        return 0;
    }

#endif

    if ( sCurrentChannel != channel )
    {
        // Set MUX2-0
        ADMUX = ( ADMUX & ~0x1f ) | ( channel & 0x07 );

        // Need to let ADC system restablize
        _delay_us( 125 );

        sCurrentChannel = channel;
    }

    // Start A2D conversion
    ADCSRA |= ( 1 << ADSC );

    // ADSC is cleared when the conversion finishes
    while ( ADCSRA & ( 1 << ADSC ) )
        ;

    // NOTE: must read ADCL before ADCH
    return  ADCL | ( ADCH << 8 );
}
