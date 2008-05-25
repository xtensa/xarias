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


#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <stdbool.h>
#include <util/delay.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#include "xarias_b02.h"
#include "xarias_ac.h"
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
//#define SPEED_TICKS 2548
#define SPEED_TICKS 2325


//#define KB_SET(i)   kb_state |=  _BV(i)
//#define KB_UNSET(i) kb_state &= ~_BV(i)

/*
 * Main menu variables and constants.
 */
#define MAIN_MENU_COUNT 6
char mainmenu_strings[6][19]={	"Trip settings",
				"Date/Time settings",
				"Screen adjustment",
				"AirCon settings",
				"Service mode",
				"Exit"};

#define MODE_TRIP_SETTINGS	0x00
#define MODE_DATETIME_SETTINGS	0x01
#define MODE_SCREEN_ADJUST	0x02
#define MODE_AIRCON_SETTINGS	0x03
#define MODE_SERVICE		0x04
#define MODE_TRIP		0x05
#define MODE_MENU		0x06
#define MODE_FUEL		0x07
#define MODE_SPEED		0x08

#define MODE_SENSORS_INFO	0xF0
uint8_t mainmenu_pos=0, func_pos, subfunc_pos, MODE_MAIN=MODE_SPEED;

/*
 * The following variable is used to store current working mode
 */
uint8_t modestate;



/*
 * Keyboard Repeat Speed
 * Lower value generates faster repeat.
 * Allowed values: min=0 (the fastest), max=REPEAT_STROKES-1 (the slowest).
 */
uint8_t kb_repeat_speed=0;
#define REPEAT_STROKES 250


/*
 * Variable is used in TWI routines. Should be increased 
 * every second.
 */
volatile uint8_t debug_seconds;

/*
 * Action flags
 * To save RAM space several flags are stored in separate
 * bits of this action flag byte
 */
uint8_t aflags;
#define AFLAGS_SET(a) 	aflags|=(a)
#define AFLAGS_UNSET(a) aflags&=~(a)
#define AFLAGS_ISSET(a) (aflags&(a))
#define FLAG_AC_MODE		_BV(0)	// 0 - manual, 1 - auto
#define FLAG_AC_ONOFF		_BV(1)	// 0 - off, 1 - on
#define FLAG_IS_KM		_BV(2)
#define FLAG_IS_LITRES		_BV(3)
#define FLAG_IS_CELSIUS		_BV(4)

bool lcd_update=false, mode_changed=false, keyboard_pressed=false; 

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


void draw_frame01();

/*
 * These variables will be saved to DS1307 rom
 * If DS1307 has correct data, these values will be 
 * updated.
 */
uint8_t contrast=104, brightness=80;
char *currency=" EU";
uint8_t temp_ac=17;

/*
 * All variables for calculating speed, rpm, fuel consumption, etc.
 */
uint16_t tcnt0_overs_sec=0, speed_ticks=0, fuel_cost=420;
uint32_t passed_inj_ticks=0, passed_inj_ticks_overruns=0;
volatile uint32_t passed_seconds=0;
uint32_t tcnt0_overs=0, passed_speed_ticks=0, tot_fuel=0, tot_cost=0;
volatile uint16_t last_inj_ticks=0, rpm_ticks=0, clock_ticks=0;
bool is12h;
uint8_t seconds, minutes, hours, day, date, month, year;
char *pmstr="PM";
int32_t temp_out, temp_in;
///////////////////////////////
uint16_t m_speed_m=0, m_speed_km=0, avg_speed_m=0, avg_speed_km=0;
uint16_t m_fuel_h=0, m_fuel_100=0, avg_fuel_h=0, avg_fuel_100=0;
uint32_t passed_distance=0;

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

















/*
 * Use only this function to change mode. Do not change mode directly.
 */
void set_mode(uint8_t mode)
{
	modestate=mode;
	mode_changed=true;
	lcd_update=true;
	gLCD_cls();
	if(mode==MODE_SPEED || mode==MODE_TRIP || mode==MODE_FUEL)
	{
		MODE_MAIN=mode;
		draw_frame01();
	}
	if(mode==MODE_MENU)
	{
		func_pos=0;
		subfunc_pos=0;
	}
	kb_repeat_speed=0;
}




void error(uint8_t errcode)
{
	uint8_t i;
	gLCD_frame(29,21,107,35,1,false);
	gLCD_frame(30,22,106,34,2,true);
	gLCD_frame(32,24,104,32,1,false);
	gLCD_locate(33,25);
	if(errcode==ERROR_OK)
	{
		printf_P(PSTR("Data updated"));
	}
	else
	{
		printf_P(PSTR("ERROR: %02x-%02x"), prog_part, errcode);
	}
	for(i=0;i<20;i++) _delay_ms(50);
	gLCD_cls();
	if(modestate==MODE_MAIN)
	{
		draw_frame01();
	}
	lcd_update=true;
}


void ac_send_cmd(uint8_t ac_cmd)
{
	uint8_t length=1;

	twi_data_buf[0]=ac_cmd;
	switch(ac_cmd)
	{
		case AC_CMD_SET_MODE: 
		{
			twi_data_buf[1]=(aflags&3); // first two bits contain what we need
			length=2;
		} break;
		case AC_CMD_WRITE_TEMP:
		{
			int32_t t=((int32_t)temp_ac)*10000;
			twi_data_buf[1] = 1; // temp_ac is always in Celsius
			twi_data_buf[2] = (((int32_t)t)>>8)&0xFF;
			twi_data_buf[3] = (((int32_t)t)>>16)&0xFF;
			twi_data_buf[4] = (((int32_t)t)>>24)&0xFF;
		} break;
	}
	twi_write_str(TWIADDR_AC,length,true);
	_delay_us(10);
}

void save_aflags(uint8_t send_to_ac)
{
	twi_data_buf[0] = 0x0D;
	twi_data_buf[1] = aflags;
	twi_write_str(TWIADDR_DS1307, 2, true );

	if(send_to_ac) ac_send_cmd(AC_CMD_SET_MODE);
}

int32_t ac_get_temp(uint8_t sensor_no)
{
	twi_data_buf[0]=AC_CMD_READ_TEMP;
	twi_data_buf[1]=sensor_no;
	twi_data_buf[2]=AFLAGS_ISSET(FLAG_IS_CELSIUS);

	twi_write_str(TWIADDR_AC,3,false);
	twi_read_str(TWIADDR_AC,4,true);
	_delay_us(10);
	
	return 
		(((int32_t)twi_data_buf[0])    ) |
		(((int32_t)twi_data_buf[1])<<8 ) |
		(((int32_t)twi_data_buf[2])<<16) |
		(((int32_t)twi_data_buf[3])<<24) 
		;
}


