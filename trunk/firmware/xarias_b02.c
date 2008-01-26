/*
 * XARIAS carputer project
 *
 * Copyright (c) 2007 by Roman Pszonczenko xtensa <_at_> go0ogle mail
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

#include <stdbool.h>
#include <util/delay.h>
//#include <util/twi.h>
#include <avr/interrupt.h>


/*
 * Using "swap without tmp" algorythm
 */
#define SWAP(a,b) { a^=b; b^=a; a^=b; }

#define MIN(a,b) 	((a)<(b)?(a):(b))
#define MAX(a,b) 	((a)>(b)?(a):(b))
#define ABS(a)		((a)<0?(-(a)):(a))

void gLCD_cls();

void gLCD_init()
{
	// disabling JTAG if enabled 
	MCUCSR |= _BV(JTD);

	// setting data direction for output ports
	B02_DDR(LCD_POWER)  |= _BV(B02_LCD_POWER);

	DDR(S6B0108_PCMD)  |=  ( _BV(CS1) | _BV(CS2) | _BV(RST) | _BV(E) | _BV(RW) | _BV(RS));

	PORT(S6B0108_PCMD) &= ~( _BV(CS1) | _BV(CS2) |_BV(RW) | _BV(RS) | _BV(RST) | _BV(E));
	
	// turn off display power
	B02_DOWN(LCD_POWER);
	// swithing display power back after delay
	_delay_us(5);
	B02_UP(LCD_POWER);
	// wait 2us and bring power up ( 1us according to datasheet)
	_delay_us(2);
	PORT(S6B0108_PCMD) |= _BV(RST);

	gLCD_cls();
}


void gLCD_cls()
{
	uint8_t i,j;
#if GLCD_RES_X/64==1
	PORT(S6B0108_PCMD) |= _BV(CS1);
#endif
#if GLCD_RES_X/64==2
	PORT(S6B0108_PCMD) |= ( _BV(CS1) | _BV(CS2) );
#endif
#if GLCD_RES_X/64==3
	PORT(S6B0108_PCMD) |= ( _BV(CS1) | _BV(CS2) | _BV(CS3) );
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
#if GLCD_RES_X/64==1
	PORT(S6B0108_PCMD) &= ~_BV(CS1);
#endif
#if GLCD_RES_X/64==2
	PORT(S6B0108_PCMD) &= ~( _BV(CS1) | _BV(CS2) );
#endif
#if GLCD_RES_X/64==3
	PORT(S6B0108_PCMD) &= ~( _BV(CS1) | _BV(CS2) | _BV(CS3) );
#endif
}

void gLCD_fill_rect(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2,uint8_t pattern)
{
	uint8_t  cs, y, pixs[64], pat;
	uint16_t x, xt; 

	pat=0xFF;
	cs=x1/64;
	y=y1/8;
	
	if(x1>x2 || y1>y2 || x1>GLCD_RES_X || y1>GLCD_RES_Y)
		return;
	
	while(y<=y2/8)
	{
		x=x1;
		pat=0xFF;

	 	/*
		 * Filling line by line, 8 bits at once
		 */
		while(x<=x2)
		{
			/*
			 * if driver canged ...
			 */
			if(!(x%64) || x==x1) 
			{
				cs=x/64;
				if(cs==0) PORT(S6B0108_PCMD) |= _BV(CS1); else PORT(S6B0108_PCMD) &= ~_BV(CS1); 
#if GLCD_RES_X/64>1
				if(cs==1) PORT(S6B0108_PCMD) |= _BV(CS2); else PORT(S6B0108_PCMD) &= ~_BV(CS2); 
#endif
#if GLCD_RES_X/64>2
				if(cs==2) PORT(S6B0108_PCMD) |= _BV(CS3); else PORT(S6B0108_PCMD) &= ~_BV(CS3); 
#endif
				s6b0108_outcmd(S6B0108_SETX_MASK|y);
				/*
				 * Start of the line
				 */
				if( x == x1)
				{
					if (y1/8 == y2/8) pat = ((0xFF >> (y1-y*8)) & (0xFF << (y2-y*8)));
					else if( y1/8 == y ) pat = 0xFF >> (y1-y*8);
					else if( y2/8 == y ) pat = 0xFF << (y2-y*8);
					/*
					 * reading byte line on current driver to buffer
					 */
				}
				if(pat!=0xFF)
				{
					for(xt=x%64; cs*64+xt<x2 && xt<64; xt++)
					{
						s6b0108_outcmd(S6B0108_SETY_MASK|xt);
						pixs[xt]=s6b0108_indata();
					}
				}
				s6b0108_outcmd(S6B0108_SETY_MASK|x);
			}
//			s6b0108_outdata(0xAA);
			if( pat == 0xFF ) s6b0108_outdata(pattern);
			else s6b0108_outdata( (pixs[x%64]&pat) | (pattern&~pat) ); 
			x++;
		}
		y++;
	}
}

/*
 * Function turns ON or OFF given pixel
 */
