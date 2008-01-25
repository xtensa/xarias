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



// the following macro make a bit shift to a from position c to b
#define _BITSHIFT(a,b,c) (((b)>(c))?(a)<<((b)-(c)):(a)>>((c)-(b)))

static inline void s6b0108_wait_ready() __attribute__((always_inline));


static inline uint8_t s6b0108_read_databits() __attribute__((always_inline));

static inline uint8_t s6b0108_read_databits()
{
  uint8_t retval=0;
  retval |= _BITSHIFT(S6B0108_PIN(DB7)&_BV(S6B0108_DB7),7,S6B0108_DB7);
  retval |= _BITSHIFT(S6B0108_PIN(DB6)&_BV(S6B0108_DB6),6,S6B0108_DB6);
  retval |= _BITSHIFT(S6B0108_PIN(DB5)&_BV(S6B0108_DB5),5,S6B0108_DB5);
  retval |= _BITSHIFT(S6B0108_PIN(DB4)&_BV(S6B0108_DB4),4,S6B0108_DB4);
  retval |= _BITSHIFT(S6B0108_PIN(DB3)&_BV(S6B0108_DB3),3,S6B0108_DB3);
  retval |= _BITSHIFT(S6B0108_PIN(DB2)&_BV(S6B0108_DB2),2,S6B0108_DB2);
  retval |= _BITSHIFT(S6B0108_PIN(DB1)&_BV(S6B0108_DB1),1,S6B0108_DB1);
  retval |= _BITSHIFT(S6B0108_PIN(DB0)&_BV(S6B0108_DB0),0,S6B0108_DB0);
  return retval;
}

static inline void s6b0108_write_databits(uint8_t x) __attribute__((always_inline));
static inline void s6b0108_write_databits(uint8_t x)
{
  S6B0108_PORT(DB7) &= ~_BV(S6B0108_DB7);
  S6B0108_PORT(DB6) &= ~_BV(S6B0108_DB6);
  S6B0108_PORT(DB5) &= ~_BV(S6B0108_DB5);
  S6B0108_PORT(DB4) &= ~_BV(S6B0108_DB4);
  S6B0108_PORT(DB3) &= ~_BV(S6B0108_DB3);
  S6B0108_PORT(DB2) &= ~_BV(S6B0108_DB2);
  S6B0108_PORT(DB1) &= ~_BV(S6B0108_DB1);
  S6B0108_PORT(DB0) &= ~_BV(S6B0108_DB0);
  
  S6B0108_PORT(DB7) |= _BITSHIFT(x & _BV(7),S6B0108_DB7,7);
  S6B0108_PORT(DB6) |= _BITSHIFT(x & _BV(6),S6B0108_DB6,6);
  S6B0108_PORT(DB5) |= _BITSHIFT(x & _BV(5),S6B0108_DB5,5);
  S6B0108_PORT(DB4) |= _BITSHIFT(x & _BV(4),S6B0108_DB4,4);
  S6B0108_PORT(DB3) |= _BITSHIFT(x & _BV(3),S6B0108_DB3,3);
  S6B0108_PORT(DB2) |= _BITSHIFT(x & _BV(2),S6B0108_DB2,2);
  S6B0108_PORT(DB1) |= _BITSHIFT(x & _BV(1),S6B0108_DB1,1);
  S6B0108_PORT(DB0) |= _BITSHIFT(x & _BV(0),S6B0108_DB0,0);
} 


static inline void s6b0108_ddr_databits(bool x) __attribute__((always_inline));
static inline void s6b0108_ddr_databits(bool x)
{ 
  if ( x )
  {
    S6B0108_DDR(DB7) |= _BV(S6B0108_DB7);
    S6B0108_DDR(DB6) |= _BV(S6B0108_DB6);
    S6B0108_DDR(DB5) |= _BV(S6B0108_DB5);
    S6B0108_DDR(DB4) |= _BV(S6B0108_DB4);
    S6B0108_DDR(DB3) |= _BV(S6B0108_DB3);
    S6B0108_DDR(DB2) |= _BV(S6B0108_DB2);
    S6B0108_DDR(DB1) |= _BV(S6B0108_DB1);
    S6B0108_DDR(DB0) |= _BV(S6B0108_DB0);
  }
  else
  {
    S6B0108_DDR(DB7) &= ~_BV(S6B0108_DB7);
    S6B0108_DDR(DB6) &= ~_BV(S6B0108_DB6);
    S6B0108_DDR(DB5) &= ~_BV(S6B0108_DB5);
    S6B0108_DDR(DB4) &= ~_BV(S6B0108_DB4);
    S6B0108_DDR(DB3) &= ~_BV(S6B0108_DB3);
    S6B0108_DDR(DB2) &= ~_BV(S6B0108_DB2);
    S6B0108_DDR(DB1) &= ~_BV(S6B0108_DB1);
    S6B0108_DDR(DB0) &= ~_BV(S6B0108_DB0);
  }
} 




