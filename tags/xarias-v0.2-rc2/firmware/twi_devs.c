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

#include <util/delay.h>
#include <util/twi.h>
#include <stdbool.h>

#include "xarias_b02.h"
#include "twi_devs.h"

#ifdef DEBUG
#include "gLCD.h"
#endif


extern volatile uint8_t debug_seconds;

#define VAL_TO_DS1307(x) x=(((x)/10)<<4)|((x)-(x)/10*10)
#define DS1307_TO_VAL(x) x=((x)>>4)*10+((x)&0x0F)

/*
 * Time in seconds that we will wait for end of TWI operation
 */
#define TWI_WAIT_TIME 1

/*
 * After such numer of tries the error will be displayed
 */
#define TWI_TRIES 3

/*
 * This function sets up TWI bus
 */
void inline twi_init()
{
	/*
	 * SCL = CPU_Freq / ( 16 + 2 * TWBR * 4^TWPS )
	 */

	TWBR = 8; // with 16MHz cpu and prescaller = 1 gives us 100kHz TWI bus
	TWSR = 0;  // prescaller = 1
}


void twi_recover()
{
	/*
	 * You will not find it in ATMega datasheet but I noticed,
	 * that if arbitration lost occure during Master Transmitter mode
	 * sending start conditiion has no effect. TWINT is never cleared 
	 * and TW_STATUS always return 0xF8. Nothing happens and 
	 * while(TWCR&_BV(TWINT)) loop will never end. The only way to resume
	 * communication is to disable TWI for a while.
	 */

#ifdef DEBUG
	while (debug_seconds<2);
	debug_seconds=0;
#endif
	TWCR &= ~_BV(TWEN);
	_delay_us(10);
//	twi_stop();
//	_delay_us(10);

}


uint8_t twi_start()
{

#ifdef DEBUG
	uint8_t t=TW_STATUS;
	gLCD_locate(0,0); 
	printf("%x    O  ", t);
#endif

	/* 
	 * Wait 10us just in case...
	 * ... if stop condition just was sent
	 */
	_delay_us(10);

#ifdef DEBUG
	gLCD_locate(0,0); 
	printf("%2x O  ST", t);
#endif
	/*
	 * enablibg TWI and sending START condition
	 */
	TWCR = _BV(TWINT)|_BV(TWSTA)|_BV(TWEN);

	/*
	 * waiting for TWINT flag set
	 */
	debug_seconds=0;
	while (!(TWCR & _BV(TWINT)) && debug_seconds<TWI_WAIT_TIME);

#ifdef DEBUG
	prog_part=TW_STATUS;
	gLCD_locate(24,0); 
	printf("K");
#endif

	if ( TW_STATUS != TW_START && TW_STATUS != TW_REP_START )
	{
		twi_recover();
		return TW_STATUS;
	}

	return ERROR_NONE;
}

/*
 * This function sends the device address through TWI bus.
 * It returns 0 on success and errorcode if failed.
 */
uint8_t twi_rw_addr(uint8_t addr, uint8_t mode)
{

#ifdef DEBUG
	uint8_t t=TW_STATUS;
	gLCD_locate(0,0); 
	printf("%x %c  ",t,(mode==TW_WRITE?'W':'R'));
#endif

	TWDR = mode | addr; 

	TWCR = _BV(TWINT) | _BV(TWEN);
	
	/*
	 * waiting for TWINT flag set
	 */
	debug_seconds=0;
	while (!(TWCR & _BV(TWINT)) && debug_seconds<TWI_WAIT_TIME);

#ifdef DEBUG
	gLCD_locate(24,0); 
	printf("B");
#endif

	if ( TW_STATUS != (mode==TW_WRITE ? TW_MT_SLA_ACK : TW_MR_SLA_ACK) ) 
	{
		twi_recover();
		return TW_STATUS;
	}
	
	return ERROR_NONE;
}

uint8_t inline twi_write_addr(uint8_t addr) 
{
	return twi_rw_addr(addr,TW_WRITE);
}

uint8_t inline twi_read_addr(uint8_t addr)
{
	return twi_rw_addr(addr,TW_READ);
}

/*
 * This function sends one byte through TWI bus.
 * It returns 0 on success and 1 if failed.
 */
