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

#define VAL_TO_DS1307(x) x=(((x)/10)<<4)|((x)-(x)/10*10)
#define DS1307_TO_VAL(x) x=((x)>>4)*10+((x)&0x0F)

/*
 * This function sets up TWI bus
 */
void inline twi_init()
{
	/*
	 * SCL = CPU_Freq / ( 16 + 2 * TWBR * 4^TWPS )
	 */

	TWBR = 98; // with 16MHz cpu and prescaller = 1 gives us 100kHz TWI bus
	TWSR = 0;  // prescaller = 1
}

void inline twi_start()
{
	/*
	 * enablibg TWI and sending START condition
	 */
	TWCR = _BV(TWINT)|_BV(TWSTA)|_BV(TWEN);

	/*
	 * waiting for TWINT flag set
	 */
	while (!(TWCR & _BV(TWINT)));
	if ( TW_STATUS != TW_START && TW_STATUS != TW_REP_START ) error(ERROR_TWI_START_CMD);
}

/*
 * This function sends the device address through TWI bus.
 * It returns 0 on success and 1 if failed.
 */
void twi_rw_addr(uint8_t addr, uint8_t mode)
{
	TWDR = mode | addr; 
	TWCR = _BV(TWINT) | _BV(TWEN);

	/*
	 * waiting for TWINT flag set
	 */
	while (!(TWCR & _BV(TWINT)));

	if ( TW_STATUS != (mode==TW_WRITE ? TW_MT_SLA_ACK : TW_MR_SLA_ACK) ) 
	{
		error(ERROR_Mx_SLA_ACK_CMD);
	}

}

void twi_write_addr(uint8_t addr) 
{
	twi_rw_addr(addr,TW_WRITE);
}

void twi_read_addr(uint8_t addr)
{
	twi_rw_addr(addr,TW_READ);
}

/*
 * This function sends one byte through TWI bus.
 * It returns 0 on success and 1 if failed.
 */
void twi_write_data(uint8_t data)
{
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);

	/*
	 * waiting for TWINT flag set
	 */
	while (!(TWCR & _BV(TWINT)));

	if ( TW_STATUS != TW_MT_DATA_ACK) 
	{
		error(ERROR_MT_DATA_ACK_CMD);
	}
}


void twi_read_data(uint8_t *data, bool islast)
{

	TWCR = _BV(TWINT) | _BV(TWEN) | (islast?0:_BV(TWEA));

	/*
	 * waiting for TWINT flag set
	 */
	while (!(TWCR & _BV(TWINT)));

	if ( islast && TW_STATUS != TW_MR_DATA_NACK) 
	{
		error(ERROR_MR_DATA_NACK_CMD);
	}
	if ( !islast && TW_STATUS != TW_MR_DATA_ACK) 
	{
		error(ERROR_MR_DATA_ACK_CMD);
	}
	*data = TWDR;
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

	twi_start();


	/*
	 * Initializing potentiometers
	 * Sending device address 0101 000 for DS1803 chip
	 */
	twi_write_addr(TWIADDR_MAINPOT); 


	if(!pot)
		twi_write_data(0xA9);
	else 
		twi_write_data(0xAA);

	twi_write_data(val);

	twi_stop();

	/*
	 * Wait a little bit to release twi.
	 * According to DS1803 docs 4.5us should be enough, but it's not.
	 */
	_delay_us(10);
}


void ds1307_write_time(uint8_t seconds, uint8_t minutes, bool is12h, char *pmstr, uint8_t hours, uint8_t date, uint8_t month, uint8_t year)
{
	uint8_t prev_prog_part=prog_part;
	prog_part = PROGPART_DS1307_WRITETIME;

	if(seconds>59) error(ERROR_DS1307_SEC);
	VAL_TO_DS1307(seconds);
	seconds &= 0x7F;

	if(minutes>59) error(ERROR_DS1307_MIN);
	VAL_TO_DS1307(minutes);

	if( hours > 23 ) error(ERROR_DS1307_HOUR);
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

	if( date > 31 || !date) error(ERROR_DS1307_DATE);
	VAL_TO_DS1307(date);

	if( month > 12 || !month ) error(ERROR_DS1307_MONTH);
	VAL_TO_DS1307(month);

	if( year > 99 ) error(ERROR_DS1307_YEAR);
	VAL_TO_DS1307(year);


	twi_start();
	twi_write_addr(TWIADDR_DS1307);
	twi_write_data(0);
	twi_write_data(seconds);
	twi_write_data(minutes);
	twi_write_data(hours);
	twi_write_data(1); // day
	twi_write_data(date);
	twi_write_data(month);
	twi_write_data(year);
	twi_stop();

	prog_part = prev_prog_part;
}


void ds1307_read_time(  uint8_t *seconds, uint8_t *minutes, bool *is12h, char *pmstr, uint8_t *hours, 
			uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year)
{
	uint8_t prev_prog_part=prog_part;
	prog_part = PROGPART_DS1307_READTIME;

	twi_start();
	twi_write_addr(TWIADDR_DS1307);
	twi_write_data(0);
	twi_start();
	twi_read_addr(TWIADDR_DS1307);
	twi_read_data(seconds,false);
	twi_read_data(minutes,false);
	twi_read_data(hours,false);
	twi_read_data(day,false);
	twi_read_data(date,false);
	twi_read_data(month,false);
	twi_read_data(year,true);
	twi_stop();


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

	prog_part = prev_prog_part;
}

