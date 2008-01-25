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

void gLCD_fill_rect(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2,uint8_t pattern)
{
	uint8_t  cs, y, pixs[64], pat;
	uint16_t x; 

	pat=0xFF;
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
				if(cs==0) S6B0108_UP(CS1); else S6B0108_DOWN(CS1);
#if GLCD_RES_X/64>1
				if(cs==1) S6B0108_UP(CS2); else S6B0108_DOWN(CS2);
#endif
#if GLCD_RES_X/64>2
				if(cs==2) S6B0108_UP(CS3); else S6B0108_DOWN(CS2);
#endif
				/*
				 * Start of the line
				 */
				pat=0xFF;
				if( cs*64+x == x1 )
				{
					if (y1/8 == y2/8) pat = ((0xFF >> (y1-y*8)) & (0xFF << (y2-y*8)));
					else if( y1/8 == y ) pat = 0xFF >> (y1-y*8);
					else if( y2/8 == y ) pat = 0xFF << (y2-y*8);
					/*
					 * reading byte line on current driver to buffer
					 */
					s6b0108_outcmd(S6B0108_SETY_MASK|x);
					while(cs*64+x<x2 && x<64) pixs[x]=s6b0108_indata();
				}
				s6b0108_outcmd(S6B0108_SETY_MASK|x);
			}
			if( pat == 0xFF ) s6b0108_outdata(pattern);
			else s6b0108_outdata( (pixs[x]&~pat) | (pattern&pat) ); 
			x++;
			x%=64;	
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
	if(x/64==0) S6B0108_UP(CS1); else S6B0108_DOWN(CS1);
#if GLCD_RES_X/64>1
	if(x/64==1) S6B0108_UP(CS2); else S6B0108_DOWN(CS2);
#endif
#if GLCD_RES_X/64>2
	if(x/64==2) S6B0108_UP(CS3); else S6B0108_DOWN(CS2);
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
	/*
	 * a and b parameters will be calculated as multiplication of 100
	 */
	int16_t x,a,b;
	
	if(x1>=GLCD_RES_X || y1>=GLCD_RES_Y) return;
	if(x2>=GLCD_RES_X || y2>=GLCD_RES_Y) return;
	
	/*
	 * 50 added for good roundings
	 */
	if(ABS((int16_t)x2-(int16_t)x1)>ABS((int8_t)y2-(int8_t)y1))
	{
		a=(((int16_t)y2-(int16_t)y1)*100)/((int16_t)x2-(int16_t)x1);
		b=(int16_t)y1*100-a*(int16_t)x1;
		for(x=MIN(x1,x2);x<=MAX(x1,x2);x++) gLCD_pixelon(x,(a*x+b)/100);
	}
	else
	{
		a=(((int16_t)x2-(int16_t)x1)*100)/((int16_t)y2-(int16_t)y1);
		b=(int16_t)x1*100-a*(int16_t)y1;
		for(x=MIN(y1,y2);x<=MAX(y1,y2);x++) gLCD_pixelon((a*x+b)/100,x);
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
	uint8_t i,j;
	
	s6b0108_outcmd(S6B0108_SETX_MASK);
	s6b0108_outcmd(S6B0108_SETY_MASK|3);
	s6b0108_outdata(0xFF); 
	s6b0108_outcmd(S6B0108_SETY_MASK|3);
//	j=s6b0108_incmd(); 
	j=s6b0108_indata(); 
	s6b0108_outdata('e'); 
	S6B0108_DOWN(CS2);
	S6B0108_UP(CS1);
	s6b0108_outcmd(S6B0108_SETX_MASK);
	s6b0108_outcmd(S6B0108_SETY_MASK);
	for(i=0;i<8;i++)
		if(j&_BV(i))
			s6b0108_outdata(0xFF);
		else
			s6b0108_outdata(0x01); 
}



int main()
{
	//uint8_t i,j;

	gLCD_init();
	S6B0108_UP(CS2);

	gLCD_line(0,0,127,1);
	gLCD_line(0,30,127,32);
	gLCD_line(0,2,1,62);
	gLCD_line(70,0,71,10);

	gLCD_line(0,40,127,39);
	gLCD_line(127,2,126,62);
	gLCD_line(80,0,79,10);
//	gLCD_frame(46,41, 99,57, 3);
	
	return 0;
}