uint8_t twi_write_byte(uint8_t data)
{

#ifdef DEBUG	
	uint8_t t=TW_STATUS;
	gLCD_locate(0,0); 
	printf("%x C  ",t);
#endif

	TWDR = data;

	TWCR = _BV(TWINT) | _BV(TWEN);
	/*
	 * waiting for TWINT flag set
	 */
	debug_seconds=0;
	while (!(TWCR & _BV(TWINT)) && debug_seconds<TWI_WAIT_TIME);

#ifdef DEBUF
	gLCD_locate(24,0); 
	printf("D");
#endif

	if ( TW_STATUS != TW_MT_DATA_ACK && TW_STATUS != 0xf8) 
	{
		twi_recover();
		return TW_STATUS;
	}
	return ERROR_NONE;
}


uint8_t twi_read_byte(uint8_t *data, bool islast)
{

#ifdef DEBUG
	uint8_t t=TW_STATUS;
	gLCD_locate(0,0); 
	printf("%x E  ",t);
#endif

	TWCR = _BV(TWINT) | _BV(TWEN) | (islast?0:_BV(TWEA));
	/*
	 * waiting for TWINT flag set
	 */
	debug_seconds=0;
	while (!(TWCR & _BV(TWINT)) && debug_seconds<TWI_WAIT_TIME);

#ifdef DEBUG
	gLCD_locate(24,0); 
	printf("F");
#endif 
	
	if ( islast && TW_STATUS != TW_MR_DATA_NACK) 
	{
		error(TW_STATUS);
	}
	if ( !islast && TW_STATUS != TW_MR_DATA_ACK) 
	{
		error(TW_STATUS);
	}

	*data = TWDR;
	return ERROR_NONE;
}


/*
 * stopping communication
 */
void inline twi_stop()
{
	TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWSTO);
}


void ds1803_write(uint8_t pot, uint8_t val)
{
	if(pot>1) return;

	twi_data_buf[0] = (pot?0xAA:0xA9);
	twi_data_buf[1] = val;

	twi_write_str(TWIADDR_MAINPOT,2,true);


	/*
	 * Wait a little bit to release twi.
	 * According to DS1803 docs 4.5us should be enough, but it's not.
	 */
	_delay_us(10);
}

void twi_read_str(uint8_t dev_addr, uint8_t count, bool release_twi)
{
	uint8_t i, tries=0, errcode=0;
	uint8_t prev_prog_part=prog_part;
	prog_part = PROGPART_TWI_READ;
	
	do 
	{
		tries++;
		if(tries>1) error(errcode);

		if((errcode=twi_start())) continue;
		if((errcode=twi_read_addr(dev_addr))) continue;
		for(i=0;i<count;i++)
		{
			if((errcode=twi_read_byte(twi_data_buf+i,i+1==count))) break;
		}
	} while( errcode && tries<TWI_TRIES );
	if( TW_STATUS != TW_MR_ARB_LOST && release_twi) twi_stop();

	if(errcode) error(errcode);
	
	prog_part = prev_prog_part;
}

void twi_write_str(uint8_t dev_addr, uint8_t count, bool release_twi)
{
	uint8_t prev_prog_part=prog_part, errcode=0, tries=0, i;

	prog_part = PROGPART_TWI_WRITE;

	do {
		tries++;
#ifdef DEBUG
		if(tries>1) error(errcode);
#endif
		
		if((errcode=twi_start())) continue;
		if((errcode=twi_write_addr(dev_addr))) continue;
		for(i=0;i<count;i++)
		{
			if((errcode=twi_write_byte(twi_data_buf[i]))) break;
		}
	} while( errcode && tries<TWI_TRIES );
	if( TW_STATUS != TW_MT_ARB_LOST && release_twi) twi_stop();

	if(errcode) error(errcode);

	prog_part = prev_prog_part;
}

void ds1307_write_ctrl()
{
	twi_data_buf[0]=0x00;
	twi_read_str(TWIADDR_DS1307,1,false);

	twi_data_buf[1] = twi_data_buf[0] & 0x7F;
	twi_data_buf[0] = 0x00;
	twi_write_str(TWIADDR_DS1307,2,false);

	twi_data_buf[0]=0x07;
	twi_data_buf[1]=_BV(4)|_BV(0)|_BV(1);
	twi_write_str(TWIADDR_DS1307,2,true);
}


