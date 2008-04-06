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

#include "xarias_b02.h"
#include "s6b0108.h"


#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <stdbool.h>
#include <util/delay.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#include "gLCD.h"
#include "twi_devs.h"
//#include "../utils/out.h"

#define PIN_INJ   PIND&_BV(3)
#define PORT_INJ  PORTD|_BV(3)


#define _NOP __asm__ volatile ("nop"::);

/*
 * Injector flow constant in ml/min
 * For Toyota Corolla 1,3 4EFE it's 136 - 176
 * For Toyota Corolla 1,6 4AFE it's 160 - 200
 */
#define INJ_FLOW 156

/*
 * Number of ticks per one kilometer
 */
#define SPEED_TICKS 2548


//#define KB_SET(i)   kb_state |=  _BV(i)
//#define KB_UNSET(i) kb_state &= ~_BV(i)

#define MODE_TRIP_SETTINGS	0x00
#define MODE_AIRCON_SETTINGS	0x01
#define MODE_DATETIME_SETTINGS	0x02
#define MODE_SCREEN_ADJUST	0x03
#define MODE_SERVICE		0x04
#define MODE_MAIN		0x05
#define MODE_MENU		0x06



/*
 * Main menu variables.
 */
uint8_t mainmenu_pos=0;
#define MAIN_MENU_COUNT 6
char mainmenu_strings[6][19]={	"Trip settings",
				"AirCon settings",
				"Date/Time settings",
				"Screen adjustment",
				"Service mode",
				"Exit"};


/*
 * Action flags
 * To save RAM space several flags are stored in separate
 * bits of this action flag byte
 */
uint8_t aflags;
#define AFLAGS_SET(a) 	aflags|=_BV(a)
#define AFLAGS_UNSET(a) aflags&=~_BV(a)
#define AFLAGS_ISSET(a) (aflags&_BV(a))
#define FLAG_LCD_UPDATE   	0
#define FLAG_MODE_CHANGED 	1
#define FLAG_KB_STH_PRESSED 	2

/*
 * This array is used to store keyboard states
 */
uint8_t kb_state[12]={0};
#define KB_UP 		kb_state[1]
#define KB_DOWN 	kb_state[5]
#define KB_LEFT		kb_state[3]
#define KB_RIGHT	kb_state[2]
#define KB_OK 		kb_state[0]
#define KB_FUNC1	kb_state[9]
#define KB_FUNC2	kb_state[8]
#define KB_FUNC3	kb_state[10]
#define KB_FUNC4	kb_state[11]
#define KB_FUNC5	kb_state[7]
#define KB_FUNC6	kb_state[6]
#define KB_FUNC7	kb_state[4]

#define KB_ESC		KB_FUNC7

/*
 * The following variable is used to store current working mode
 */
uint8_t modestate;

/*
 * Use only this function to change mode. Do not change mode directly.
 */
void inline set_mode(uint8_t mode)
{
	modestate=mode;
	aflags|=_BV(FLAG_MODE_CHANGED)|_BV(FLAG_LCD_UPDATE);
	gLCD_cls();
}


/*
 * All variables for calculating speed, rpm, fuel consumption, etc.
 */
uint16_t tcnt0_overs_sec=0, speed_ticks=0;
uint64_t passed_inj_ticks=0;
uint32_t passed_seconds=0, tcnt0_overs=0, passed_speed_ticks=0;
volatile uint16_t last_inj_ticks=0, rpm_ticks=0, clock_ticks=0;
///////////////////////////////








#define error(x) error_(x,1)

void error_(char *msg, uint8_t data)
{
	uint8_t i;
	gLCD_cls();
	gLCD_alert(msg);
	gLCD_locate(15,3);
	fprintf(stdout,"ERROR: %u", data);
	for(i=0;i<200;i++) _delay_ms(500);
}


/*
 * Do all the startup-time peripheral initializations.
 */
