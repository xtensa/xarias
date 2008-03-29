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

#define error(x) error_(x,0)
extern void error_(char *msg, uint8_t data);

/*
 * This function sets up TWI bus
 */
void inline twi_init()
{
	/*
	 * SCL = CPU_Freq / ( 16 + 2 * TWBR * 4^TWPS )
	 */

	TWBR = 98; // with prescaller = 1 gives us 100kHz TWI bus
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
	if ( TW_STATUS != TW_START) error("TWI START cmd");
}

/*
 * This function sends the device address through TWI bus.
 * It returns 0 on success and 1 if failed.
 */
uint8_t twi_write_addr(uint8_t addr)
{
	TWDR = TW_WRITE | addr; 
	TWCR = _BV(TWINT) | _BV(TWEN);

	/*
	 * waiting for TWINT flag set
	 */
	while (!(TWCR & _BV(TWINT)));

	if ( TW_STATUS != TW_MT_SLA_ACK) 
	{
		error("MT_SLA_ACK cmd");
		return 1;
	}

	return 0;
}

/*
 * This function sends one byte through TWI bus.
 * It returns 0 on success and 1 if failed.
 */
uint8_t twi_write_data(uint8_t data)
{
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);

	/*
	 * waiting for TWINT flag set
	 */
	while (!(TWCR & _BV(TWINT)));

	if ( TW_STATUS != TW_MT_DATA_ACK) 
	{
		error("MT_DATA_ACK cmd");
		return 1;
	}
	return 0;
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
	twi_write_addr(0x50); 


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
