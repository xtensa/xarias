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

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "s6b0108.h"
#include "gLCD.h"
#include "fonts.h"


bool is_lcd_on=false;
uint8_t gLCD_x, gLCD_y;

FILE gLCD_str5x7   = FDEV_SETUP_STREAM(gLCD_putchar_5x7,   NULL, _FDEV_SETUP_WRITE);
FILE gLCD_str16x24 = FDEV_SETUP_STREAM(gLCD_putchar_16x24, NULL, _FDEV_SETUP_WRITE);


void gLCD_switchon()
{
	if (is_lcd_on) return;
	// disabling JTAG if enabled (should be written twice) 
	MCUCSR |= _BV(JTD);
	MCUCSR |= _BV(JTD);
	_delay_us(10); //??

	// setting data direction for output ports
	B02_DDR(LCD_POWER)  |= _BV(B02_LCD_POWER);

	// turn off display power
	B02_PORT_SET(LCD_POWER);

	DDR(S6B0108_PCMD)  |=  ( _BV(CS1) | _BV(CS2) | _BV(RST) | _BV(E) | _BV(RW) | _BV(RS));
#if CS_LINES_INVERTED==1
	PORT(S6B0108_PCMD) &= ~( _BV(CS1) | _BV(CS2) | _BV(RW) | _BV(RS) | _BV(RST) | _BV(E));
#else
	PORT(S6B0108_PCMD) &= ~( _BV(RW) | _BV(RS) | _BV(RST) | _BV(E));
	PORT(S6B0108_PCMD) |= ( _BV(CS1) | _BV(CS2) );
#endif

	// swithing display power back after delay
	_delay_us(5);
	B02_PORT_UNSET(LCD_POWER);
	// wait 2us and bring power up ( 1us according to datasheet)
	_delay_us(2);
	PORT(S6B0108_PCMD) |= _BV(RST);

	is_lcd_on=true;
	// FIXME: first screen clearence should be done twice
	//        For unkown reason first cls leave garbage on screen
	gLCD_cls();
	//gLCD_cls();

	stdout = &gLCD_str5x7;
	init_font5x7();
	init_font16x24();
}

void gLCD_switchoff()
{
	if (!is_lcd_on) return;
#if CS_LINES_INVERTED==1
	PORT(S6B0108_PCMD) &= ~( _BV(CS1) | _BV(CS2) | _BV(RW) | _BV(RS) | _BV(RST) | _BV(E));
#else
	PORT(S6B0108_PCMD) &= ~( _BV(RW) | _BV(RS) | _BV(RST) | _BV(E));
	PORT(S6B0108_PCMD) |= ( _BV(CS1) | _BV(CS2) );
#endif
	PORT(S6B0108_PDATA)=0x00;
	B02_PORT_SET(LCD_POWER); 
	is_lcd_on=false;
}


void gLCD_cls()
{
	uint8_t i,j;

	if (!is_lcd_on) return;


#if CS_LINES_INVERTED==1
	#if GLCD_RES_X/64==1
		PORT(S6B0108_PCMD) |= _BV(CS1);
	#endif
	#if GLCD_RES_X/64==2
		PORT(S6B0108_PCMD) |= ( _BV(CS1) | _BV(CS2) );
	#endif
	#if GLCD_RES_X/64==3
		PORT(S6B0108_PCMD) |= ( _BV(CS1) | _BV(CS2) | _BV(CS3) );
	#endif
#else
	#if GLCD_RES_X/64==1
		PORT(S6B0108_PCMD) &= ~_BV(CS1);
	#endif
	#if GLCD_RES_X/64==2
		PORT(S6B0108_PCMD) &= ~( _BV(CS1) | _BV(CS2) );
	#endif
	#if GLCD_RES_X/64==3
		PORT(S6B0108_PCMD) &= ~( _BV(CS1) | _BV(CS2) | _BV(CS3) );
	#endif
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
#if CS_LINES_INVERTED==1
	#if GLCD_RES_X/64==1
		PORT(S6B0108_PCMD) &= ~_BV(CS1);
	#endif
	#if GLCD_RES_X/64==2
		PORT(S6B0108_PCMD) &= ~( _BV(CS1) | _BV(CS2) );
	#endif
	#if GLCD_RES_X/64==3
		PORT(S6B0108_PCMD) &= ~( _BV(CS1) | _BV(CS2) | _BV(CS3) );
	#endif
#else
	#if GLCD_RES_X/64==1
		PORT(S6B0108_PCMD) |= _BV(CS1);
	#endif
	#if GLCD_RES_X/64==2
		PORT(S6B0108_PCMD) |= ( _BV(CS1) | _BV(CS2) );
	#endif
	#if GLCD_RES_X/64==3
		PORT(S6B0108_PCMD) |= ( _BV(CS1) | _BV(CS2) | _BV(CS3) );
	#endif
#endif
}

