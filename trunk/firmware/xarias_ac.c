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


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "xarias_b02.h"

#include <util/twi.h>
#include <stdbool.h>
#include "one_wire.h"


#define MODE_MANUAL	0
#define MODE_AUTO 	1

#define TURN_RELAY_ON() PORTD |= _BV(6)
#define TURN_RELAY_OFF() PORTD &= ~_BV(6)

#define TURN_AC_ON() PORTD |= _BV(7)
#define TURN_AC_OFF() PORTD &= ~_BV(7)

#define MAX_PARAMS_CNT 		3
#define MAX_RET_VALS_CNT	(8 * ONEW_MAX_DEVICE_COUNT + 1)

int16_t temp[ONEW_MAX_DEVICE_COUNT+2]={0}; // two extra virtual sensors
uint8_t ac_mode = 0; // manual, off

void xarias_ac_init()
{
        /*
	 * Enable external interrupt 0
	 */
	GICR = _BV(INT0);
	/*
	 * Falling edge will generate interrupt 0
	 */
	MCUCR |= _BV(1);
	DDRD  &= ~_BV(PORT2); // set pin to input 
	PORTD &= ~_BV(PORT2); // disable pull-up

	sei();



	/*
	 * Writing slave address
	 */
	TWAR = TWIADDR_AC & 0xFE;

	DDRC  &= ~(_BV(PORT4)|_BV(PORT5));
	//PORTC &= ~(_BV(PORT4)|_BV(PORT5));
	//PORTC |= _BV(PORT4)|_BV(PORT5);

	/*
	 * Enabling TWI, ack and TWI interrupt
	 */
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);


	// set PWM, phase correct mode
	TCCR2 |= _BV(WGM20);
	TCCR2 &= ~_BV(WGM21);

	// Clear OC2 on Compare Match when up-counting. Set OC2 on Compare
	// Match when downcounting.
	TCCR2 |= _BV(COM21);
//	TCCR2 &= ~(_BV(COM21)|_BV(COM20)); // normal port

	// For 4Mhz quarz CPU_FREQ/(256*2)
	// CS22 CS21 CS20 Description
	// 0 0 0 No clock source (Timer/Counter stopped).			4MHz		8MHz
	// 0 0 1 clkT2S/	(No prescaling) 	CPU_FREQ/(512)		7812 Hz		15625
	// 0 1 0 clkT2S/8 	(From prescaler) 	CPU_FREQ/(256*2*8)	976 Hz		1953
	// 0 1 1 clkT2S/32 	(From prescaler)	CPU_FREQ/(256*2*32)	488 Hz		976
	// 1 0 0 clkT2S/64 	(From prescaler)	CPU_FREQ/(256*2*64)	244 Hz		488
	// 1 0 1 clkT2S/128 	(From prescaler)	CPU_FREQ/(256*2*128)	122 Hz		244
	// 1 1 0 clkT2S/256 	(From prescaler)	CPU_FREQ/(256*2*256)	61 Hz		122
	// 1 1 1 clkT2S/1024 	(From prescaler)	CPU_FREQ/(256*2*1024)	7 Hz		14
	
	TCCR2 |= _BV(CS20) ; // 15625 PWM freq for 8Mhz

	// Setting OC2 as output
	DDRB |= _BV(3);
	//PORTB |= _BV(3);
	
	// Setting relay (6) and AC (7) pins as output
	DDRD |= _BV(6) | _BV(7);

}

void ac_set_mode(volatile uint8_t new_ac_mode)
{
	ac_mode=new_ac_mode;
	
	PORTB |= 1;

	if( ac_mode & _BV(AC_MODE) ) // AUTO 
	{
		if( ac_mode & _BV(AC_ONOFF) )
		{
			TURN_RELAY_ON();
		}
		else
		{
			TURN_AC_OFF();
			TURN_RELAY_OFF();
		}
	}
	else
	{
		TURN_RELAY_OFF();
		if( ac_mode & _BV(AC_ONOFF) )
		{
			TURN_AC_ON();
		}
		else
		{
			TURN_AC_OFF();
		}
	}

}

int main()
{

	uint8_t cmp=255;

	xarias_ac_init();

	OCR2=cmp;

	DDRB |= 1;
	PORTB &= ~1;

	return 0;
}
SIGNAL(SIG_2WIRE_SERIAL)
{
	static volatile uint8_t byte_no, cmd=AC_CMD_NOP, params[MAX_PARAMS_CNT], ret_vals[MAX_RET_VALS_CNT], ret_vals_cnt;
	int32_t temperature;
	uint8_t i,j;

	switch(TW_STATUS)
	{
		/*
		 * Slave receiver mode
		 */
		case TW_SR_SLA_ACK: 
		{
			byte_no=0;
		} break;

		case TW_SR_DATA_ACK:
//		case TW_SR_DATA_NACK:
		{
			if(byte_no>MAX_PARAMS_CNT) break;
			if(!byte_no) cmd=TWDR; // first byte is always command
			else params[byte_no-1]=TWDR;

			if(cmd==AC_CMD_READ_TEMP && byte_no==1)
			{
				temperature=temp[params[0]];
				temperature=ds18b20_temp_to_decimal((int16_t)temperature);
				temperature=ds18b20_temp_from_decimal(temperature);
				temperature=ds18b20_temp_to_decimal((int16_t)temperature);

				
				for(i=0;i<4;i++)
					ret_vals[i] = (temperature >> (i*8))&0xFF;

				ret_vals_cnt=4;

			}
			
			if(cmd==AC_CMD_WRITE_TEMP && byte_no==1)
			{
				// ....
			}
			
			if(cmd==AC_CMD_SET_MODE && byte_no==1)
			{
				ac_set_mode(params[0]);
			}

			if(cmd==AC_CMD_SEARCH_1W_DEVS)
			{
				onew_search_addresses();
				
			//	onew_dev_num=2;
			//	onew_dev_list[0]=0x23ef67ab06ULL;
			//	onew_dev_list[1]=0x10ec90dd56ea13aaULL;
				
				ret_vals[0] = onew_dev_num;
				for(i=0;i<onew_dev_num;i++)
				{
					for(j=0;j<8;j++)
					{
						ret_vals[i*8+j+1] = (uint8_t)((onew_dev_list[i] >> (8*j))&0xff);
					}
				}
				ret_vals_cnt = onew_dev_num*8+1;
			}

			byte_no++;
		} break;

		case TW_SR_STOP:
		{
			cmd=AC_CMD_NOP;
		} break;

		/*
		 * Slave transmitter mode
		 */
		case TW_ST_SLA_ACK:
		case TW_ST_DATA_ACK:
		{
			if(TW_STATUS==TW_ST_SLA_ACK) byte_no=0;
			if(byte_no>=ret_vals_cnt) break;
			TWDR=ret_vals[byte_no];
			byte_no++;
		} break;

/*
		case TW_ST_STOP:
		{
			cmd=AC_CMD_NOP;
		} break;
*/

	}

	TWCR |= _BV(TWINT);
}

SIGNAL(SIG_INTERRUPT0)
{
	static uint16_t clock_ticks=0;
	uint8_t i;

	if(!clock_ticks)
	{
		for(i=0;i<onew_dev_num;i++)
		{
			temp[i] = ds18b20_read_temp(i);
			ds18b20_convert_temp(i);
		}
		/*
		 * Calculating average temperatures FIXME
		 */
		temp[3]=temp[0];
		temp[4]=temp[1];
	}

	clock_ticks++;
	if(clock_ticks==32768)
	{
		clock_ticks=0;
	}

}