void ds1307_write_time(uint8_t seconds, uint8_t minutes, bool is12h, char *pmstr, uint8_t hours, uint8_t date, uint8_t month, uint8_t year)
{
//	bool readlast;

//	if(seconds>59) error(ERROR_DS1307_SEC);
	VAL_TO_DS1307(seconds);
	seconds &= 0x7F;

//	if(minutes>59) error(ERROR_DS1307_MIN);
	VAL_TO_DS1307(minutes);

//	if( hours > 23 ) error(ERROR_DS1307_HOUR);
	if( is12h ) // 12h mode
	{
		VAL_TO_DS1307(hours);
		if(pmstr[0]=='P') 
			hours |= _BV(5); // set PM bit
		hours |= _BV(6); // set 12h bit
	}
	else
	{
		VAL_TO_DS1307(hours);
	}

//	if( date > 31 || !date) error(ERROR_DS1307_DATE);
	VAL_TO_DS1307(date);

//	if( month > 12 || !month ) error(ERROR_DS1307_MONTH);
	VAL_TO_DS1307(month);

//	if( year > 99 ) error(ERROR_DS1307_YEAR);
	VAL_TO_DS1307(year);

	twi_data_buf[0] = 0; // address
	twi_data_buf[1] = seconds;
	twi_data_buf[2] = minutes;
	twi_data_buf[3] = hours;
	twi_data_buf[4] = 1;
	twi_data_buf[5] = date;
	twi_data_buf[6] = month;
	twi_data_buf[7] = year;

	twi_write_str(TWIADDR_DS1307, 8, true );

/*
	do {
		readlast=false;
		tries++;
#ifdef DEBUG
		if(tries>1) error(errcode);
#endif
		
		if((errcode=twi_start())) continue;
		if((errcode=twi_write_addr(TWIADDR_DS1307))) continue;
		if((errcode=twi_write_byte(0))) continue;
		if((errcode=twi_write_byte(seconds))) continue;
		if((errcode=twi_write_byte(minutes))) continue;
		if((errcode=twi_write_byte(hours))) continue;
		if((errcode=twi_write_byte(1))) continue; // day
		if((errcode=twi_write_byte(date))) continue;
		if((errcode=twi_write_byte(month))) continue;
		if((errcode=twi_write_byte(year))) continue;
		readlast=true;
	} while( !readlast && tries<TWI_TRIES );
	if( TW_STATUS != TW_MT_ARB_LOST ) twi_stop();

	if(!readlast) error(errcode);
*/
	
}


void ds1307_read_time(  uint8_t *seconds, uint8_t *minutes, bool *is12h, char *pmstr, uint8_t *hours, 
			uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year)
{

	twi_data_buf[0]=0x00; // starting with address 0x00
	twi_write_str(TWIADDR_DS1307,1,false);
	twi_read_str(TWIADDR_DS1307,7,true);


/*
	do 
	{
		readlast=false;
		tries++;
		if(tries>1) error(errcode);

		if((errcode=twi_start())) continue;
		if((errcode=twi_write_addr(TWIADDR_DS1307))) continue;
		if((errcode=twi_write_byte(0))) continue;
		if((errcode=twi_start())) continue;
		if((errcode=twi_read_addr(TWIADDR_DS1307))) continue;
		if((errcode=twi_read_byte(seconds,false))) continue;
		if((errcode=twi_read_byte(minutes,false))) continue;
		if((errcode=twi_read_byte(hours,false))) continue;
		if((errcode=twi_read_byte(day,false))) continue;
		if((errcode=twi_read_byte(date,false))) continue;
		if((errcode=twi_read_byte(month,false))) continue;
		if((errcode=twi_read_byte(year,true))) continue;
		readlast=true;
	} while( !readlast && tries<TWI_TRIES );
	if( TW_STATUS != TW_MT_ARB_LOST ) twi_stop();
*/

	*seconds = twi_data_buf[0];
	*minutes = twi_data_buf[1];
	*hours   = twi_data_buf[2];
	*day     = twi_data_buf[3];
	*date    = twi_data_buf[4];
	*month   = twi_data_buf[5];
	*year    = twi_data_buf[6];

	*seconds &= 0x7F;
	DS1307_TO_VAL( (*seconds) );
	DS1307_TO_VAL( (*minutes) );
	*is12h=(bool)( *hours & _BV(6) );
	if( *is12h ) 
	{
		if( *hours & _BV(5) ) 
		{
			pmstr[0]='P';
		}
		else
		{
			pmstr[0]='A';
		}
		pmstr[1]='M';
		*hours &= 0x1F;
	}
	else
	{
		pmstr[0]=' ';
		pmstr[1]=' ';
		*hours &= 0x3F;
	}
	DS1307_TO_VAL( (*hours) );
	DS1307_TO_VAL( (*day) );
	DS1307_TO_VAL( (*date) );
	DS1307_TO_VAL( (*month) );
	DS1307_TO_VAL( (*year) );
}

