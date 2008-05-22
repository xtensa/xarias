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



#ifndef _ONE_WIRE_H_
#define _ONE_WIRE_H_

/*
 * 1-Wire port settings
 */
#define ONEW_P_BUS_PORT		B
#define ONEW_BUS_PORT		0

/*
 * The number of allowed devices on 1-wire
 */
#define ONEW_MAX_DEVICE_COUNT	3


uint64_t onew_dev_list[ONEW_MAX_DEVICE_COUNT];
uint8_t onew_dev_num;

void onew_search_addresses();

void ds18b20_convert_temp(uint8_t dev); 
int16_t ds18b20_read_temp(uint8_t dev);
int32_t ds18b20_temp_to_decimal(int16_t temp);
int16_t ds18b20_temp_from_decimal(int32_t temp);

#endif
