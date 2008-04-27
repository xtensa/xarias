/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * General stdiodemo defines
 *
 * $Id: defines.h,v 1.1.2.2 2006/10/08 21:51:14 joerg_wunsch Exp $
 */

/* HD44780 LCD port connections */
#define HD44780_PORT D
#define HD44780_RS PORT0
#define HD44780_RW PORT1
#define HD44780_E  PORT2
/* The data bits have to be in ascending order. */
#define HD44780_D4 PORT4
#define HD44780_D5 PORT5
#define HD44780_D6 PORT6
#define HD44780_D7 PORT7


/* HD44780 LCD port connections */
//#define HD44780_PORT A
//#define HD44780_RS PORT0
//#define HD44780_RW PORT6
//#define HD44780_E  PORT1
/* The data bits have to be in ascending order. */
//#define HD44780_D4 PORT2
//#define HD44780_D5 PORT3
//#define HD44780_D6 PORT4
//#define HD44780_D7 PORT5