void gLCD_pixel(uint16_t x, uint8_t y, bool onoff)
{
	uint8_t tmp;
	if(x>=GLCD_RES_X || y>=GLCD_RES_Y) return;
	if(x/64==0) PORT(S6B0108_PCMD) |= _BV(CS1); else PORT(S6B0108_PCMD) &= ~_BV(CS1); 
#if GLCD_RES_X/64>1
	if(x/64==1) PORT(S6B0108_PCMD) |= _BV(CS2); else PORT(S6B0108_PCMD) &= ~_BV(CS2); 
#endif
#if GLCD_RES_X/64>2
	if(x/64==2) PORT(S6B0108_PCMD) |= _BV(CS3); else PORT(S6B0108_PCMD) &= ~_BV(CS3); 
#endif

	s6b0108_outcmd(S6B0108_SETX_MASK|(y/8));
	s6b0108_outcmd(S6B0108_SETY_MASK|(x%64));
	tmp=s6b0108_indata(); 
	s6b0108_outcmd(S6B0108_SETY_MASK|(x%64));
	if(onoff) s6b0108_outdata(tmp|_BV(y%8));
	else s6b0108_outdata(tmp&~_BV(y%8));
}

#define gLCD_pixelon(x,y) 	gLCD_pixel(x,y,true)
#define gLCD_pixeloff(x,y) 	gLCD_pixel(x,y,false)

/*
 * gLCD_line draw the line between points (x1,y1) and (x2,y2)
 */
void gLCD_line(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2)
{
	int8_t j;
	int16_t x,y;
	
	if(x1>=GLCD_RES_X || y1>=GLCD_RES_Y) return;
	if(x2>=GLCD_RES_X || y2>=GLCD_RES_Y) return;
	
	/*
	 * 50 added for good roundings
	 */
	if(ABS((int16_t)x2-(int16_t)x1)>ABS((int8_t)y2-(int8_t)y1))
	{
		j=x2>x1?1:-1;
		for(x=x1;x!=x2+j;x+=j)
		{
			y=y1+((int16_t)x-(int16_t)x1)/(((int16_t)x2-(int16_t)x1+(x2>x1?1:-1))/((int8_t)y2-(int8_t)y1+(y2>y1?1:-1)));
			if((y2>y1 && y>y2) || (y2<y1 && y<y2)) y=y2;
			gLCD_pixelon(x,y);
		}
	}
	else
	{
		j=y2>y1?1:-1; 
		for(y=y1;y!=y2+j;y+=j)
		{
			x=x1+((int16_t)y-(int16_t)y1)/(((int16_t)y2-(int16_t)y1+(y2>y1?1:-1))/((int8_t)x2-(int8_t)x1+(x2>x1?1:-1)));
			if((x2>x1 && x>x2) || (x2<x1 && x<x2)) x=x2;
			gLCD_pixelon(x,y);
		}
	}

}

/*
 * The following function draws rectangle frame. As point (x1,y1) 
 * and (x2,y2) you shoud give opposie vertices of the rectangle.
 */
void gLCD_frame(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2, uint8_t width)
{
	uint8_t i;
	/*
	 * First we shoud swap the coordinats so that we have
	 * top left corner and right bottom corner vertices.
	 */
	if(x1>x2) SWAP(x1,x2);
	if(y1>y2) SWAP(y1,y2);

	for(i=0;i<width;i++)
	{
		gLCD_line(x1+i+1,y1+i,x2-i,y1+i);
		gLCD_line(x2-i,y1+i+1,x2-i,y2-i);
		gLCD_line(x2-i-1,y2-i,x1+i,y2-i);
		gLCD_line(x1+i,y2-i-1,x1+i,y1+i);
	}	
}


void gLCD_rwtest()
{
	uint8_t i,j[2],k=0;
	
	PORT(S6B0108_PCMD) |= _BV(CS2);
	s6b0108_outcmd(S6B0108_SETX_MASK);
	s6b0108_outcmd(S6B0108_SETY_MASK|3);
	s6b0108_outdata(0xFF); 
	s6b0108_outdata('a'); 
	s6b0108_outdata('t'); 
	//j=s6b0108_incmd(); 
	s6b0108_outcmd(S6B0108_SETY_MASK|3);
	j[0]=s6b0108_indata(); 
	j[1]=s6b0108_indata(); 
	s6b0108_outdata('e'); 
	PORT(S6B0108_PCMD) &= ~_BV(CS2);
	PORT(S6B0108_PCMD) |= _BV(CS1);
	s6b0108_outcmd(S6B0108_SETX_MASK);
	s6b0108_outcmd(S6B0108_SETY_MASK);
	for(k=0;k<2;k++)
	{
		for(i=0;i<8;i++)
			if(j[k]&_BV(i))
				s6b0108_outdata(0xFF);
			else
				s6b0108_outdata(0x01); 
	}
	PORT(S6B0108_PCMD) &= ~_BV(CS1);
}



int main()
{
	//uint8_t i,j;

	gLCD_init();
//	gLCD_rwtest();
//	return;

	gLCD_fill_rect(33,4,103,34,0xAA);
//	gLCD_frame(26,41, 99,57, 3);
	
	return 0;
}
