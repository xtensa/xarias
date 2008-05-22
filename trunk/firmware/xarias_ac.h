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
 * AirCon temperature control
 */
#define AC_TEMP_MIN 5
#define AC_TEMP_MAX 35


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
