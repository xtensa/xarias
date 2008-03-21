/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * S6B0108 graphic LCD driver, based on HD44780 LCD display driver by Joerg Wunch
 *
 * The LCD controller is used in 4-bit mode with a full bi-directional
 * interface (i.e. R/~W is connected) so the busy flag can be read.
 *
 * $Id: hd44780.c,v 1.1.2.3 2006/10/08 21:51:14 joerg_wunsch Exp $
 */

#include "xarias_b02.h"
#include "s6b0108.h"

#include <stdbool.h>
//#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>

/*
void _delay_us(double a)
{
	int i;
	for (i=0;i<a*1;i++)
	asm("nop");
}
*/

// the following macro make a bit shift to a from position c to b
#define _BITSHIFT(a,b,c) (((b)>(c))?(a)<<((b)-(c)):(a)>>((c)-(b)))

/*
 * Wait until the busy flag is cleared.
 */
static inline void s6b0108_wait_ready() __attribute__((always_inline));
static inline void s6b0108_wait_ready()
{
	while (s6b0108_incmd() & S6B0108_BUSY_FLAG); 
}


/*
 * Send byte one  out to the LCD controller.
 */
void s6b0108_outbyte(uint8_t n, uint8_t rs)
{
	s6b0108_wait_ready();
	DDR(S6B0108_PDATA)=OUTPUT;
	if (rs)
	{
		PORT(S6B0108_PCMD) &= ~_BV(RW);
		PORT(S6B0108_PCMD) |= _BV(RS);
	}
	else
	{
		PORT(S6B0108_PCMD) &= ~(_BV(RW)|_BV(RS));
	}
	_delay_us(0.14);
	PORT(S6B0108_PCMD) |= _BV(E);
	PORT(S6B0108_PDATA)=n;
	_delay_us(0.5); 
	// 0.45 should be enough but doesn't work
	PORT(S6B0108_PCMD) &= ~_BV(E);

	PORT(S6B0108_PCMD) &= ~( _BV(RS)|_BV(RW) );
}


/*
 * Read one byte from the LCD controller.
 *
 * According to ks6b0108 documentation delays should much more
 * smaller but in practice we should wait much much more !!! 
 * At least I have got such an LCD :(
 */

uint8_t s6b0108_inbyte(uint8_t rs)
{
	uint8_t x;

	if(rs) s6b0108_wait_ready();
	DDR(S6B0108_PDATA)=INPUT;
	PORT(S6B0108_PDATA)=0x00;
	_delay_us(0.3);
	if (rs)
	{	// reading data
		PORT(S6B0108_PCMD) |= (_BV(RW) | _BV(RS));
		_delay_us(0.2);
		//first access is to copy display data to display output register
		PORT(S6B0108_PCMD) |= _BV(E);
		_delay_us(0.5);
		PORT(S6B0108_PCMD) &= ~_BV(E);
		_delay_us(2.5); // 0.5 should be enough but doesn't work
	}
	else
	{	// reading status
		PORT(S6B0108_PCMD) |= _BV(RW);
		PORT(S6B0108_PCMD) &= ~_BV(RS);
		_delay_us(0.2);
	}
	PORT(S6B0108_PCMD) |= _BV(E);
	_delay_us(1.2); // 0.32 should be enough but doesn't work
 	x = PIN(S6B0108_PDATA);
	_delay_us(0.2);
	PORT(S6B0108_PCMD) &= ~_BV(E);

	PORT(S6B0108_PCMD) &= ~( _BV(RS) | _BV(RW) | _BV(E) );
	return x;
}


