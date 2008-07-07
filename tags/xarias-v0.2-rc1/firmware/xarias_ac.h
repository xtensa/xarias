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


#ifndef _XARIAS_AC_H_
#define _XARIAS_AC_H_

/*
 * AirCon minimum and maximum temperature in Celsius
 */
#define AC_TEMP_MIN 5
#define AC_TEMP_MAX 35

/*
 * The following commands are accepted through I2C bus. 
 * Number of params o returns is given in bytes in brackets.
 */
#define AC_CMD_NOP	 	0x90
/* No operation.
 * 	params: none
 * 	return: none  
 */
#define AC_CMD_READ_TEMP 	0x91 
/* Reading the temperature. It gets the sensor number to read and the scale
 * in which it should be returned. If is_fahrenheit=0 it means in Celsius,
 * otherwise in Fahrenheit. The temperature is returned as int32_t in decimal 
 * multiplied by 10000. For example 23.65C is returned as 23650.
 * 	params(2): sensor_no, is_celsius
 * 	return(4): temperature in decimal (4 bytes, LMB first) 
 */
#define AC_CMD_WRITE_TEMP	0x92 
/* Writing desired AC temperature. This temperature will be used when AC 
 * is in the AUTO mode.
 * 	params(5): is_celsius, temperature in decimal (4 bytes, LMB first) 
 * 	return(0): none 
 */
#define AC_CMD_SET_MODE		0x93 
/* Sets the AC mode. Please see mode mask bits below to learn what modes
 * are available.
 * 	params(1): mode_mask; 
 * 	return(0): none 
 */
#define AC_CMD_GET_1W_DEVS 	0x94 
/* This command returns all available devices on the 1-wire bus.
 * 	params(0): none
 * 	return(?): byte 1 - number of detected devices, than 8 bytes for each device 
 */
#define AC_CMD_GET_DOORS	0x95
/*
 * This command gets no parameters and return 1 byte - the mask of opened doors
 * in which every bit indicates on door. 1 - is open, 0 - is closed
 * 	params(0): none
 * 	return(1): opened doors bitmask
 */
#define AC_CMD_MAKE_BEEPS	0x96
/*
 * This command just makes <beeps_number> beeps of tone <tone>.
 * You can adjust duration and interval of beeps: 255 = 5 secs
 * 	params(4): tone, beeps_number, duration, interval
 */


/*
 * Mode mask bits
 */
#define AC_MODE			0 // 0 - Manual; 1 - Auto
#define AC_ONOFF		1 // 0 - Off;    1 - On


/*
 * Available 18B20 sensors map
 */
#define AC_TEMP_OUT_1		0
#define AC_TEMP_IN_1		1
#define AC_TEMP_IN_2		2

/*
 * Virtual sensors for average temperature
 */
#define AC_TEMP_OUT_AVG		3
#define AC_TEMP_IN_AVG		4

#endif