static void xarias_init(void)
{
	/*
	 * Setting up TWI bus.
	 */
	twi_init();
	twi_start();

	/*
	 * Sending device address 1101 000 of DS1307 chip
	 */
	twi_write_addr(TWIADDR_DS1307);

	/*
	 * writing address start 0
	 */
	twi_write_data(0);

	/*
	 * Writing time registers
	 * Important to enable CH bit in register 0
	 */
	twi_write_data(51); 
	twi_write_data(1);
	twi_write_data(1);
	twi_write_data(1);
	twi_write_data(1);
	twi_write_data(1);
	twi_write_data(1);

	/*
	 * writing control register
	 * setting 32,768 kHz (bits 0 and 1)
	 */
	twi_write_data(_BV(4)|_BV(0)|_BV(1));

	
	/*
	 * Stopping TWI communication
	 */
	twi_stop();

	_delay_us(10);


	/*
	 * Disabling self-programming feature
	 */
	SPCR &= ~_BV(SPE);

	/*
	 * ---------- Setting keyboard -----------
	 *
	 *  Setting keyboard rows as output and turning on pull-ups
	 */
	B02_DDR_SET(KB_ROW1);
	B02_DDR_SET(KB_ROW2);
	B02_DDR_SET(KB_ROW3);
	B02_PORT_SET(KB_ROW1);
	B02_PORT_SET(KB_ROW2);
	B02_PORT_SET(KB_ROW3);
	/*
	 * Setting keyboard columns as input
	 */
	B02_PORT_SET(KB_COL1);
	B02_PORT_SET(KB_COL2);
	B02_PORT_SET(KB_COL3);
	B02_PORT_SET(KB_COL4);
	B02_DDR_UNSET(KB_COL1);
	B02_DDR_UNSET(KB_COL2);
	B02_DDR_UNSET(KB_COL3);
	B02_DDR_UNSET(KB_COL4);
	/* --------------------------------------------------- */



	/*
	 *
	 * Enable external interrupt 0
	 */
	GICR = _BV(INT0);
	/*
	 * Falling edge will generate interrupt 0
	 */
	MCUCR |= _BV(1);
	/*
	 * enable pull-up resistors for ports D2 and D3
	 */
	PORTD |= _BV(2) | _BV(3);

	/*
	 * Initialize Timer0 for speed counter.
	 * Setting external clock source for Timer0, clock on rising edge.
	 */
	TCCR0 |= _BV(CS02) | _BV(CS01) | _BV(CS00);
	/*
	 * Initialize Timer1 fro RPM counter.
	 * Setting External clock source for Timer1, clock on rising edge.
	 */
	TCCR1B |= _BV(CS12) | _BV(CS11) | _BV(CS10);
	/*
	 * Enable Timer0 and Timer1 overflow interrupts
	 */
	TIMSK |= _BV(TOIE0) | _BV(TOIE1);

	/*
	 * enable all interrupts
	 */
	sei();

	set_mode(MODE_MAIN);
}

/*
 * This function return fuel consumption in l/h * 1000
 * We assume that fuel consumption is not greater than 65.536 l/h
 * Although l_inj_ticks is uint64_t, it cannot be greater than
 * xxxxxx and 
 */
uint16_t calc_fuel_h(uint64_t l_inj_ticks, uint32_t l_seconds)
{
	return (uint16_t)((l_inj_ticks*INJ_FLOW*15)/((uint64_t)2048*l_seconds));
}

uint16_t calc_fuel_100(uint16_t l_fuel_h, uint64_t l_speed_ticks, uint32_t l_seconds)
{
	return (uint16_t)(((uint64_t)l_fuel_h*SPEED_TICKS*l_seconds)/(l_speed_ticks*36));
}

uint16_t calc_speed_m(uint64_t l_speed_ticks, uint32_t l_seconds)
{
	return (uint16_t)(l_speed_ticks*1000/(SPEED_TICKS*l_seconds));
}


uint32_t power(uint32_t x, uint8_t y)
{
	uint8_t i;
	uint32_t retval=1;
	for(i=0;i<y;i++,retval*=x);
	return retval;
}

/*
 * Cuts last p digits of the value
 */
#define CUT(val,mult) ((uint16_t)((val)/power(10,(uint8_t)mult)))

/* 
 * The most calculations in this program made on integer values. 
 * Floating point precision is achieved by doing calculations on 
 * values multiplied with 10^x, where i x is precision. For example 
 * if we want to write 2.345 with precision of 3 digits after 
 * floating point, we will use 2345, as it is equal to 2.345*10^3
 * Recalculation is done just before displayng the values.
 * The next three macros are provided to made such calculations.
 *
 * Parameters:
 *    <val>   - value on which calculations are made
 *    <mult>  - multiplier 
 *    <prec>  - precision of the calculations; shoud be less
 *              or equal then multiplier
 *
 * ROUND macro just do the rounding to <prec> digits after
 * imaginable point. Be aware that it does not truncate the rest 
 * digits
 *
 * uint32_t gives us 4294967 km 296 m of maximium pass
 */
#define ROUND(val,mult,prec) ((uint32_t)(val+5*power(10,(uint8_t)mult-prec-1)))

/*
 * This macro just truncates digits after the floating point, so it
 * only left integer part before.
 */
