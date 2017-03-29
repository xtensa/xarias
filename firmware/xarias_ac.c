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
#include "xarias_ac.h"
#include "xarias_b02.h" // needed only to read TWIADDR_AC constant

#include <util/twi.h>
#include <stdbool.h>
#include "one_wire.h"


#define MODE_MANUAL	0
#define MODE_AUTO 	1

#define TURN_RELAY_ON() PORTD |= _BV(6)
#define TURN_RELAY_OFF() PORTD &= ~_BV(6)

#define TURN_AC_ON() PORTD |= _BV(7)
#define TURN_AC_OFF() PORTD &= ~_BV(7)
#define TURN_BLOWER_OFF() OCR2=255

#define MAX_PARAMS_CNT 		8
#define MAX_RET_VALS_CNT	(8 * ONEW_MAX_DEVICE_COUNT + 1)


int16_t temp[ONEW_MAX_DEVICE_COUNT+2]={0}; // two extra virtual sensors
volatile int16_t ac_desired_temp = 35<<4;
uint8_t ac_mode = 0; // manual, off

uint8_t beep_count=0, beep_tone=100;
uint16_t beep_interval=0; // in clock ticks, 32768 is equal to 1 second
uint16_t beep_duration=0; // in clock ticks, 32768 is equal to 1 second

void xarias_ac_init()
{
	bool crc_ok;

    

	/*
	 * Writing slave address
	 */
	TWAR = TWIADDR_AC & 0xFE;

	DDRC  &= ~(_BV(PORT4)|_BV(PORT5));

	/*
	 * Enabling TWI, ack and TWI interrupt
	 */
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);


	// set PWM for OC2, phase correct mode
	TCCR2 |= _BV(WGM20);
	TCCR2 &= ~_BV(WGM21);

	// set PWM for OC1B, 
	TCCR1A = _BV(COM1B1)|_BV(COM1B0) | _BV(WGM10);
	TCCR1B = _BV(CS11);// | _BV(WGM13);

	OCR1B = 255;

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

	// Setting OC2 and PB2 as output
	DDRB |= _BV(3) | _BV(2);
	//PORTB |= _BV(3);
	
	// Setting relay (6) and AC (7) pins as output
	DDRD |= _BV(6) | _BV(7);

	// Setting PB1 pin as output for main board interrupt generator
	DDRB |= _BV(1);

	// Setting door pins as input
	DDRD &= ~( _BV(0) | _BV(1));
	DDRC &= ~( _BV(0) | _BV(1) | _BV(2) | _BV(3) );
	// Enabling internal pullups on door pins
	PORTD |= ( _BV(0) | _BV(1));
	PORTC |= ( _BV(0) | _BV(1) | _BV(2) | _BV(3) );

	uint8_t try=0;

	do {
		uint8_t k;
		crc_ok = true;
		onew_search_addresses();
		for(k=0;k<onew_dev_num;k++)
			if(onew_dev_list[k][7] != onew_calc_crc(onew_dev_list[k],7) )
				crc_ok = false;
		try++;
	} while (!crc_ok && try<MAX_1WIRE_TRIES);

	/*
	 * Enable external interrupt 0 and 1
	 */
	GICR = _BV(INT0);
	/*
	 * Falling edge will generate interrupt 0 and 
	 * any logical change will generate interrupt 1
	 */
	MCUCR |= _BV(ISC01) | _BV(ISC10);
	DDRD  &= ~(_BV(PORT2)|_BV(PORT3)); // set pin to input 
	PORTD &= ~(_BV(PORT2)|_BV(PORT3)); // disable pull-up


	sei();

	
}

