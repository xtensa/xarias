/*
 * XARIAS carputer project
 *
 * Copyright (c) 2007 by Roman Pszonczenko xtensa <_at_> gmail
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

#include "xarias_b02.h"
#include "s6b0108.h"


#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
//#include <avr/pgmspace.h>

#include <util/delay.h>
//#include <util/twi.h>
#include <avr/interrupt.h>


void gLCD_cls();

void gLCD_init()
{
	// disabling JTAG if enabled 
	MCUCSR |= _BV(JTD);

	// setting data direction for output ports
	B02_DDR(LCD_POWER)  |= _BV(B02_LCD_POWER);
	S6B0108_DDR(CS1)    |= _BV(S6B0108_CS1);
	S6B0108_DDR(CS2)    |= _BV(S6B0108_CS2);
	S6B0108_DDR(RST)    |= _BV(S6B0108_RST);
	S6B0108_DDR(RW)     |= _BV(S6B0108_RW);
	S6B0108_DDR(RS)     |= _BV(S6B0108_RS);
	S6B0108_DDR(E)      |= _BV(S6B0108_E);

	S6B0108_DOWN(RW);
	S6B0108_DOWN(RS);
	S6B0108_DOWN(RST);
	S6B0108_DOWN(E);
	
	// turn off display power
	B02_DOWN(LCD_POWER);
	// now switching RST down
	S6B0108_DOWN(RST);

	// swithing display power back
	_delay_us(5);
	B02_UP(LCD_POWER);
	// wait 2us and bring power up ( 1us according to datasheet)
	_delay_us(2);
	S6B0108_UP(RST);

	gLCD_cls();
}


void gLCD_cls()
{
	uint8_t i,j, drives;
	drives=GLCD_RES_X/64;
	S6B0108_UP(CS1);
#if GLCD_RES_X/64>1
	S6B0108_UP(CS2);
#endif
#if GLCD_RES_X/64>2
	S6B0108_UP(CS3);
#endif
	s6b0108_outcmd(S6B0108_DISPLAY_OFF);
	s6b0108_outcmd(S6B0108_START_MASK);
	s6b0108_outcmd(S6B0108_SETY_MASK);
	for(i=0;i<8;i++)
	{
		s6b0108_outcmd(S6B0108_SETX_MASK|i);
		for(j=0;j<64;j++)
			s6b0108_outdata(0x00);
	}
	s6b0108_outcmd(S6B0108_DISPLAY_ON);
	S6B0108_DOWN(CS1);
#if GLCD_RES_X/64>1
	S6B0108_DOWN(CS2);
#endif
#if GLCD_RES_X/64>2
	S6B0108_DOWN(CS3);
#endif
}

void gLCD_clear_region(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2)
{
	uint8_t  cs, y, pixs[64], pattern;
	uint16_t x; 

	pattern=0xFF;
	cs=x1/64;
	y=y1/8;
	x=x1%64; // x is the position on active driver
	
	if(x1>x2 || y1>y2 || x1>GLCD_RES_X || y1>GLCD_RES_Y)
		return;
	
	while(y<=y2/8)
	{
		s6b0108_outcmd(S6B0108_SETX_MASK|(y/8));
	 	/*
		 * Clearing line by line, 8 bits at once
		 */
		while((cs*64+x)<=x2)
		{
			cs=x/64;
			/*
			 * if driver canged ...
			 */
			if(!x || cs*64+x==x1) 
			{
				if(cs==1) S6B0108_UP(CS1); else S6B0108_DOWN(CS1);
#if GLCD_RES_X/64>1
				if(cs==2) S6B0108_UP(CS2); else S6B0108_DOWN(CS2);
#endif
#if GLCD_RES_X/64>2
				if(cs==3) S6B0108_UP(CS3); else S6B0108_DOWN(CS2);
#endif
				/*
				 * reading byte line on current driver to buffer
				 */
				s6b0108_outcmd(S6B0108_SETY_MASK|x);
				while(cs*64+x<x2 && x<64) pixs[x]=s6b0108_indata();
				s6b0108_outcmd(S6B0108_SETY_MASK|x);
				/*
				 * Start of the line
				 */
				if( cs*64+x == x1 )
				{
						pattern=0xFF;
						if( y1/8 == y ) pattern <<= ((y+1)*8-y1);
						else if( y2/8 == y ) pattern >>= ((y+1)*8-y2);
				}
			}
			if( pattern == 0xFF ) s6b0108_outdata(0x00);
			else s6b0108_outdata(pixs[x]&pattern); 
			x++;
			x%=64;	
		}
	}
}


int main()
{
	gLCD_init();

	//S6B0108_DOWN(CS1);
	S6B0108_UP(CS2);

	s6b0108_outcmd(S6B0108_START_MASK|2);
	s6b0108_outcmd(S6B0108_SETX_MASK|3);
	s6b0108_outcmd(S6B0108_SETY_MASK);
//	return 0;


	while(1)
	{
		s6b0108_outdata(0xAA);
		_delay_ms(100);
		s6b0108_outdata(0x55);
		_delay_ms(100);
	}

	return 0;
}