/*
 * Do all the startup-time peripheral initializations.
 */
static void xarias_init(void)
{
	prog_part = PROGPART_INIT;

	gLCD_switchon();
	/*
	 * Setting up TWI bus.
	 */
	twi_init();

        TWAR = TWIADDR_MAINBOARD & 0xFE;

	ds1803_write(0,contrast);
	ds1803_write(1,brightness);


	/*
	 * First we should check if there was lack of power and ds1307 memory was erased.
	 * Writing address start 08h - the beginning of data memory.
	 * The check is simple: sum of two first bytes should give us third byte
	 */
	twi_data_buf[0] = 0x08;
	twi_write_str(TWIADDR_DS1307,1,false); // sending start address to read from
	twi_read_str(TWIADDR_DS1307,3,false);
	/*
	 * DS1307 memory map:
	 * 	0x08-0x0A : control byte (sum of two previous bytes)
	 *	0x0B      : contrast 
	 *	0x0C      : brightness
	 *	0x0D      : flag byte 
	 *	0x0E-0x10 : currency
	 *	0x11-0x12 : fuel cost
	 *	0x13      : desired A/C temperature
	 */
	if((uint8_t)(twi_data_buf[0]+twi_data_buf[1])!=twi_data_buf[2] || (!twi_data_buf[0] && !twi_data_buf[1]) )
	{
		/*
		 * First turn on LCD
		 */
		ds1803_write(0, contrast);
		ds1803_write(1, brightness);


		/*
		 * If condition is not met than we should initialize clock and additional bytes
		 */
		ds1307_write_time(1,1,true,"  ",1,1,1,1);
		ds1307_write_ctrl();


		srandom(twi_data_buf[0]+twi_data_buf[1]+twi_data_buf[2]);
		do{
			twi_data_buf[0]=(uint8_t)random();
		}while(!twi_data_buf[0]);
		do{
			twi_data_buf[1]=(uint8_t)random();
		}while(!twi_data_buf[1]);
		twi_data_buf[2]=twi_data_buf[0]+twi_data_buf[1];

		/*
		 * First byte should contain address to start from
		 */
		twi_data_buf[3]=twi_data_buf[2];
		twi_data_buf[2]=twi_data_buf[1];
		twi_data_buf[1]=twi_data_buf[0];
		twi_data_buf[0]=0x08;
		/*
		 * Writeing all the variables that should be saved
		 */
		twi_data_buf[4]  = (contrast);
		twi_data_buf[5]  = (brightness);
		twi_data_buf[6]  = aflags;
		twi_data_buf[7]  = (currency[0]);
		twi_data_buf[8]  = (currency[1]);
		twi_data_buf[9]  = (currency[2]);
		twi_data_buf[10]  = ((uint8_t)(fuel_cost>>8));
		twi_data_buf[11] = ((uint8_t)(fuel_cost));
		twi_data_buf[12] = (temp_ac);

		twi_write_str(TWIADDR_DS1307,13,true);

	}	
	else 
	{
		/*
		 * Reading existing data and restore settings
		 */


		twi_read_str(TWIADDR_DS1307,9,true);

		contrast    	= twi_data_buf[0];
		brightness  	= twi_data_buf[1];
		aflags		= twi_data_buf[2];
		currency[0] 	= (char)twi_data_buf[3];
		currency[1] 	= (char)twi_data_buf[4];
		currency[2] 	= twi_data_buf[5];
		fuel_cost 	= (uint16_t) (twi_data_buf[6]) << 8;
		fuel_cost      += twi_data_buf[7];
		temp_ac 	= twi_data_buf[8];

/*		twi_start();
		twi_read_addr(TWIADDR_DS1307);
		twi_read_byte(&contrast,false);
		twi_read_byte(&brightness,false);
		twi_read_byte(&tmpdata,false);
		is_km=tmpdata&_BV(0);
		is_litres=tmpdata&_BV(1);
		if(tmpdata&_BV(2))
			AFLAGS_SET(FLAG_AC_MODE);
		else
			AFLAGS_UNSET(FLAG_AC_MODE);
		twi_read_byte((uint8_t *)currency,    false);
		twi_read_byte((uint8_t *)(currency+1),false);
		twi_read_byte((uint8_t *)(currency+2),false);
		twi_read_byte(&tmpdata,false);
		fuel_cost=(uint16_t)tmpdata<<8;
		twi_read_byte(&tmpdata,false);
		fuel_cost+=tmpdata;
		twi_read_byte(&temp_ac,true);
*/
		ds1803_write(0,contrast);
		ds1803_write(1,brightness);
	}

	/*
	 * Setting up AC mode and ending desired temperature 
	 */
	ac_send_cmd(AC_CMD_SET_MODE);
	ac_send_cmd(AC_CMD_WRITE_TEMP);


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

//#define INJTICKS_MAX (UINT32_MAX/(SPEED_TICKS*15))
/*
 * Such number of ticks gives us INJ_FLOW ml fuel consumed
 */
#define INJTICKS_MAX (32768*60)

/*
 * This function return fuel consumption in l/h * 1000
 * We assume that fuel consumption is not greater than 65.536 l/h
 * Although l_inj_ticks is uint64_t, it cannot be greater than
 * xxxxxx and 
 */
/* old
uint16_t calc_fuel_h(uint64_t l_inj_ticks, uint32_t l_seconds)
{
	return (uint16_t)((l_inj_ticks*INJ_FLOW*15)/((uint64_t)2048*l_seconds));
}
*/


uint16_t calc_fuel_h(uint16_t l_inj_ticks)
{
	return ((((uint32_t)l_inj_ticks)*INJ_FLOW*15)/(2048));

//	if(retval>UINT16_MAX) retval=UINT16_MAX;
//	return (uint16_t) retval;
}


/* old 
uint16_t calc_fuel_100(uint16_t l_fuel_h, uint64_t l_speed_ticks, uint32_t l_seconds)
{
	 * If the distance we've passed is not too big we will get very big fuel consumption and 
	 * returned value will exceed 16 bits length integer. 
	uint64_t fuel_100=(((uint64_t)l_fuel_h*SPEED_TICKS*l_seconds)/(l_speed_ticks*36));

	if(fuel_100>65535)
		return 0;
	else
		return (uint16_t)fuel_100; 
}
*/

uint16_t calc_fuel_1002(uint16_t l_inj_ticks, uint16_t l_speed_ticks)
{
	return ((((uint64_t)l_inj_ticks)*INJ_FLOW*15*SPEED_TICKS)/((uint32_t)l_speed_ticks*36*2048));
}
uint16_t calc_fuel_100(uint16_t l_inj_ticks, uint16_t l_speed_ticks)
{
	uint32_t fuel_100=0, sum;
//	fuel_100=((((uint64_t)l_inj_ticks)*INJ_FLOW*15*SPEED_TICKS)/(((uint32_t)l_speed_ticks)*36*2048));

	sum=(((uint32_t)l_inj_ticks)*15*SPEED_TICKS);
	
	fuel_100=sum/2048;
	fuel_100=fuel_100*INJ_FLOW+(sum-fuel_100*2048)*INJ_FLOW/2048;
	return fuel_100/(l_speed_ticks*36); 
}



/* old 
uint32_t calc_fuel_total(uint64_t l_inj_ticks, uint64_t l_inj_ticks_overruns)
{
	return (uint32_t)(((l_inj_ticks+l_inj_ticks_overruns*INJTICKS_MAX)*INJ_FLOW)/(32768*60/4)UL);
}
*/
uint32_t calc_fuel_total()
{
	return passed_inj_ticks_overruns*INJ_FLOW+passed_inj_ticks*INJ_FLOW/(32768*60/4);
}


/* old
uint16_t calc_speed_m(uint64_t l_speed_ticks, uint32_t l_seconds)
{
	return (uint16_t)(l_speed_ticks*1000/(SPEED_TICKS*l_seconds));
}
*/

uint16_t calc_speed_m(uint16_t l_speed_ticks)
{
	return (uint16_t)((uint32_t)l_speed_ticks*1000/SPEED_TICKS);
}

uint32_t power(uint32_t x, uint8_t y)
{
	uint8_t i;
	uint32_t retval=1;
	for(i=0;i<y;i++,retval*=x);
	return retval;
}

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
		printf_P(PSTR("%u/%u"),i,j);
	else
		printf_P(PSTR("%u-%u"),i,j);
}

