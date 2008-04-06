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


/*
 * This function sets up TWI bus
 */
void inline twi_init();

uint8_t twi_write_addr(uint8_t addr);
uint8_t twi_write_data(uint8_t data);
void inline twi_start();
void inline twi_stop();

void ds1803_write(uint8_t pot, uint8_t val);
uint8_t ds1307_write_time(uint8_t seconds, uint8_t minutes, bool is12h, uint8_t hours, uint8_t date, uint8_t month, uint8_t year);
void ds1307_read_time(  uint8_t *seconds, uint8_t *minutes, bool *is12h, uint8_t *hours, 
			uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year);


