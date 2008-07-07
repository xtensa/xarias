/*
 * XARIAS carputer project
 *
 * Copyright (c) 2007 by Roman Pszonczenko xtensa <_at_> gmail
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

/* HD44780 LCD port connections */
/*#define HD44780_PORT D
#define HD44780_RS PORT0
#define HD44780_RW PORT1
#define HD44780_E  PORT2
// The data bits have to be in ascending order. 
#define HD44780_D4 PORT4
#define HD44780_D5 PORT5
#define HD44780_D6 PORT6
#define HD44780_D7 PORT7
*/

/* HD44780 LCD port connections */
#define HD44780_PORT A
#define HD44780_RS PORT0
#define HD44780_RW PORT6
#define HD44780_E  PORT1
/* The data bits have to be in ascending order. */
#define HD44780_D4 PORT2
#define HD44780_D5 PORT3
#define HD44780_D6 PORT4
#define HD44780_D7 PORT5