#define gLCD_draw_img_progmem(x,y,img,width,height) 	gLCD_draw_rect( x,  y,  x+width-1, y+height-1, 0, img, 1)
#define gLCD_draw_img(x,y,img,width,height)		gLCD_draw_rect( x,  y,  x+width-1, y+height-1, 0, img, 0)
#define gLCD_fill_rect(x1,y1,x2,y2,p) 			gLCD_draw_rect( x1, y1, x2,      y2,       p,    NULL, 0)

void gLCD_draw_rect(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2, uint8_t ptrn, const uint8_t *img, uint8_t is_progmem)
{
	uint8_t  cs, y, pixs[64], pat, pattern=ptrn, width=x2-x1+1;
	uint16_t x, xt; 

	if (!is_lcd_on) return;

	x2 = MIN(x2,GLCD_RES_X-1);
	y2 = MIN(y2,GLCD_RES_Y-1);


	if(x1>x2 || y1>y2 || x1>GLCD_RES_X || y1>GLCD_RES_Y)
		return;


	pat=0xFF;
	cs=x1/64;
	y=y1/8;
	
		
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
#if CS_LINES_INVERTED==1
				if(cs==0) PORT(S6B0108_PCMD) |= _BV(CS1); else PORT(S6B0108_PCMD) &= ~_BV(CS1); 
	#if GLCD_RES_X/64>1
				if(cs==1) PORT(S6B0108_PCMD) |= _BV(CS2); else PORT(S6B0108_PCMD) &= ~_BV(CS2); 
	#endif
	#if GLCD_RES_X/64>2
				if(cs==2) PORT(S6B0108_PCMD) |= _BV(CS3); else PORT(S6B0108_PCMD) &= ~_BV(CS3); 
	#endif
#else
				if(cs==0) PORT(S6B0108_PCMD) &= ~_BV(CS1); else PORT(S6B0108_PCMD) |= _BV(CS1); 
	#if GLCD_RES_X/64>1
				if(cs==1) PORT(S6B0108_PCMD) &= ~_BV(CS2); else PORT(S6B0108_PCMD) |= _BV(CS2); 
	#endif
	#if GLCD_RES_X/64>2
				if(cs==2) PORT(S6B0108_PCMD) &= ~_BV(CS3); else PORT(S6B0108_PCMD) |= _BV(CS3); 
	#endif
#endif
				s6b0108_outcmd(S6B0108_SETX_MASK|y);
				/*
				 * Start of the line
				 */
				if( x == x1)
				{
					pat = 0xFF;
					if( y1/8 == y ) pat &= 0xFF << (y1%8);
					if( y2/8 == y ) pat &= 0xFF >> (7-y2%8);
					/*
					 * reading byte line on current driver to buffer
					 */
				}
				if(pat!=0xFF)
				{
					for(xt=x%64; cs*64+xt<=x2 && xt<64; xt++)
					{
						s6b0108_outcmd(S6B0108_SETY_MASK|xt);
						pixs[xt]=s6b0108_indata();
					}
				}
				s6b0108_outcmd(S6B0108_SETY_MASK|x);
			}
//			s6b0108_outdata(0xAA);
			if(img)
			{
				//if( pat == 0xFF )
				{
				//	pattern = PIC_smile[y-y1/8][x-x1];
				}
				//else
				{
					pattern=0;
					if(is_progmem)
					{
						if( y<=y2/8 ) pattern |= pgm_read_byte(&(img[(y-y1/8)*width+(x-x1)])) << (y1%8);
						if( y>y1/8 ) pattern |= pgm_read_byte(&(img[(y-y1/8-1)*width+(x-x1)])) >> (8-y1%8);
					}
					else
					{
						if( y<=y2/8 ) pattern |= img[(y-y1/8)*width+(x-x1)] << (y1%8);
						if( y>y1/8 ) pattern |= img[(y-y1/8-1)*width+(x-x1)] >> (8-y1%8);
					}
	
				}
				//img=PIC_smile;
				//pattern = img[y-y1/8][x-x1];
				//pattern = PIC_smile[y-y1/8][x-x1];
				//pat = 0xff;
			}
			if( pat == 0xFF ) s6b0108_outdata(pattern);
			else s6b0108_outdata( (pixs[x%64]&~pat) | (pattern&pat) ); 
			//s6b0108_outdata( pat ); 
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
	if (!is_lcd_on) return;
	if(x>=GLCD_RES_X || y>=GLCD_RES_Y) return;
#if CS_LINES_INVERTED==1
	if(x/64==0) PORT(S6B0108_PCMD) |= _BV(CS1); else PORT(S6B0108_PCMD) &= ~_BV(CS1); 
	#if GLCD_RES_X/64>1
		if(x/64==1) PORT(S6B0108_PCMD) |= _BV(CS2); else PORT(S6B0108_PCMD) &= ~_BV(CS2); 
	#endif
	#if GLCD_RES_X/64>2
		if(x/64==2) PORT(S6B0108_PCMD) |= _BV(CS3); else PORT(S6B0108_PCMD) &= ~_BV(CS3); 
	#endif