#define ROUND1(val,mult,prec) ((uint16_t)CUT(ROUND(val,mult,prec),mult))

/*
 * And the following macro truncate digits before floating point,
 * so it only left floating part with the specified precision.
 */
#define ROUND2(val,mult,prec) (uint16_t)(CUT(ROUND(val,mult,prec)-CUT(ROUND(val,mult,prec),mult)*power(10,(uint8_t)mult),mult-prec))


void if_draw_progressbar(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t percent)
{
	uint8_t xx=x1+(uint8_t)((x2-x1)*percent/100);
	if(x1>=x2 || y1>=y2 || y2-y1<2 || x2-x1<2) return; 
	gLCD_frame(x1,y1,x2,y2,1,true);
	gLCD_fill_rect(x1,y1,xx,y2,0xff);
	gLCD_fill_rect(xx+1,y1+1,x2-1,y2-1,0x00);
}

void kb_set(uint8_t i, uint8_t j, uint8_t is_on)
{
	gLCD_locate(i*29,j*10);
	if(is_on)
		fprintf(stdout,"%u/%u",i,j);
	else
		fprintf(stdout,"%u-%u",i,j);
}

uint8_t inline kb_column_getstate(uint8_t col)
{
	if(col==0) return B02_GETSTATE(KB_COL1);
	if(col==1) return B02_GETSTATE(KB_COL2);
	if(col==2) return B02_GETSTATE(KB_COL3);
	if(col==3) return B02_GETSTATE(KB_COL4);
	return 0;
}