uint8_t inline kb_column_getstate(uint8_t col)
{
	if(col==0) return B02_GETSTATE(KB_COL1);
	if(col==1) return B02_GETSTATE(KB_COL2);
	if(col==2) return B02_GETSTATE(KB_COL3);
	if(col==3) return B02_GETSTATE(KB_COL4);
	return 0;
}

uint8_t get_days_in_month(uint8_t year, uint8_t month)
{
	if(month==4 || month==6 || month==9 || month==11)
	{
		return 30;
	}
	else if(month==2)
	{
		return 28+(uint8_t)(!(year%4));
	}

	return 31;
}

void draw_frame01()
{
	gLCD_frame(0,0,127,63,1,true);
	gLCD_line(89,0,89,63,true);
	gLCD_line(0,45,89,45,true);
	gLCD_line(44,46,44,63,true);
}



void draw_acinfo()
{
	char str_temp='F';

	if(AFLAGS_ISSET(FLAG_IS_CELSIUS)) str_temp='C';
	
	gLCD_locate(100,2)
	printf_P(PSTR("A/C"));
	if(AFLAGS_ISSET(FLAG_AC_MODE))
	{
		gLCD_locate(91,10);
		printf_P(PSTR("A:%2u^%c"),(AFLAGS_ISSET(FLAG_IS_CELSIUS)?temp_ac:temp_ac*9/5+32),str_temp);
	}
	else
	{
		gLCD_locate(91,10);
		printf_P(PSTR("Manual"));
	}
	
	if(AFLAGS_ISSET(FLAG_AC_ONOFF))
	{
		gLCD_locate(97,18)
		printf_P(PSTR(" ON "));
	}
	else
	{
		gLCD_locate(100,18)
		printf_P(PSTR("OFF"));
	}
	gLCD_line(89,27,127,27,true);
	/*
	 * Inside/Outside temperature
	 */
	temp_out = ac_get_temp(AC_TEMP_OUT_AVG);
	temp_in  = ac_get_temp(AC_TEMP_IN_AVG);
	
	gLCD_locate(103,29);
	printf_P(PSTR("In"));
	gLCD_locate(91,37);
	printf_P(PSTR("%3d^%c"),(int8_t)ROUND1(temp_in,4,4),str_temp);
	gLCD_line(89,45,127,45,true);
	gLCD_locate(103,47);
	printf_P(PSTR("Out"));
	gLCD_locate(91,55);
	printf_P(PSTR("%3d^%c"),(int8_t)ROUND1(temp_out,4,4),str_temp);
}

void draw_clock()
{
	ds1307_read_time(&seconds, &minutes, &is12h, pmstr, &hours, &day, &date, &month, &year);
	gLCD_locate(46,47);
	printf_P(PSTR("%s%2u:%02u"),pmstr,hours,minutes);
	gLCD_locate(58,55);
	printf_P(PSTR("%02u/%02u"),date,month);
}

void switch_pmstr()
{
	if(pmstr[0]=='A') pmstr[0]='P';
	else pmstr[0]='A';
}

