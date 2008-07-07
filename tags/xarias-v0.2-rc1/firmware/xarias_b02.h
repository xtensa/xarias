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

#ifndef XARIAS_B02_h
#define XARIAS_B02_h


#define MERGE(a,b) a##b
#define DUMMY(a,b) MERGE(a,b)
#define B02_PORT(a) DUMMY(PORT,B02_P_##a)
#define B02_PIN(a) DUMMY(PIN,B02_P_##a)
#define B02_DDR(a) DUMMY(DDR,B02_P_##a)

#define B02_PORT_SET(a) B02_PORT(a)|=_BV(B02_##a)
#define B02_PORT_UNSET(a) B02_PORT(a)&=~_BV(B02_##a)

#define B02_DDR_SET(a) B02_DDR(a)|=_BV(B02_##a)
#define B02_DDR_UNSET(a) B02_DDR(a)&=~_BV(B02_##a)

#define B02_GETSTATE(a) (B02_PIN(a)&_BV(B02_##a))


// resolution of the gLCD display
#define GLCD_RES_X	128
#define GLCD_RES_Y	64


#define SCREEN_CONTRAST_MIN   60
#define SCREEN_CONTRAST_MAX   150
#define SCREEN_BRIGHTNESS_MIN 28
#define SCREEN_BRIGHTNESS_MAX 150

/*
 * List of addresses of devices on TWI bus
 */
#define TWIADDR_DS1307 		0xD0
#define TWIADDR_MAINPOT 	0x50
#define TWIADDR_MAINBOARD	0xBA
#define TWIADDR_AC		0xAA



#define AC_TEMP_OUT_SENSOR	0
#define AC_TEMP_IN_SENSOR_1	1
#define AC_TEMP_IN_SENSOR_2	2

/*
 * It is assumed that pins of LCD are connected
 * to corresponding pins of MCU, e.g. 0 to 0,
 * 1 to 1, 2 to 2 etc.
 */
#define S6B0108_PDATA 	A

#define S6B0108_PCMD	C

#define RS    	7
#define RW    	6
#define E     	5
#define CS1   	4
#define CS2   	3
#define RST   	2

#define B02_P_LCD_POWER D
#define B02_LCD_POWER   1

#define B02_P_KB_ROW1 D
#define B02_P_KB_ROW2 D
#define B02_P_KB_ROW3 D
#define B02_KB_ROW1 4
#define B02_KB_ROW2 5
#define B02_KB_ROW3 6
#define B02_P_KB_COL1 B
#define B02_P_KB_COL2 B
#define B02_P_KB_COL3 B
#define B02_P_KB_COL4 B
#define B02_KB_COL1 4
#define B02_KB_COL2 5
#define B02_KB_COL3 6
#define B02_KB_COL4 7

/*
 * Error codes
 */
#define ERROR_NONE 			0x00
#define ERROR_UNKNOWN_MODE 		0x01
#define ERROR_RPM_COUNTER_OVERFLOW	0x02
#define ERROR_OK			0x03

#define ERROR_DS1307_SEC		0xB0
#define ERROR_DS1307_MIN		0xB1
#define ERROR_DS1307_HOUR		0xB2
#define ERROR_DS1307_DATE		0xB3
#define ERROR_DS1307_MONTH		0xB4
#define ERROR_DS1307_YEAR		0xB5

/*
#define ERROR_TWI_START_CMD		0xE3
#define ERROR_Mx_SLA_ACK_CMD		0xE4
#define ERROR_MT_DATA_ACK_CMD		0xE5
#define ERROR_MR_DATA_NACK_CMD		0xE6
#define ERROR_MR_DATA_ACK_CMD		0xE7
*/

#include <stdio.h>

/*
 * Function that displays error code and program part identifier defined by prog_part.
 * 
 * Return error codes (more common solution) are not used in this project beacuse of bigger 
 * memory consumption.
 */
void error(uint8_t errcode);

/*
 * Global variable indicating in which program part we are.
 * Should be set on the begining of each function that uses error() function.
 */
uint8_t prog_part;


/*
 * Use one of the below consts to assign to prog_part.
 */
#define PROGPART_MAIN 			0x01
#define PROGPART_INIT			0x02
#define PROGPART_LCD_UPDATE		0x03
#define PROGPART_DOORS_STATE		0x04
#define PROGPART_TWI_READ		0xA0
#define PROGPART_TWI_WRITE		0xA1

#endif
