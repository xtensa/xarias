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


#ifndef _TWI_DEVS_H_
#define _TWI_DEVS_H_

/*
 * This is global variable that is used to read or write to I2C bus.
 */
uint8_t twi_data_buf[17];


/*
 * This function sets up TWI bus
 */
void inline twi_init();

//uint8_t inline twi_write_addr(uint8_t addr);
//uint8_t inline twi_write_byte(uint8_t data);
//uint8_t twi_read_addr(uint8_t addr);
//uint8_t twi_read_byte(uint8_t *data, bool islast);

void twi_read_str  (uint8_t dev_addr, uint8_t count, bool release_twi);
void twi_write_str (uint8_t dev_addr, uint8_t count, bool release_twi);


uint8_t        twi_start();
void    inline twi_stop();

/*
 * DS1803 routines
 */
void ds1803_write(uint8_t pot, uint8_t val);

/*
 * DS1307 routines
 */
void 	ds1307_write_ctrl();
void ds1307_write_time(uint8_t seconds, uint8_t minutes, bool is12h, char *pmstr, uint8_t hours, uint8_t date, uint8_t month, uint8_t year);
void ds1307_read_time(  uint8_t *seconds, uint8_t *minutes, bool *is12h, char *pmstr, uint8_t *hours, 
			uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year);

#endif