#else
	if(x/64==0) PORT(S6B0108_PCMD) &= ~_BV(CS1); else PORT(S6B0108_PCMD) |= _BV(CS1); 
	#if GLCD_RES_X/64>1
		if(x/64==1) PORT(S6B0108_PCMD) &= ~_BV(CS2); else PORT(S6B0108_PCMD) |= _BV(CS2); 
	#endif
	#if GLCD_RES_X/64>2
		if(x/64==2) PORT(S6B0108_PCMD) &= ~_BV(CS3); else PORT(S6B0108_PCMD) |= _BV(CS3); 
	#endif
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
void gLCD_line(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2, bool onoff)
{
	int8_t j;
	int16_t x,y;
	
	if (!is_lcd_on) return;
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
			gLCD_pixel(x,y,onoff);
		}
	}
	else
	{
		j=y2>y1?1:-1; 
		for(y=y1;y!=y2+j;y+=j)
		{
			x=x1+((int16_t)y-(int16_t)y1)/(((int16_t)y2-(int16_t)y1+(y2>y1?1:-1))/((int8_t)x2-(int8_t)x1+(x2>x1?1:-1)));
			if((x2>x1 && x>x2) || (x2<x1 && x<x2)) x=x2;
			gLCD_pixel(x,y,onoff);
		}
	}

}

/*
 * The following function draws rectangle frame. As point (x1,y1) 
 * and (x2,y2) you shoud give opposie vertices of the rectangle.
 */
void gLCD_frame(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2, uint8_t width, bool onoff)
{
	uint8_t i;
	if (!is_lcd_on) return;
	/*
	 * First we shoud swap the coordinats so that we have
	 * top left corner and right bottom corner vertices.
	 */
	if(x1>x2) SWAP(x1,x2);
	if(y1>y2) SWAP(y1,y2);

	for(i=0;i<width;i++)
	{
		gLCD_line(x1+i+1,y1+i,x2-i,y1+i,onoff);
		gLCD_line(x2-i,y1+i+1,x2-i,y2-i,onoff);
		gLCD_line(x2-i-1,y2-i,x1+i,y2-i,onoff);
		gLCD_line(x1+i,y2-i-1,x1+i,y1+i,onoff);
	}	
}

#if 0
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

/*
//	gLCD_cls();
//	gLCD_rwtest();
//	return;
//	gLCD_draw_img_progmem(30,8,PIC_sun,PIC_sun_WIDTH,PIC_sun_HEIGHT);
	//gLCD_draw_img_progmem(20,10,PIC_car,PIC_car_WIDTH,PIC_car_HEIGHT);
	gLCD_draw_img_progmem(6,10,font5x7['A'],5,7);
	//gLCD_echo(13,0,"123 @#$(%_'\"/\\");
	gLCD_draw_img_progmem(0,0,PIC_A,5,8);
	gLCD_draw_img_progmem(0,8,PIC_A,5,8);
	gLCD_draw_img_progmem(0,16,PIC_A,5,8);
	gLCD_draw_img_progmem(0,24,PIC_A,5,8);
	gLCD_draw_img_progmem(0,32,PIC_A,5,8);
	gLCD_draw_img_progmem(0,40,PIC_A,5,8);
	gLCD_draw_img_progmem(0,48,PIC_A,5,8);
	gLCD_draw_img_progmem(0,56,PIC_A,5,8);
	gLCD_draw_img_progmem(10,8,PIC_car,PIC_car_WIDTH,PIC_car_HEIGHT);
//	gLCD_echo(13,3,"Hello world");
//	return 0;

	
	while(1)
	{
		int i,j;
		for(i=127;i<250;i-=3)
		{
			//gLCD_cls();
			//gLCD_fill_rect(2,2,i,i,0xFF);
			gLCD_draw_img_progmem(i,0,PIC_car,PIC_car_WIDTH,PIC_car_HEIGHT);
			//for(j=0;j<1;j++)_delay_ms(20);
		}
		for(j=0;j<5;j++)_delay_ms(200);
		gLCD_cls();
	}
	gLCD_frame(26,41, 99,57, 3);
*/

//	gLCD_alert("asdasda");
//	gLCD_locate(1,1);
//	fprintf(stderr,"TEST");

	gLCD_echo(0,3,"a");

	gLCD_frame(5,5,90,45,3);
	gLCD_frame(15,15,113,60,2);
	gLCD_echo(30,33,"HELLO WORLD");

}
#endif

#define gLCD_locate(x,y) {gLCD_x=x;gLCD_y=y;}

int gLCD_putchar(char c, const uint8_t **font, uint8_t width, uint8_t height, uint8_t space)
{
	uint8_t zero=0;
	if (!is_lcd_on) return 0;
	gLCD_draw_img_progmem(gLCD_x,gLCD_y,font[(uint8_t)c],width,height);
	if(space) gLCD_draw_img(gLCD_x+width,gLCD_y,&zero,space,height);

	gLCD_x+=width+space;
	return 0;	
}

int gLCD_putchar_5x7(char c, FILE *unused)
{
	return gLCD_putchar(c,font5x7,5,7,1);	
}

int gLCD_putchar_16x24(char c, FILE *unused)
{
	return gLCD_putchar(c,font16x24,16,24,0);	
}