void ac_set_mode(volatile uint8_t new_ac_mode)
{
	ac_mode=new_ac_mode;
	

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
			TURN_BLOWER_OFF();
		}
	}
	else
	{
		TURN_RELAY_OFF();
		TURN_BLOWER_OFF();
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

int32_t inline decimal_celsius_to_fahrenheit(int32_t temp_c)
{
	return ((temp_c*9)/5 + 320000);
}

int32_t inline decimal_fahrenheit_to_celsius(int32_t temp_f)
{
	return ((temp_f-320000)*5/9);
}

/*
 * Setting blower power in percentage
 */
void ac_set_blower_power(uint8_t power)
{
	#define AC_BLOWER_PMAX	160 // THE SLOWEST
	#define AC_BLOWER_PMIN	0  // THE FASTEST
	if(power > 100) power=100;

	OCR2=AC_BLOWER_PMAX-((uint16_t)(AC_BLOWER_PMAX-AC_BLOWER_PMIN))*power/100;
}

void ac_control_auto()
{
	uint8_t blower_power;
	#define AC_DELTA_BOTTOM		3
	#define AC_DELTA_UP		5
	#define AC_MAX_BLOWER_TDELTA 	8
	#define AC_MAX_INOUT_TDELTA 	6
	#define MAX(a,b) ((a)>(b)?(a):(b))
	#define MIN(a,b) ((a)<(b)?(a):(b))

	if( (ac_mode & (_BV(AC_MODE)|_BV(AC_ONOFF)) ) == (_BV(AC_MODE)|_BV(AC_ONOFF)) )
	{
		if(temp[AC_TEMP_IN_AVG] > ac_desired_temp+(1<<4)-AC_DELTA_UP)
		{
			/*
			 * Now we're calculating the blower power to be set.
			 * First of all we take the difference between desired AC temperature and what we have inside.
			 * The greater is the difference, the greater is blower power. Maximum difference when the 
			 * blower power is set to 100% is defined by AC_MAX_BLOWER_TDELTA 
			 */
			blower_power = (MIN(MAX(temp[AC_TEMP_IN_AVG]-ac_desired_temp,0),AC_MAX_BLOWER_TDELTA<<4)*100) 
					/ ((AC_MAX_BLOWER_TDELTA<<4));
			/*
			 * In the next step we alse should take into account outside temperature.
			 * If the outside temperature is too high we also should increase blower power.
			 * Every one celsius degree of difference higher than AC_MAX_INOUT_TDELTA increases 
			 * blower power by 6%.
			 */
			blower_power += MIN((MAX(temp[AC_TEMP_OUT_AVG]-ac_desired_temp-(AC_MAX_INOUT_TDELTA<<4),0)*6) >>4, 100);

			ac_set_blower_power(blower_power);
			TURN_AC_ON();
		}
		if(temp[AC_TEMP_IN_AVG] < ac_desired_temp+AC_DELTA_BOTTOM)
		{
			ac_set_blower_power(0);
			TURN_AC_OFF();
		}
	}
	else
	{
		TURN_BLOWER_OFF();
	}

}

int8_t get_doors_state()
{
	uint8_t doors_state=0,i;

	#define DOOR_COUNT	6
	for(i=0;i<DOOR_COUNT;i++)
	{
		switch(i)
		{
			case 0:
			{
				doors_state |= ( (PIND&_BV(1)) ? 0 : 1 ) << 0;
			} break;
			case 1:
			{
				doors_state |= ( (PIND&_BV(0)) ? 0 : 1 ) << 1;
			} break;
			case 2:
			{
				doors_state |= ( (PINC&_BV(0)) ? 0 : 1 ) << 2;
			} break;
			case 3:
			{
				doors_state |= ( (PINC&_BV(1)) ? 0 : 1 ) << 3;
			} break;
			case 4:
			{
				doors_state |= ( (PINC&_BV(2)) ? 0 : 1 ) << 4;
			} break;
			case 5:
			{
				doors_state |= ( (PINC&_BV(3)) ? 0 : 1 ) << 5;
			} break;
		}
	}
	return doors_state;
}


void ac_set_beep(uint8_t beeps, uint8_t tone, uint16_t duration, uint16_t interval)
{
	if(!beep_count)
	{
		beep_count = beeps;
		beep_tone = tone;
		beep_duration = duration;
		beep_interval = interval;
	}
}

int main()
{
	uint8_t i;
	/*
	 * Wait for power to stabilize
	 */
	for(i=0;i<40;i++)
	{
		_delay_ms(10);
	}

	xarias_ac_init();

	ac_set_beep(4,1,10000,5000);
//	cli();	
//	DDRB |= _BV(3) | _BV(2);
/*	while(1)
	{
		_delay_ms(1);
		PORTB |= _BV(2);
		_delay_ms(1);
		PORTB &= ~_BV(2);
	}
*/
//	DDRB |= 1;
//	PORTB &= ~1;

	return 0;
}

ISR(TWI_vect)
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

			if(cmd==AC_CMD_READ_TEMP && byte_no==2)
			{
				temperature=temp[params[0]];
				
				temperature=ds18b20_temp_to_decimal((int16_t)temperature);
				
				if(!params[1]) // if fahrenheit
				{
					temperature = decimal_celsius_to_fahrenheit(temperature);
				}

				
				for(i=0;i<4;i++)
					ret_vals[i] = (temperature >> (i*8))&0xFF;

				ret_vals_cnt=4;

			}
			
			if(cmd==AC_CMD_WRITE_TEMP && byte_no==5)
			{
				temperature  = ((int32_t)params[4])<<24;
				temperature |= ((int32_t)params[3])<<16;
				temperature |= ((int32_t)params[2])<<8;
				temperature |= ((int32_t)params[1]);
				if(!params[0]) // is fahrenheit
				{
					temperature=decimal_fahrenheit_to_celsius(temperature);
				}
				ac_desired_temp=ds18b20_temp_from_decimal(temperature);
				ret_vals_cnt=0;
			}
			
			if(cmd==AC_CMD_SET_MODE && byte_no==1)
			{
				ac_set_mode(params[0]);
				_delay_ms(30);
				ac_control_auto();
				ret_vals_cnt=0;
			}
/* orig
			if(cmd==AC_CMD_GET_1W_DEVS && byte_no==0)
			{
				//onew_search_addresses();
				
				ret_vals[0] = onew_dev_num;
				for(i=0;i<onew_dev_num;i++)
				{
					for(j=0;j<8;j++)
					{
						ret_vals[i*8+j+1] = onew_dev_list[i][j];
					}
				}
				ret_vals_cnt = onew_dev_num*8+1;
			}
*/
			if(cmd==AC_CMD_GET_1W_DEVS_CNT && byte_no==0)
			{
				//onew_search_addresses();
				
				ret_vals[0] = onew_dev_num;
				ret_vals_cnt = 1;
			}
			if(cmd==AC_CMD_GET_1W_DEVS_ADDR && byte_no==1)
			{
				//onew_search_addresses();
				
				for(j=0;j<8;j++)
				{
					ret_vals[j] = onew_dev_list[params[0]][j];
				}
				ret_vals_cnt = 8;
			}

			if(cmd==AC_CMD_GET_DOORS && byte_no==0)
			{
				ret_vals_cnt = 1;
				ret_vals[0] = get_doors_state();
			}

			if(cmd==AC_CMD_MAKE_BEEPS && byte_no==4)
			{
				ac_set_beep(	params[0], 
						params[1],
						((uint16_t)params[2])*(32768/128),
						((uint16_t)params[3])*(32768/128)
						);
				ret_vals_cnt=0;
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


/*
 * Main clock interrupt
 */
ISR(INT0_vect)
{
	static uint16_t clock_ticks=0;
	uint8_t i;
	static uint16_t interval=0, duration=0;
	static uint8_t prev_doors_state=0;
	uint8_t doors_state;


	if(beep_count)
	{
		if(!interval)
		{
			OCR1B=120;
			duration=beep_duration;
			interval=beep_interval;
		}
		else
		{
			if(!duration)
			{
				OCR1B=255;
				if(interval) interval--;
				if(interval==1) beep_count--;
			}
			else
			{
				duration--;
			}
		}
	}


	if(!clock_ticks)
	{
	
		for(i=0;i<onew_dev_num;i++)
		{
			temp[i] = ds18b20_read_temp(i);
			ds18b20_convert_temp(i);
		}
		/*
		 * Calculating average temperatures - set it depending on your sensors layout
		 */
		temp[AC_TEMP_OUT_AVG]=temp[2];
		temp[AC_TEMP_IN_AVG]=(((int32_t)temp[1])+(int32_t)(temp[1]))/2;

		ac_control_auto();

	}

	if(clock_ticks==10000 || clock_ticks==20000 || clock_ticks==30000)
	{
		doors_state=get_doors_state();

		if(doors_state!=prev_doors_state)
		{
			_delay_us(50);
			if(doors_state==get_doors_state())
			{
				prev_doors_state=doors_state;
				PORTB |= _BV(1);
				_delay_us(2);
				PORTB &= ~_BV(1);
			}
		}
		
	}

	clock_ticks++;
	if(clock_ticks==32768)
	{
		clock_ticks=0;
	}

}

