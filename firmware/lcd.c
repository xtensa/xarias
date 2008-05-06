/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Stdio demo, upper layer of LCD driver.
 *
 * $Id: lcd.c,v 1.1.2.1 2005/12/28 22:35:08 joerg_wunsch Exp $
 */

#include "xarias_b01.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>

#include <util/delay.h>

#include "hd44780.h"
#include "lcd.h"


FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);


/*
 * Setup the LCD controller.  First, call the hardware initialization
 * function, then adjust the display attributes we want.
 */
void lcd_init(void)
{

	hd44780_init();


	hd44780_outcmd(HD44780_FNSET(0,1,0));
	hd44780_wait_ready();


	/*
	* Clear the display.
	*/
	hd44780_outcmd(HD44780_CLR);
	hd44780_wait_ready();

	/*
	* Entry mode: auto-increment address counter, no display shift in
	* effect.
	*/
	hd44780_outcmd(HD44780_ENTMODE(1, 0));
	hd44780_wait_ready();

	/*
	* Enable display, deactivate non-blinking cursor.
	*/
	hd44780_outcmd(HD44780_DISPCTL(1, 0, 0));
	hd44780_wait_ready();


	stdout = &lcd_str;
}

void    lcd_cls()
{
	/*
	 * Clear the display.
	 */
	hd44780_outcmd(HD44780_CLR);
	hd44780_wait_ready();

	hd44780_outcmd(HD44780_HOME);
	hd44780_wait_ready();

}

/* 
 * Locate function for 4x20 lines LCD display which is simply 2x40 lines
 * display. 1st and 3rd line of 4x20 is just splitted line 1 of 2x40 and 
 * 2nd and 4th line of 4x20 is splitted 2nd line of 2x40
 */
uint8_t lcd_locate(uint8_t y, uint8_t x)
{

	if(x>20 || x==0)x=1;

        hd44780_outcmd(HD44780_HOME);
	hd44780_wait_ready();
	hd44780_outcmd(HD44780_DDADDR(x-1 + (y>2?1:0)*20 + (y+1)%2*0x40));
	hd44780_wait_ready();

	return 0;
}

/*
 * Send character c to the LCD display.  After a '\n' has been seen,
 * the next character will first clear the display.
 */
int lcd_putchar(char c, FILE *unused)
{
	hd44780_outdata(c);
	hd44780_wait_ready();

	return 0;
}