int main()
{
	uint8_t i,j,k=104, l=80;
	uint8_t pin;


	/*
	 * Wait for power to stabilize
	 */
	_delay_ms(20);

	gLCD_init();
	xarias_init();
					ds1803_write(0,k);
					ds1803_write(1,l);
	
	while(1)
	{
		uint8_t kb_sth_pressed=0;
		/*
		 * Keyboard routine
		 */
		for(j=0;j<3;j++)
		{
			if(j==0) B02_PORT_UNSET(KB_ROW1); else B02_PORT_SET(KB_ROW1);
			if(j==1) B02_PORT_UNSET(KB_ROW2); else B02_PORT_SET(KB_ROW2);
			if(j==2) B02_PORT_UNSET(KB_ROW3); else B02_PORT_SET(KB_ROW3);


			for(i=0;i<4;i++)
			{
				if(!(pin=kb_column_getstate(i)))
				{
					_delay_us(100);
					if(!(pin=kb_column_getstate(i)))
					{
						kb_state[i+j*4]++;
						kb_sth_pressed=1;
					}
				}
				else
				{
					kb_state[i+j*4]=0;
				}
			//	kb_set(i,j,!pin);
				if(AFLAGS_ISSET(FLAG_MODE_CHANGED)) kb_state[i+j*4]=0;

			}
		} // end keyboard routine

		if(!kb_sth_pressed) AFLAGS_UNSET(FLAG_MODE_CHANGED);
		
		/*
		 * key processing logics
		 */
		switch(modestate)
		{
			case MODE_MAIN:
			{
				if(KB_OK) 
				{ 
					set_mode(MODE_MENU); 
				}
			} break;

			case MODE_MENU:
			{
				if(KB_DOWN==1 || KB_DOWN==50) 
				{ 
					mainmenu_pos++;
					mainmenu_pos%=MAIN_MENU_COUNT;
					AFLAGS_SET(FLAG_LCD_UPDATE); 
					if(KB_DOWN==50) KB_DOWN=49;
				}
				if(KB_UP==1 || KB_UP==50) 
				{ 
					if(mainmenu_pos)
						mainmenu_pos--;
					else
						mainmenu_pos=MAIN_MENU_COUNT-1;
					AFLAGS_SET(FLAG_LCD_UPDATE); 
					if(KB_UP==50) KB_UP=49;
				}
				if(KB_OK)
				{
					set_mode(mainmenu_pos);
					if(mainmenu_pos==MODE_MAIN) mainmenu_pos=0;
				}
				if(KB_ESC) 
				{
					set_mode(MODE_MAIN);
					mainmenu_pos=0;
				}

			} break;

			case MODE_SCREEN_ADJUST:
			{
				if(KB_ESC) 
				{
					set_mode(MODE_MENU);
				}

				if(KB_LEFT==1 || KB_LEFT==50)
				{
					if(k>SCREEN_CONTRAST_MIN) k--;
					if(KB_LEFT==50) KB_LEFT=49;
					AFLAGS_SET(FLAG_LCD_UPDATE); 
				}
	
				if(KB_RIGHT==1 || KB_RIGHT==50)
				{
					if(k<SCREEN_CONTRAST_MAX) k++;
					if(KB_RIGHT==50) KB_RIGHT=49;
					AFLAGS_SET(FLAG_LCD_UPDATE); 
				}
	
				if(KB_DOWN==1 || KB_DOWN==50)
				{
					if(l>SCREEN_BRIGHTNESS_MIN) l--;
					if(KB_DOWN==50) KB_DOWN=49;
					AFLAGS_SET(FLAG_LCD_UPDATE); 
				}
	
				if(KB_UP==1 || KB_UP==50)
				{
					if(l<SCREEN_BRIGHTNESS_MAX) l++;
					if(KB_UP==50) KB_UP=49;
					AFLAGS_SET(FLAG_LCD_UPDATE); 
				}
			} break;

			case MODE_TRIP_SETTINGS:
			{
				if(KB_ESC)
				{
					set_mode(MODE_MENU);
				}
			} break;

			case MODE_AIRCON_SETTINGS:
			{
				if(KB_ESC)
				{
					set_mode(MODE_MENU);
				}
			} break;

			case MODE_DATETIME_SETTINGS:
			{
				if(KB_ESC)
				{
					set_mode(MODE_MENU);
				}
			} break;

			case MODE_SERVICE:
			{
				if(KB_ESC)
				{
					set_mode(MODE_MENU);
				}
			} break;

			default: error_("Unkown mode", modestate);
		}
		/*
		 **************************************************
		*/


		if(AFLAGS_ISSET(FLAG_LCD_UPDATE))
		{
			AFLAGS_UNSET(FLAG_LCD_UPDATE); 

			switch(modestate)
			{
				case MODE_MENU:
				{
					if(AFLAGS_ISSET(FLAG_MODE_CHANGED))
					{
						gLCD_frame(0,0,127,63,1,true);
						for(i=0;i<MAIN_MENU_COUNT;i++)
						{
							gLCD_locate(4,4+i*10);
							fprintf(stdout,"%s",mainmenu_strings[i]);
						}
					}
					for(i=0;i<MAIN_MENU_COUNT;i++)
						gLCD_frame(2,i*10+2,125,i*10+12,1,false);
					gLCD_frame(2,mainmenu_pos*10+2,125,mainmenu_pos*10+12,1,true);
				} break;
			
				case MODE_MAIN:
				{
					uint16_t m_speed_m, m_speed_km, avg_speed_m, avg_speed_km;
					uint16_t m_fuel_h, m_fuel_100, avg_fuel_h, avg_fuel_100;
					uint32_t passed_distance;
	
					uint8_t seconds;
					uint8_t minutes;
					bool is12h;
					uint8_t hours;
					uint8_t day;
					uint8_t date;
					uint8_t month;
					uint8_t year;

					passed_distance=passed_speed_ticks*1000/SPEED_TICKS;
		
		
					m_fuel_h     = calc_fuel_h(last_inj_ticks,1);
					m_fuel_100   = calc_fuel_100(m_fuel_h,speed_ticks,1);
					avg_fuel_h   = calc_fuel_h(passed_inj_ticks,passed_seconds);
					avg_fuel_100 = calc_fuel_100(avg_fuel_h,passed_speed_ticks,passed_seconds);
			
					m_speed_m    = calc_speed_m(speed_ticks,1);
					m_speed_km   = m_speed_m * 36 / 10;
					avg_speed_m  = calc_speed_m(passed_speed_ticks,passed_seconds);
					avg_speed_km = avg_speed_m * 36 / 10;
		/*
					gLCD_locate(0,40);
					fprintf(stdout,"Inject  Speed   RPM");
					gLCD_locate(0,50);
					fprintf(stdout,"%5u  %5u  %5u",last_inj_ticks,speed_ticks,rpm_ticks*20);
*/
					// printing speed in km/h and m/s
					gLCD_locate(0,0);
					fprintf(stdout, "%3u km/h  %3u m/s",m_speed_km,m_speed_m);
	//				fprintf(stdout, "-");
	//				gLCD_echo(0,0,"-");
			
					// printing fuel consumption
					gLCD_locate(0,8);
					fprintf(stdout, " %3u.%u l/100km %2u.%u",ROUND1(m_fuel_100,3,1),ROUND2(m_fuel_100,3,1),ROUND1(m_fuel_h,3,1),ROUND2(m_fuel_h,3,1));
			
					// temporarily: checking for error
					if(ROUND2(m_fuel_100,3,1)>9) { gLCD_locate(0,0); fprintf(stdout,"%u",m_fuel_100); while(1);}
					
					// avarage speed and fuel consumption
					gLCD_locate(0,16);
					fprintf(stdout, "%3u km/h %3u.%u l/100",avg_speed_km,ROUND1(avg_fuel_100,3,1),ROUND2(avg_fuel_100,3,1));
						
					// printing journey time
					gLCD_locate(0,24);
					fprintf(stdout,"%02u:%02u:%02u", (uint8_t)(passed_seconds/3600),(uint8_t)((passed_seconds%3600)/60),(uint8_t)(passed_seconds%60));
			
					// printing passed dist
					gLCD_locate(50,24);
					fprintf(stdout, "%4u.%03u km",ROUND1(passed_distance,3,3),ROUND2(passed_distance,3,3));


					ds1307_read_time(&seconds, &minutes, &is12h, &hours, &day, &date, &month, &year);
					gLCD_locate(0,33);
					fprintf(stdout,"%02u-%02u-%02u  %u", date, month, year, day);
					gLCD_locate(0,42);
					fprintf(stdout,"%02u:%02u:%02u  %u", hours, minutes, seconds, (uint8_t)is12h );
 

				
				} break;


				case MODE_SCREEN_ADJUST:
				{
					uint8_t scr_tmp;

					gLCD_frame(0,0,127,63,1,true);
					gLCD_locate(14,3);
					fprintf(stdout,"SCREEN ADJUSTMENT");

					scr_tmp=((uint16_t)k-SCREEN_CONTRAST_MIN)*100/(SCREEN_CONTRAST_MAX-SCREEN_CONTRAST_MIN);
					gLCD_locate(5,18);
					fprintf(stdout,"Contrast LEFT/RIGHT");
					if_draw_progressbar(3,27,99,34,scr_tmp);
					gLCD_locate(102,27);
					fprintf(stdout,"%3u%%",scr_tmp);

					scr_tmp=((uint16_t)l-SCREEN_BRIGHTNESS_MIN)*100/(SCREEN_BRIGHTNESS_MAX-SCREEN_BRIGHTNESS_MIN);
					gLCD_locate(5,40);
					fprintf(stdout,"Brightness UP/DOWN");
					if_draw_progressbar(3,49,99,56,scr_tmp);
					gLCD_locate(102,49);
					fprintf(stdout,"%3u%%",scr_tmp);

					ds1803_write(0,k);
					ds1803_write(1,l);
				} break;
			
				case MODE_TRIP_SETTINGS:
				{
					gLCD_locate(2,2);
					fprintf(stdout,"Trip settings mode");
				} break;

				case MODE_AIRCON_SETTINGS:
				{
					gLCD_locate(2,2);
					fprintf(stdout,"AirCon settings mode");
				} break;

				case MODE_DATETIME_SETTINGS:
				{
					gLCD_locate(2,2);
					fprintf(stdout,"Datetime mode");
				} break;

				case MODE_SERVICE:
				{
					gLCD_locate(2,2);
					fprintf(stdout,"Service mode");
				} break;

				default: error_("Display unkown mode", modestate);
			} // switch
		} // if(AFLAGS_ISSET(FLAG_LCD_UPDATE))
		else
		{
			if(!AFLAGS_ISSET(FLAG_MODE_CHANGED)) _delay_ms(5);
		}
	}
	return 0;
}


SIGNAL(SIG_INTERRUPT0)
{
	uint8_t pin_inj_state=PIN_INJ, tcnt0;
	static uint16_t inj_ticks;
	
	if(!pin_inj_state) inj_ticks++;

	if(!clock_ticks)
	{
		tcnt0=TCNT0;
		TCNT0=0;
		speed_ticks=tcnt0_overs_sec*256+tcnt0;
		tcnt0_overs_sec=0;
		
		rpm_ticks=TCNT1;
		TCNT1=0;
		

		passed_seconds++;
		if(rpm_ticks) last_inj_ticks=inj_ticks;
		inj_ticks=0;
	
		passed_speed_ticks += speed_ticks;
		passed_inj_ticks += last_inj_ticks;
	

		if(modestate==MODE_MAIN)
		{
			AFLAGS_SET(FLAG_LCD_UPDATE);
		}
	}

	clock_ticks++;
	if(clock_ticks==32768)
	{
		clock_ticks=0;
	}
}


SIGNAL(SIG_OVERFLOW0)
{
	tcnt0_overs++;
	tcnt0_overs_sec++;
}

SIGNAL(SIG_OVERFLOW1)
{
	error("RPM counter overflow");
}
