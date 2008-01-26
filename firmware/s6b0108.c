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
void _delay_us(int a)
{
	asm("nop");
}
*/

// the following macro make a bit shift to a from position c to b
#define _BITSHIFT(a,b,c) (((b)>(c))?(a)<<((b)-(c)):(a)>>((c)-(b)))

static inline void s6b0108_wait_ready() __attribute__((always_inline));

/*
 * Send one pulse to the E signal (enable).  Mind the timing
 * constraints.  If readback is set to true, read the S6B0108 data
 * pins right before the falling edge of E, and return that value.
 */
static inline void s6b0108_pulse_e() __attribute__((always_inline));

static inline void s6b0108_pulse_e()
{
  PORT(S6B0108_PCMD) |= _BV(E);
  /*
   * Guarantee at least 500 ns of pulse width (according to datasheet
   * 45 i enough. For high CPU frequencies, a delay loop is used.  
   * For lower frequencies, NOPs are used, and at or below 1 MHz, 
   * the native pulse width will already be 1 us or more so no 
   * additional delays are needed.
   */
#if F_CPU > 4000000UL
  _delay_us(0.5);
#else
#  if F_CPU > 1000000UL
  __asm__ volatile("nop");
#    if F_CPU > 2000000UL
  __asm__ volatile("nop");
  __asm__ volatile("nop");
#    endif /* F_CPU > 2000000UL */
#  endif /* F_CPU > 1000000UL */
#endif
  PORT(S6B0108_PCMD) &= ~_BV(E);
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
	PORT(S6B0108_PDATA)=n;
	_delay_us(0.14);
	s6b0108_pulse_e();
	PORT(S6B0108_PCMD) &= ~( _BV(RS)|_BV(RW) );
}


/*
 * Read one byte from the LCD controller.
 */
uint8_t s6b0108_inbyte(uint8_t rs)
{
	uint8_t x;

	if(rs) s6b0108_wait_ready();
	DDR(S6B0108_PDATA)=INPUT;
	PORT(S6B0108_PDATA)=0x00;
	if (rs)
	{	// reading data
		PORT(S6B0108_PCMD) |= (_BV(RW) | _BV(RS));
		_delay_us(0.14);
		s6b0108_pulse_e(); //first access is to copy display data to display output register
		_delay_us(0.32);
	}
	else
	{	// reading status
		PORT(S6B0108_PCMD) |= _BV(RW);
		PORT(S6B0108_PCMD) &= ~_BV(RS);
	}
	PORT(S6B0108_PCMD) |= _BV(E);
	_delay_us(0.32);
 	x = PIN(S6B0108_PDATA);
	_delay_us(0.1);
	PORT(S6B0108_PCMD) &= ~_BV(E);
	_delay_us(0.1);
	PORT(S6B0108_PCMD) &= ~( _BV(RS) | _BV(RW) | _BV(E) );
	return x;
}

/*
 * Wait until the busy flag is cleared.
 */
static inline void s6b0108_wait_ready()
{
	while (s6b0108_incmd() & S6B0108_BUSY_FLAG); 
}


