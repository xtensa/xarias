/*
 * XARIAS carputer project
 *
 * Copyright (c) 2008 by Roman Pszonczenko xtensa <_at_> go0ogle mail
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "xarias_b02.h"

void xarias_ac_init()
{
        /*
	 * Enable external interrupt 0
	 */
	GICR = _BV(INT0);
	/*
	 * Falling edge will generate interrupt 0
	 */
	MCUCR |= _BV(1);
	DDRD  &= ~_BV(PORT2); // set pin to input 
	PORTD &= ~_BV(PORT2); // disable pull-up

	sei();



	/*
	 * Writing slave address
	 */
	TWAR = TWIADDR_AC & 0xFE;

//	DDRC  &= ~(_BV(PORT4)|_BV(PORT5));
//	PORTC &= ~(_BV(PORT4)|_BV(PORT5));

	/*
	 * Enabling TWI
	 */
	TWCR &= ~(_BV(TWSTA) | _BV(TWSTO));
	//TWCR |= _BV(TWEN) | _BV(TWEA);

}

int main()
{

	uint8_t cmp=255;

	xarias_ac_init();
	// set PWM, phase correct mode
	TCCR2 |= _BV(WGM20);
	TCCR2 &= ~_BV(WGM21);

	// Clear OC2 on Compare Match when up-counting. Set OC2 on Compare
	// Match when downcounting.
	TCCR2 |= _BV(COM21);
//	TCCR2 &= ~(_BV(COM21)|_BV(COM20)); // normal port

	// For 4Mhz quarz CPU_FREQ/(256*2)
	// CS22 CS21 CS20 Description
	// 0 0 0 No clock source (Timer/Counter stopped).			4MHz		8MHz
	// 0 0 1 clkT2S/	(No prescaling) 	CPU_FREQ/(512)		7812 Hz		15625
	// 0 1 0 clkT2S/8 	(From prescaler) 	CPU_FREQ/(256*2*8)	976 Hz		1953
	// 0 1 1 clkT2S/32 	(From prescaler)	CPU_FREQ/(256*2*32)	488 Hz		976
	// 1 0 0 clkT2S/64 	(From prescaler)	CPU_FREQ/(256*2*64)	244 Hz		488
	// 1 0 1 clkT2S/128 	(From prescaler)	CPU_FREQ/(256*2*128)	122 Hz		244
	// 1 1 0 clkT2S/256 	(From prescaler)	CPU_FREQ/(256*2*256)	61 Hz		122
	// 1 1 1 clkT2S/1024 	(From prescaler)	CPU_FREQ/(256*2*1024)	7 Hz		14
	
	TCCR2 |= _BV(CS20) ; // 15625caler for 8Mhz

	// Setting OC2 as output
	DDRB |= _BV(3);
	//PORTB |= _BV(3);


	OCR2=cmp;



	return 0;
}