/*
 * Send one pulse to the E signal (enable).  Mind the timing
 * constraints.  If readback is set to true, read the S6B0108 data
 * pins right before the falling edge of E, and return that value.
 */
static inline void s6b0108_pulse_e(bool readback) __attribute__((always_inline));

static inline void s6b0108_pulse_e(bool readback)
{
  S6B0108_PORT(E) |= _BV(S6B0108_E);
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
  /*
   * When reading back, we need one additional NOP, as the value read
   * back from the input pin is sampled close to the beginning of a
   * CPU clock cycle, while the previous edge on the output pin is
   * generated towards the end of a CPU clock cycle.
   */
  if (readback)
    __asm__ volatile("nop");
#  if F_CPU > 1000000UL
  __asm__ volatile("nop");
#    if F_CPU > 2000000UL
  __asm__ volatile("nop");
  __asm__ volatile("nop");
#    endif /* F_CPU > 2000000UL */
#  endif /* F_CPU > 1000000UL */
#endif
  S6B0108_PORT(E) &= ~_BV(S6B0108_E);
}

/*
 * Send byte one  out to the LCD controller.
 */
void s6b0108_outbyte(uint8_t n, uint8_t rs)
{
	s6b0108_wait_ready();
	s6b0108_ddr_databits(OUTPUT);
	S6B0108_PORT(RW)  &= ~_BV(S6B0108_RW);
	if (rs)
		S6B0108_PORT(RS) |= _BV(S6B0108_RS);
	else
		S6B0108_PORT(RS) &= ~_BV(S6B0108_RS);
	s6b0108_write_databits(n);
	_delay_us(0.14);
	s6b0108_pulse_e(false);
	_delay_us(0.01);
	S6B0108_PORT(RW) &= ~_BV(S6B0108_RW);
	S6B0108_PORT(RS) &= ~_BV(S6B0108_RS);
}


/*
 * Read one byte from the LCD controller.
 */
uint8_t s6b0108_inbyte(uint8_t rs)
{
	uint8_t x;

	if(rs) s6b0108_wait_ready();
	s6b0108_write_databits(0x00); // disabling pull-ups
	s6b0108_ddr_databits(INPUT);
	S6B0108_PORT(RW) |= _BV(S6B0108_RW);
	if (rs)
		S6B0108_PORT(RS) |= _BV(S6B0108_RS);
	else
		S6B0108_PORT(RS) &= ~_BV(S6B0108_RS);
	s6b0108_pulse_e(false); //first access is to copy display data to display output register
	_delay_us(0.14);
	S6B0108_PORT(E) |= _BV(S6B0108_E); 
	_delay_us(0.32);
	
 	x = s6b0108_read_databits();
	S6B0108_PORT(E)  &= ~_BV(S6B0108_E);
	S6B0108_PORT(RW) &= ~_BV(S6B0108_RW);
	S6B0108_PORT(RS) &= ~_BV(S6B0108_RS);
	return x;
}

/*
 * Wait until the busy flag is cleared.
 */
static inline void s6b0108_wait_ready()
{
/*
 * old busy flag read code
 *
	uint8_t x;

	s6b0108_ddr_databits(INPUT);
	s6b0108_write_databits(0x00); // disabling pull-ups
	S6B0108_PORT(RW) |= _BV(S6B0108_RW);
	S6B0108_PORT(RS) &= ~_BV(S6B0108_RS);
	while(x & S6B0108_BUSY_FLAG )
	{
		_delay_us(0.14);
		S6B0108_PORT(E) |= _BV(S6B0108_E);
		_delay_us(0.25);
		x=s6b0108_read_databits();
		S6B0108_PORT(E)  &= ~_BV(S6B0108_E);
	}
	S6B0108_PORT(RW) &= ~_BV(S6B0108_RW);
	S6B0108_PORT(RS) &= ~_BV(S6B0108_RS);

*/
	while (s6b0108_incmd() & S6B0108_BUSY_FLAG); 
}


