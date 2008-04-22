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


#ifndef GLCD_h
#define GLCD_h
//#include "xarias_b02.h"
//#include "s6b0108.h"


//#include <ctype.h>
//#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

//#include <avr/io.h>
//#include <avr/pgmspace.h>

//#include <avr/interrupt.h>


/*
 * Using "swap without tmp" algorythm
 */
#define SWAP(a,b) { a^=b; b^=a; a^=b; }

#define MIN(a,b) 	((a)<(b)?(a):(b))
#define MAX(a,b) 	((a)>(b)?(a):(b))
#define ABS(a)		((a)<0?(-(a)):(a))


extern bool is_lcd_on;
extern uint8_t gLCD_x, gLCD_y;
#define gLCD_locate(x,y) {gLCD_x=x;gLCD_y=y;}

extern FILE gLCD_str5x7; 
extern FILE gLCD_str16x24; 


void gLCD_switchon();
void gLCD_switchoff();
void gLCD_cls();

void gLCD_draw_rect(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2, uint8_t ptrn, const uint8_t *img, uint8_t is_progmem);
#define gLCD_draw_img_progmem(x,y,img,width,height) 	gLCD_draw_rect( x,  y,  x+width-1, y+height-1, 0, img, 1)
#define gLCD_draw_img(x,y,img,width,height)		gLCD_draw_rect( x,  y,  x+width-1, y+height-1, 0, img, 0)
#define gLCD_fill_rect(x1,y1,x2,y2,p) 			gLCD_draw_rect( x1, y1, x2,      y2,       p,    NULL, 0)


 /*
  * Function turns ON or OFF given pixel
 */
void gLCD_pixel(uint16_t x, uint8_t y, bool onoff);
#define gLCD_pixelon(x,y) 	gLCD_pixel(x,y,true)
#define gLCD_pixeloff(x,y) 	gLCD_pixel(x,y,false)


/*
 * gLCD_line draw the line between points (x1,y1) and (x2,y2)
 */
void gLCD_line(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2, bool onoff);


/*
 * The following function draws rectangle frame. As point (x1,y1) 
 * and (x2,y2) you shoud give opposie vertices of the rectangle.
 */
void gLCD_frame(uint16_t x1, uint8_t y1, uint16_t x2, uint8_t y2, uint8_t width, bool onoff);


int  gLCD_putchar_5x7(char c, FILE *unused);
int  gLCD_putchar_16x24(char c, FILE *unused);

#endif