int main()
{
	uint8_t i,j;
	uint8_t pin;
	int8_t inc;

	uint8_t kb_sth_pressed;
	uint8_t ac_pressed=0;
	

	/*
	 * Wait for power to stabilize
	 */
	_delay_ms(20);

	xarias_init();

	prog_part = PROGPART_MAIN;
/*
g=f=0;
while(f==g)
{
	passed_distance+=50;
	gLCD_locate(0,0);
	printf("start");
	gLCD_locate(0,10);
	clock_ticks=0;
	debug_seconds=0;
	printf("%lu",g=calc_fuel_100(passed_distance,434));
	gLCD_locate(0,20);
	printf("%u %u",debug_seconds, clock_ticks);

	clock_ticks=0;
	debug_seconds=0;
	gLCD_locate(0,30);
	printf("%lu",f=calc_fuel_1002(passed_distance,434));
	gLCD_locate(0,40);
	printf("%u %u",debug_seconds, clock_ticks);
}
	gLCD_locate(0,40);
	printf("%lu",passed_distance);
	return 0;
*/
	while(1)
	{
		char str_temp;
		str_temp=AFLAGS_ISSET(FLAG_IS_CELSIUS)?'C':'F';
		kb_sth_pressed=0;
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
						if(kb_state[i+j*4]==REPEAT_STROKES+1) kb_state[i+j*4]=REPEAT_STROKES - kb_repeat_speed;
					}
				}
				else
				{
					kb_state[i+j*4]=0;
				}
			//	kb_set(i,j,!pin);
				if(mode_changed) kb_state[i+j*4]=0;

			}
		} // end keyboard routine

		if(!kb_sth_pressed) mode_changed=false;
		
		/*
		 * key processing logics
		 */
		switch(modestate)
		{

			case MODE_TRIP:
			case MODE_FUEL:
			case MODE_SPEED:
			{
				if(KB_OK) 
				{ 
					set_mode(MODE_MENU);
				}

				if(KB_FUNC1==1)
				{
					set_mode(MODE_SPEED);
				}
				if(KB_FUNC2==1)
				{
					set_mode(MODE_FUEL);
				}
				if(KB_FUNC3==1)
				{
					set_mode(MODE_TRIP);
				}
				if(KB_FUNC4==1) 
				{
					gLCD_switchon();
					set_mode(MODE_MAIN);
					ds1803_write(0,contrast);
					ds1803_write(1,brightness);
				}
				if(KB_FUNC5==1) 
				{
					gLCD_switchoff();
				}
				
				if(KB_FUNC6==1) ac_pressed=1;
				
				if(!KB_FUNC6 && ac_pressed && ac_pressed<REPEAT_STROKES) // switching A/C ON or OFF
				{
					aflags ^= FLAG_AC_ONOFF;
					save_aflags(1);
					
					draw_acinfo();
					ac_pressed=0;
				}
				if(KB_FUNC6==REPEAT_STROKES && ac_pressed) ac_pressed++;

				if(ac_pressed==REPEAT_STROKES) // changing A/C mode
				{
					aflags ^= FLAG_AC_MODE;
					save_aflags(1);

					draw_acinfo();
					ac_pressed=0;
				}

				if(AFLAGS_ISSET(FLAG_AC_MODE) && temp_ac>=AC_TEMP_MIN && temp_ac<=AC_TEMP_MAX)
				{
					inc=0;
					if(temp_ac<AC_TEMP_MAX)
					{
						if(KB_UP==1) inc=1;
						if(KB_UP==REPEAT_STROKES) { inc=1; }
					}
					if(temp_ac>AC_TEMP_MIN)
					{
						if(KB_DOWN==1) inc=-1;
						if(KB_DOWN==REPEAT_STROKES) { inc=-1;  }
					}
					if(inc)
					{
						temp_ac+=inc;
						ac_send_cmd(AC_CMD_WRITE_TEMP);

						twi_data_buf[0] = 0x13;
						twi_data_buf[1] = temp_ac;
						twi_write_str(TWIADDR_DS1307, 2, true);

						draw_acinfo();
					}
				}

			} break;

			case MODE_MENU:
			{
				if(KB_DOWN==1 || KB_DOWN==REPEAT_STROKES) 
				{ 
					mainmenu_pos++;
					mainmenu_pos%=MAIN_MENU_COUNT;
					lcd_update=true;
				}
				if(KB_UP==1 || KB_UP==REPEAT_STROKES) 
				{ 
					if(mainmenu_pos)
						mainmenu_pos--;
					else
						mainmenu_pos=MAIN_MENU_COUNT-1;
					lcd_update=true;
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
				if(KB_ESC==1) 
				{
					/*
					twi_start();
					twi_write_addr(TWIADDR_DS1307);
					twi_write_byte(11);
					twi_write_byte(contrast);
					twi_write_byte(brightness);
					twi_stop();
					*/
						
					twi_data_buf[0] = 0x0B;
					twi_data_buf[1] = contrast;
					twi_data_buf[2] = brightness;
					twi_write_str(TWIADDR_DS1307, 3, true);


					set_mode(MODE_MENU);
				}

				if(KB_LEFT==1 || KB_LEFT==REPEAT_STROKES)
				{
					if(contrast>SCREEN_CONTRAST_MIN) contrast--;
					lcd_update=true;
				}
	
				if(KB_RIGHT==1 || KB_RIGHT==REPEAT_STROKES)
				{
					if(contrast<SCREEN_CONTRAST_MAX) contrast++;
					lcd_update=true;
				}
	
				if(KB_DOWN==1 || KB_DOWN==REPEAT_STROKES)
				{
					if(brightness>SCREEN_BRIGHTNESS_MIN) brightness--;
					lcd_update=true;
				}
	
				if(KB_UP==1 || KB_UP==REPEAT_STROKES)
				{
					if(brightness<SCREEN_BRIGHTNESS_MAX) brightness++;
					lcd_update=true;
				}
			} break;

			case MODE_TRIP_SETTINGS:
			{
				if(subfunc_pos)
				{
					gLCD_locate(113,13+10*func_pos);
					printf_P(PSTR("<<"));
					if(KB_ESC==1 || KB_OK==1)
					{
						gLCD_locate(113,13+10*func_pos);
						printf_P(PSTR("  "));
						subfunc_pos=0;
						// TODO: esc changes
						// TODO: save changes
						lcd_update=true;
					}

					switch(func_pos)
					{
						case 0: // changing fuel cost
						{
							kb_repeat_speed=5;
							if(KB_UP==1 || KB_UP==REPEAT_STROKES)
							{
								fuel_cost++;
							}
							if(KB_DOWN==1 || KB_DOWN==REPEAT_STROKES)
							{
								fuel_cost--;
							}
							gLCD_locate(68,13);
							printf_P(PSTR("%4u.%02u"),ROUND1(fuel_cost,2,2),ROUND2(fuel_cost,2,2));
						} break;
						case 1: // changing currency
						{
							if(subfunc_pos>1 && KB_LEFT==1) subfunc_pos--;
							if(subfunc_pos<3 && KB_RIGHT==1) subfunc_pos++;
	
							// [space] -> '@'
							if(currency[subfunc_pos-1]==' ') currency[subfunc_pos-1]='@';
							kb_repeat_speed=6;
	
							if(KB_UP==1 || KB_UP==REPEAT_STROKES)
							{
								currency[subfunc_pos-1]--;
								if(currency[subfunc_pos-1]==63) currency[subfunc_pos-1]=90;
							}
							if(KB_DOWN==1 || KB_DOWN==REPEAT_STROKES)
							{
								currency[subfunc_pos-1]++;
								if(currency[subfunc_pos-1]==91) currency[subfunc_pos-1]=64;
							}
	
							// @ -> [space] 
							if(currency[subfunc_pos-1]=='@') currency[subfunc_pos-1]=' ';
	
							gLCD_line(92,31,108,31,false);
							gLCD_line(92+(subfunc_pos-1)*6,31,90+subfunc_pos*6,31,true);
							gLCD_locate(92,23);
							printf_P(PSTR("%3s"),currency);
						} break;
						case 2: // changing kilometers/miles
						{
							kb_repeat_speed=11;
							if(KB_UP==1 || KB_DOWN==1 || KB_UP==REPEAT_STROKES || KB_DOWN==REPEAT_STROKES)
							{
								aflags ^= FLAG_IS_KM;
								save_aflags(0);

								gLCD_locate(68,33);
								printf_P(PSTR("%7s"),(AFLAGS_ISSET(FLAG_IS_KM)?"km":"miles"));
							}
						} break;
						case 3: // changing leters/gallons
						{
							kb_repeat_speed=11;
							if(KB_UP==1 || KB_DOWN==1 || KB_UP==REPEAT_STROKES || KB_DOWN==REPEAT_STROKES)
							{
								aflags ^= FLAG_IS_LITRES;
								save_aflags(0);

								gLCD_locate(68,43);
								printf_P(PSTR("%7s"),(AFLAGS_ISSET(FLAG_IS_LITRES)?"litres":"gallons"));
							}
						} break;
					}


				}
				else
				{
					if(KB_ESC==1)
					{
						set_mode(MODE_MENU);
					
						/*
						 * Saving all variables
						 */
						
						/*twi_start();
						twi_write_addr(TWIADDR_DS1307);
						twi_write_byte(0x0D);
						twi_write_byte(((uint8_t)is_km)|((uint8_t)is_litres<<1)|(AFLAGS_ISSET(FLAG_AC_MODE)?_BV(2):0));
						twi_write_byte(currency[0]);
						twi_write_byte(currency[1]);
						twi_write_byte(currency[2]);
						twi_write_byte((uint8_t)(fuel_cost>>8));
						twi_write_byte((uint8_t)(fuel_cost));
						twi_stop();
						*/


	                                        twi_data_buf[0] = 0x0D;
	                                        twi_data_buf[1] = aflags;
	                                        twi_data_buf[2] = currency[0];
	                                        twi_data_buf[3] = currency[1];
	                                        twi_data_buf[4] = currency[2];
	                                        twi_data_buf[5] = (uint8_t)(fuel_cost>>8);
	                                        twi_data_buf[6] = (uint8_t)(fuel_cost);
	                                        twi_write_str(TWIADDR_DS1307, 7, true);

					}
					
					if(KB_UP==1)
					{
						func_pos--;
						lcd_update=true;
					}

					if(KB_DOWN==1)
					{
						func_pos++;
						lcd_update=true;
					}

					if(KB_OK==1)
					{
						if(func_pos<4)
						{
							subfunc_pos=1;
							gLCD_frame(66,11+10*func_pos,110,21+10*func_pos,1,false);
						}
						else // reseting the trip
						{
							passed_seconds     		= 0;
							last_inj_ticks     		= 0;
							passed_inj_ticks   		= 0;
							passed_inj_ticks_overruns 	= 0;
							last_inj_ticks     		= 0;
							passed_speed_ticks 		= 0;

							ds1307_write_ctrl();

							error(ERROR_OK);
						} break;
					}

					if(func_pos==255) func_pos=4;
					func_pos%=5;
				}

			} break;

			case MODE_AIRCON_SETTINGS:
			{
				if(subfunc_pos)
				{
					gLCD_locate(113,13+10*func_pos);
					printf_P(PSTR("<<"));
					if(KB_ESC==1 || KB_OK==1)
					{
						gLCD_locate(113,13+10*func_pos);
						printf_P(PSTR("  "));
						subfunc_pos=0;
						// TODO: esc changes
						// TODO: save changes
						lcd_update=true;
					}

					switch(func_pos)
					{
						case 0: // changing fuel cost
						{
							kb_repeat_speed=5;
							if(KB_UP==1 || KB_UP==REPEAT_STROKES ||
							   KB_DOWN==1 || KB_DOWN==REPEAT_STROKES)
							{
								aflags ^= FLAG_IS_CELSIUS;
								if(AFLAGS_ISSET(FLAG_IS_CELSIUS))
								{
									str_temp='C';
								}
								else
								{
									str_temp='F';
								}
								gLCD_locate(104,13);
								putc(str_temp,stdout);
							}
						} break;
					}
				}
				else
				{
					if(KB_OK==1)
					{
						if(func_pos<1)
						{
							subfunc_pos=1;
							gLCD_frame(66,11+10*func_pos,110,21+10*func_pos,1,false);
						}
					}

					if(KB_ESC)
					{
						set_mode(MODE_MENU);
						save_aflags(0);
					}
					/* this section is not needed until we have only on position in menu
					if(KB_UP==1)
					{
						func_pos--;
						lcd_update=true;
					}

					if(KB_DOWN==1)
					{
						func_pos++;
						lcd_update=true;
					}

					if(func_pos==255) func_pos=1;
					func_pos%=1;
					*/
				}
			} break;

			case MODE_DATETIME_SETTINGS:
			{
				if(KB_ESC)
				{
					set_mode(MODE_MENU);
				}

				if(KB_RIGHT==1)
				{
					func_pos++;
					lcd_update=true;
				}

				if(KB_LEFT==1)
				{
					func_pos--;
					lcd_update=true;
				}

				if(func_pos==255) func_pos=6;
				func_pos%=7;

				if( ((KB_UP==1 || KB_UP==REPEAT_STROKES) && !KB_DOWN) ||
				    ((KB_DOWN==1 || KB_DOWN==REPEAT_STROKES) && !KB_UP) )
				{
					int8_t days_in_month;
					if(KB_UP==1 || KB_UP==REPEAT_STROKES) inc=1; else inc=-1;
					ds1307_read_time(&seconds, &minutes, &is12h, pmstr, &hours, &day, &date, &month, &year);


					switch(func_pos)
					{
						case 0: // date
							date += inc;
							days_in_month=get_days_in_month(year,month);
							if(date>days_in_month) date=1;
							if(!date) date=days_in_month;
						break;
						case 1: // month
							month += inc;
							if(month==13) month=1;
							if(!month) month=12;
							days_in_month=get_days_in_month(year,month);
							if(date>days_in_month) date=days_in_month;
						break;
						case 2: // year
							year += inc;
							if(year==100) year=0;
							if(year==255) year=99;
							days_in_month=get_days_in_month(year,month);
							if(date>days_in_month) date=days_in_month;
						break;
						case 3: // hours
							hours += inc;
							if(is12h)
							{
								if(hours==12 || (hours==11 && inc<0)) switch_pmstr();
								if(hours==13) hours=1;
								if(!hours) hours=12;
							}
							else
							{
								if(hours==24) hours=0;
								if(hours==255) hours=23;
							}


						break;
						case 4: // minutes
							minutes += inc;
							if(minutes==60) minutes=0;
							if(minutes==255) minutes=59;
						break;
						case 5: // seconds
							seconds += inc;
							if(seconds==60) seconds=0;
							if(seconds==255) seconds=59;
						break;
						case 6: // is12h
							is12h = !is12h;
							if(is12h)
							{
								pmstr[0]='A';
								pmstr[1]='M';
								if(hours>12) { hours-=12; pmstr[0]='P'; }
								if(hours==12) { pmstr[0]='P'; }
								if(!hours) { hours=12; }
							}
							else
							{
								if(pmstr[0]=='A' && hours==12) hours=0;
								if(pmstr[0]=='P' && hours<12) hours+=12;
							}
						break;
					}
					ds1307_write_time(seconds, minutes, is12h, pmstr, hours, date, month, year);
					lcd_update=true;
				}

			} break;

			case MODE_SERVICE:
			{
				if(KB_ESC)
				{
					set_mode(MODE_MENU);
				}
				if(KB_FUNC6==1)
				{
					set_mode(MODE_SENSORS_INFO);
					func_pos=0;
				}
			} break;

			case MODE_SENSORS_INFO:
			{
				if(KB_ESC)
				{
					set_mode(MODE_SERVICE);
				}
				if(KB_FUNC6==1)
				{
					gLCD_cls();
					lcd_update=true;
					func_pos++;
				}
			} break;

			default: error(ERROR_UNKNOWN_MODE);
		}
		/*
		 **************************************************
		*/


		//if(is_lcd_on && lcd_update)
		if(lcd_update)
		{
			char str_dist[3]="km", str_mdist[2]="m", str_vol='l';
			lcd_update=false;

			if(modestate==MODE_MAIN)
			{
			

				if(!AFLAGS_ISSET(FLAG_IS_KM))
				{
					str_dist[0]='m';
					str_dist[1]='i';

					m_speed_km      = ((uint32_t)m_speed_km)*1000/1609;
					avg_speed_km    = ((uint32_t)avg_speed_km)*1000/1609;
					passed_distance = ((uint32_t)passed_distance)*1000/1609;
					m_fuel_100      = ((uint32_t)m_fuel_100)*1609/1000;
					avg_fuel_100    = ((uint32_t)avg_fuel_100)*1609/1000;

				}
				if(!AFLAGS_ISSET(FLAG_IS_LITRES))
				{
					str_vol='g';

					tot_fuel     = ((uint32_t)tot_fuel)*1000/3785;
					m_fuel_h     = ((uint32_t)m_fuel_h)*1000/3785;
					avg_fuel_h   = ((uint32_t)avg_fuel_h)*1000/3785;
					m_fuel_100   = ((uint32_t)m_fuel_100)*1000/3785;
					avg_fuel_100 = ((uint32_t)avg_fuel_100)*1000/3785;
				}
			}


			switch(modestate)
			{
				case MODE_MENU:
				{
					if(mode_changed)
					{
						gLCD_frame(0,0,127,63,1,true);
						for(i=0;i<MAIN_MENU_COUNT;i++)
						{
							gLCD_locate(4,4+i*10);
							printf_P(PSTR("%s"),mainmenu_strings[i]);
						}
					}
					for(i=0;i<MAIN_MENU_COUNT;i++)
						gLCD_frame(2,i*10+2,125,i*10+12,1,false);
					gLCD_frame(2,mainmenu_pos*10+2,125,mainmenu_pos*10+12,1,true);
				} break;
			
				case MODE_SPEED:
				{
					gLCD_locate(2,2);
					printf_P(PSTR("SPEED"));

					/*
					 * MAIN for this mode - current speed
					 */
					gLCD_locate(15,11);
					fprintf(&gLCD_str16x24,"%3u",m_speed_km);
					gLCD_locate(65,28);
					printf_P(PSTR("%s/h"),str_dist);

					/*
					 * Speed in m/s
					 */
					gLCD_locate(47,2);
					printf_P(PSTR("%3u %s/s"),m_speed_m,str_mdist);

					/*
					 * Passed distance
					 */
					gLCD_locate(2,37);
					printf_P(PSTR("%s: %6u.%03u"),str_dist,ROUND1(passed_distance,3,3),ROUND2(passed_distance,3,3));

					/*
					 * Average speed
					 */
					gLCD_locate(2,47);
					printf_P(PSTR("Avg SPD"));
					gLCD_locate(14,55);
					printf_P(PSTR("%3u"),avg_speed_km);

					draw_clock();
					draw_acinfo();

				} break;

				case MODE_TRIP:
				{
					gLCD_locate(2,2);
					printf_P(PSTR("TRIP"));

					/*
					 * MAIN for this mode - passed km
					 */
					if(passed_distance>9999999UL)
					{
						gLCD_locate(2,11);
						fprintf(&gLCD_str16x24,"%5u",ROUND1(passed_distance,3,3));
					}
					else
					{
						gLCD_locate(12,11);
						fprintf(&gLCD_str16x24,"%4u",ROUND1(passed_distance,3,3));
						gLCD_locate(77,28);
						printf_P(PSTR("%s"),str_dist);
					}

					/*
					 * Passed meters
					 */
					gLCD_locate(59,2);
					printf_P(PSTR("%3u %s"),ROUND2(passed_distance,3,3),str_mdist);

					/*
					 * Trip time
					 */
					gLCD_locate(2,37);
					printf_P(PSTR("%4ud %02u:%02u:%02u"),	(uint16_t)(passed_seconds/86400),
										(uint8_t)(passed_seconds/3600),
										(uint8_t)((passed_seconds%3600)/60),
										(uint8_t)(passed_seconds%60));

					/*
					 * Trip fuel
					 */
					gLCD_locate(2,47);
					printf_P(PSTR("TotFuel"));
					gLCD_locate(2,55);
					printf_P(PSTR("%3u.%03u"),ROUND1(tot_fuel,3,3), ROUND2(tot_fuel,3,3));

					/*
					 * Trip fuel cost
					 */
					gLCD_locate(46,47);
					printf_P(PSTR("Cost%s"),currency);
					gLCD_locate(46,55);
					printf_P(PSTR("%4u.%02u"),ROUND1(tot_cost,2,2), ROUND2(tot_cost,2,2));

					draw_acinfo();

				
				} break;

				case MODE_FUEL:
				{
					gLCD_locate(2,2);
					printf_P(PSTR("FUEL"));

					/*
					 * MAIN for this mode - current fuel consumption
					 */
					gLCD_locate(18,11);
					fprintf(&gLCD_str16x24,"%2u.%u",ROUND1(m_fuel_100,3,1),ROUND2(m_fuel_100,3,1));
					gLCD_locate(42,37);
					printf_P(PSTR("%c/100%s"),str_vol,str_dist);

					/*
					 * Fuel consumption in l/h
					 */
					gLCD_locate(41,2);
					printf_P(PSTR("%2u.%u %c/h"),ROUND1(m_fuel_h,3,1),ROUND2(m_fuel_h,3,1),str_vol);

					/*
					 * Trip time
					 *
					gLCD_locate(2,37);
					printf_P(PSTR("%4ud %02u:%02u:%02u"),	(uint16_t)(passed_seconds/86400),
										(uint8_t)(passed_seconds/3600),
										(uint8_t)((passed_seconds%3600)/60),
										(uint8_t)(passed_seconds%60));
					*/
					/*
					 * Average fuel consumption
					 */
					gLCD_locate(2,47);
					printf_P(PSTR("AvgFuel"));
					gLCD_locate(2,55);
					printf_P(PSTR("%5u.%u"),ROUND1(avg_fuel_100,3,1), ROUND2(avg_fuel_100,3,1));

					/*
					 * Trip fuel cost
					 *
					gLCD_locate(46,47);
					printf_P(PSTR("Cost%s"),"PLN");
					gLCD_locate(46,55);
					printf_P(PSTR("%4u.%02u"),ROUND1(tot_cost,2,2), ROUND2(tot_cost,2,2));
					*/
					draw_clock();
					draw_acinfo();
				} break;


				case MODE_SCREEN_ADJUST:
				{
					uint8_t scr_tmp;

					gLCD_frame(0,0,127,63,1,true);
					gLCD_locate(14,3);
					printf_P(PSTR("SCREEN ADJUSTMENT"));

					scr_tmp=((uint16_t)contrast-SCREEN_CONTRAST_MIN)*100/(SCREEN_CONTRAST_MAX-SCREEN_CONTRAST_MIN);
					gLCD_locate(5,18);
					printf_P(PSTR("Contrast LEFT/RIGHT"));
					if_draw_progressbar(3,27,99,34,scr_tmp);
					gLCD_locate(102,27);
					printf_P(PSTR("%3u%%"),scr_tmp);

					scr_tmp=((uint16_t)brightness-SCREEN_BRIGHTNESS_MIN)*100/(SCREEN_BRIGHTNESS_MAX-SCREEN_BRIGHTNESS_MIN);
					gLCD_locate(5,40);
					printf_P(PSTR("Brightness UP/DOWN"));
					if_draw_progressbar(3,49,99,56,scr_tmp);
					gLCD_locate(102,49);
					printf_P(PSTR("%3u%%"),scr_tmp);

					ds1803_write(0,contrast);
					ds1803_write(1,brightness);
				} break;
			
				case MODE_TRIP_SETTINGS:
				{
					gLCD_frame(0,0,127,63,1,true);

					gLCD_locate(25,2);
					printf_P(PSTR("TRIP SETTINGS"));

					gLCD_locate(2,13);
					printf_P(PSTR("Fuel cost: %4u.%02u"),ROUND1(fuel_cost,2,2),ROUND2(fuel_cost,2,2));

					gLCD_locate(2,23);
					printf_P(PSTR("Currency :     %3s"),currency);

					gLCD_locate(2,33);
					printf_P(PSTR("Distance : %7s"),(AFLAGS_ISSET(FLAG_IS_KM)?"km":"miles"));

					gLCD_locate(2,43);
					printf_P(PSTR("Fuel     : %7s"),(AFLAGS_ISSET(FLAG_IS_LITRES)?"litres":"gallons"));

					gLCD_locate(30,53);
					printf_P(PSTR("Reset trip"));
					
					for(i=0;i<4;i++) 
					{
						if(func_pos!=i) gLCD_frame(66,11+10*i,110,21+10*i,1,false);
					}

					if(func_pos<4)
					{
						gLCD_frame(28,51,90,61,1,false);
						gLCD_frame(66,11+10*func_pos,110,21+10*func_pos,1,true);
					}
					else
					{
						gLCD_frame(28,51,90,61,1,true);
					}
				} break;

				case MODE_AIRCON_SETTINGS:
				{
					uint8_t dev_num;
					uint32_t temp;
					gLCD_frame(0,0,127,63,1,true);
					gLCD_locate(23,2);
					printf_P(PSTR("AIRCON SETTINGS"));

					gLCD_locate(2,13);
					printf_P(PSTR("Scale    :%8c"),str_temp);

					twi_data_buf[0]=AC_CMD_GET_1W_DEVS;
					twi_write_str(TWIADDR_AC,1,false);
					twi_read_str(TWIADDR_AC,17,true);

					dev_num=twi_data_buf[0];
					
					for(i=0;i<dev_num;i++)
					{
						temp=ac_get_temp(i);
						gLCD_locate(2,10*i+23);
						printf_P(PSTR("Sensor %d :%4d.%d^%c"),i,ROUND1(temp,4,1),ROUND2(temp,4,1),str_temp);
					}
	
					
					for(i=0;i<4;i++) 
					{
						if(func_pos!=i) gLCD_frame(66,11+10*i,110,21+10*i,1,false);
					}

					if(func_pos<4)
					{
						gLCD_frame(28,51,92,61,1,false);
						gLCD_frame(66,11+10*func_pos,110,21+10*func_pos,1,true);
					}
					else
					{
						gLCD_frame(28,51,92,61,1,true);
					}
				} break;

				case MODE_SENSORS_INFO:
				{
					int32_t temp;
					uint8_t dev_num;

					gLCD_frame(0,0,127,63,1,true);
					gLCD_locate(2,2);
					printf_P(PSTR("1-WIRE DEVICE INFO"));

					//-----------------
					twi_data_buf[0]=AC_CMD_GET_1W_DEVS;
					twi_write_str(TWIADDR_AC,1,false);
					twi_read_str(TWIADDR_AC,17,true);

					dev_num=twi_data_buf[0];
					gLCD_locate(2,12);
					printf_P(PSTR("Found %u devices"),dev_num);
				
					j=((dev_num+1)/2);
					func_pos %= j;
					
					for(i=func_pos*2;i<func_pos*2+2 && i<dev_num;i++)
					{
						
						gLCD_locate(2,16*(i%2)+23);
						printf_P(PSTR("D%u:%02x%02x%02x%02x%02x%02x%02x%02x"),
							i,
							twi_data_buf[i*8+8],
							twi_data_buf[i*8+7],
							twi_data_buf[i*8+6],
							twi_data_buf[i*8+5],
							twi_data_buf[i*8+4],
							twi_data_buf[i*8+3],
							twi_data_buf[i*8+2],
							twi_data_buf[i*8+1]);

						temp=ac_get_temp(i);
						gLCD_locate(2,16*(i%2)+31);
						printf_P(PSTR("t: %4d.%04d^%c"),ROUND1(temp,4,4), ROUND2(temp,4,4),str_temp);
					}

				/*	temp_out=ac_get_temp(AC_TEMP_OUT_AVG);
					temp_in=ac_get_temp(AC_TEMP_IN_AVG);
					gLCD_locate(80,32);
					printf_P(PSTR("%6ld   "),temp_out);
				*/
 				} break;

				case MODE_DATETIME_SETTINGS:
				{
					uint8_t xtab[]={14,56,98,14,56,98,44,69};
					uint8_t ytab[]={21,21,21,41,41,41,51,51};
					

					gLCD_frame(0,0,127,63,1,true);
					gLCD_locate(30,3);
 					printf_P(PSTR("DATE & TIME"));
					
					ds1307_read_time(&seconds, &minutes, &is12h, pmstr, &hours, &day, &date, &month, &year);
					gLCD_locate(10,13);
					printf_P(PSTR("Date  Month   Year"));
					
					gLCD_locate(16,23);
					printf_P(PSTR("%02u"), date);
					
					gLCD_locate(58,23);
					printf_P(PSTR("%02u"), month);
					
					gLCD_locate(100,23);
					printf_P(PSTR("%02u"), year);

					gLCD_locate(2,33);
					printf_P(PSTR("Hours Minutes Seconds"));

					gLCD_locate(16,43);
 					printf_P(PSTR("%02u"), hours);
					gLCD_locate(30,43);
					printf_P(PSTR("%s"), pmstr);
					
					gLCD_locate(58,43);
					printf_P(PSTR("%02u"), minutes);
					
					gLCD_locate(100,43);
					printf_P(PSTR("%02u"), seconds);

					gLCD_locate(10,53);
					printf_P(PSTR("Mode: %u h"),(is12h?12:24));

					for(i=0;i<7;i++) if(func_pos!=i) gLCD_frame(xtab[i],ytab[i],xtab[i]+14,ytab[i]+10,1,false);
					gLCD_frame(xtab[func_pos],ytab[func_pos],xtab[func_pos]+14,ytab[func_pos]+10,1,true);

				} break;

				case MODE_SERVICE:
				{
				/*
					gLCD_locate(0,40);
					printf_P(PSTR("Inject  Speed   RPM"));
					gLCD_locate(0,50);
					printf_P(PSTR("%5u  %5u  %5u"),last_inj_ticks,speed_ticks,rpm_ticks*20);
*/
					// printing speed in km/h and m/s
					gLCD_locate(2,2);
					printf( "%3u %s/h  %3u %s/s",m_speed_km,str_dist,m_speed_m,str_mdist);
	//				printf( "-");
	//				gLCD_echo(0,0,"-");
			
					// printing fuel consumption
					gLCD_locate(0,10);
					printf( " %3u.%u l/100%s %2u.%u",ROUND1(m_fuel_100,3,1),ROUND2(m_fuel_100,3,1),str_dist,ROUND1(m_fuel_h,3,1),ROUND2(m_fuel_h,3,1));
			
					// temporarily: checking for error
					if(ROUND2(m_fuel_100,3,1)>9) { gLCD_locate(0,0); printf_P(PSTR("%u"),m_fuel_100); while(1);}
					
					// avarage speed and fuel consumption
					gLCD_locate(0,18);
					printf( "%3u %s/h %3u.%u %c/100",avg_speed_km,str_dist,ROUND1(avg_fuel_100,3,1),ROUND2(avg_fuel_100,3,1),str_vol);
						
					// printing journey time
					gLCD_locate(0,26);
					printf_P(PSTR("%02u:%02u:%02u"), (uint8_t)(passed_seconds/3600),(uint8_t)((passed_seconds%3600)/60),(uint8_t)(passed_seconds%60));
			
					// printing passed dist
					gLCD_locate(50,26);
					printf( "%4u.%03u %s",ROUND1(passed_distance,3,3),ROUND2(passed_distance,3,3),str_dist);


					ds1307_read_time(&seconds, &minutes, &is12h, pmstr, &hours, &day, &date, &month, &year);
					gLCD_locate(0,34);
					printf_P(PSTR("%02u-%02u-%02u  %u"), date, month, year, day);
					gLCD_locate(0,42);
					printf_P(PSTR("%02u%s:%02u:%02u  %u"), hours, (is12h?pmstr:""),minutes, seconds, (uint8_t)is12h );



				} break;

			} // switch
		} // if(lcd_update)
		else
		{
			if(!mode_changed) _delay_ms(1);
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
	
		debug_seconds++;
		/*
		 * we do calculations only if engine is running
		 */
	//	rpm_ticks=1;
		if(rpm_ticks) 
		{
			passed_seconds++;
			last_inj_ticks=inj_ticks;
			passed_inj_ticks += last_inj_ticks;
			passed_speed_ticks += speed_ticks;

			if(passed_inj_ticks>INJTICKS_MAX)
			{
				passed_inj_ticks_overruns++;
				passed_inj_ticks -= INJTICKS_MAX;
			}
			passed_distance=passed_speed_ticks*1000/SPEED_TICKS;
	
			m_fuel_h     = calc_fuel_h(last_inj_ticks);
			m_fuel_100   = calc_fuel_100(last_inj_ticks,speed_ticks);
			m_speed_m    = calc_speed_m(speed_ticks);
			m_speed_km   = m_speed_m * 36 / 10;
			tot_fuel     = calc_fuel_total();
			tot_cost     = tot_fuel * fuel_cost / 1000;

			avg_fuel_h   = tot_fuel * 3600000 / passed_seconds;
			avg_fuel_100 = tot_fuel * 100000 / passed_distance;
			avg_speed_m  = passed_distance / passed_seconds;
			avg_speed_km = passed_distance * 36 / (passed_seconds*10);
		}
		else
		{
			last_inj_ticks 	= 0;
			m_fuel_h	= 0;
			m_fuel_100	= 0;
			m_speed_m	= 0;
			m_speed_km	= 0;
		}
		inj_ticks=0;
	
	

		if(modestate==MODE_MAIN || 
		   modestate==MODE_DATETIME_SETTINGS || 
		   modestate==MODE_SERVICE || 
		   modestate==MODE_SENSORS_INFO
		//   || modestate==MODE_AIRCON_SETTINGS
		)
		{
			lcd_update=true;
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
	error(ERROR_RPM_COUNTER_OVERFLOW);
}
