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



//#include <ctype.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include "one_wire.h"


// ROM commands
#define DS18B20_ROM_SEARCH			0xF0
#define DS18B20_ROM_READ			0x33
#define DS18B20_ROM_MATCH			0x55
#define DS18B20_ROM_SKIP			0xCC
#define DS18B20_ROM_ALARM_SEARCH		0xEC

#define DS18B20_CMD_CONVERT 		0x44
#define DS18B20_CMD_READ_SCRATCHPAD	0xBE
#define DS18B20_CMD_WRITE_SCRATCHPAD	0x4E
#define DS18B20_CMD_COPY_SCRATCHPAD	0x48
#define DS18B20_CMD_RECALL_E2		0xB8
#define DS18B20_CMD_READ_POWER_SUPP	0xB4


#define MERGE(a,b) a##b
#define DUMMY(a,b) MERGE(a,b)
#define PORT(a) DUMMY(PORT,ONEW_P_##a)
#define PIN(a) DUMMY(PIN,ONEW_P_##a)
#define DDR(a) DUMMY(DDR,ONEW_P_##a)

#define PORT_SET(a) PORT(a)|=_BV(ONEW_##a)
#define PORT_UNSET(a) PORT(a)&=~_BV(ONEW_##a)

#define DDR_SET(a) DDR(a)|=_BV(ONEW_##a)
#define DDR_UNSET(a) DDR(a)&=~_BV(ONEW_##a)

#define PIN_GETSTATE(a) (PIN(a)&_BV(ONEW_##a))

#define ONEW_BUS_SET_OUTPUT()	DDR_SET(BUS_PORT)
#define ONEW_BUS_SET_INPUT()	DDR_UNSET(BUS_PORT)
#define ONEW_BUS_PULL_UP()	PORT_SET(BUS_PORT)
#define ONEW_BUS_PULL_DOWN()	PORT_UNSET(BUS_PORT)
#define ONEW_BUS_GET_STATE()	PIN_GETSTATE(BUS_PORT)

uint8_t onew_dev_list[ONEW_MAX_DEVICE_COUNT][8];
uint8_t onew_dev_num=0;


uint8_t onew_reset() 
{
	uint8_t result;

	ONEW_BUS_SET_OUTPUT();
	_delay_us(10);
	ONEW_BUS_PULL_UP();
	ONEW_BUS_PULL_DOWN();
	_delay_us(550);
	ONEW_BUS_PULL_UP();
	ONEW_BUS_SET_INPUT();
	_delay_us(35);
	result=ONEW_BUS_GET_STATE();
	_delay_us(500);

	return result; // 0 means OK
}

void onew_write_bit(uint8_t bit) 
{
	ONEW_BUS_SET_OUTPUT();
	ONEW_BUS_PULL_UP();
	_delay_us(1);
	if (bit) 
	{ 
		ONEW_BUS_PULL_DOWN();
		_delay_us(10); // release the bus within 15us 
		ONEW_BUS_PULL_UP();
		_delay_us(55); // wait until the end of the slot
	}
	else
	{
		ONEW_BUS_PULL_DOWN();
		_delay_us(65); // pull the bus low for at least 60us
		ONEW_BUS_PULL_UP();
	} 
	_delay_us(2); // recovery time
}

uint8_t onew_read_bit() 
{
	uint8_t result;
	ONEW_BUS_SET_OUTPUT();
	ONEW_BUS_PULL_DOWN();
	_delay_us(2);

	ONEW_BUS_PULL_UP(); 
	ONEW_BUS_SET_INPUT();
	_delay_us(9); // waiting for respond (<15us)
	result = ONEW_BUS_GET_STATE(); 
	result = result ? 1 : 0;
	_delay_us(55); // make sure the slot is >60us long
	return result;
}

void onew_write_byte(uint8_t command) 
{
	uint8_t i;
	for (i = 0; i < 8; i++) 
	{
		onew_write_bit(command & 1);
		command >>= 1;
	}
}

uint8_t onew_read_byte() 
{
	uint8_t byte=0,i;
	for (i = 0; i < 8; i++) 
	{
		byte |= (onew_read_bit() << i);
	}
	return byte;
}


uint8_t onew_calc_crc(uint8_t *p, uint8_t len)
{
	#define CRC_DIVISOR 281

	uint8_t i, j;
	uint16_t crc=0;

	if(!len) return 0;

	crc = p[0];
	for(i=0;i<len;i++)
	{
		if(i+1<len)
			crc |= (((uint16_t)p[i+1])<<8);

		for(j=0;j<8;j++)
		{
			if( crc & 1)
			{
				crc ^= CRC_DIVISOR;
			}
			crc >>= 1;
		}
	}

	return (uint8_t)(crc);
}



/*
 * 1-Wire is searched for available devices.
 * The result adresses are written to onew_dev_list table.
 * Maximum number of searched devices is limited by 
 * ONEW_MAX_DEVICE_COUNT
 */
void onew_search_addresses()
{
	uint8_t conf_mask[8]={0};
	uint8_t i=255, j, retcode;

	for(onew_dev_num=0;onew_dev_num<ONEW_MAX_DEVICE_COUNT;onew_dev_num++)
	{
		if(onew_reset()) return;
		onew_write_byte(DS18B20_ROM_SEARCH);
		if(onew_dev_num)
		{
			/*
			 * Search for the last conflict
			 */
			for(i=63; i<64; i--)
			{
				if(conf_mask[i/8] & (1<<(i%8))) break;
			}

			/*
			 * If i==255 than there was no conflicts and we found all devices
			 */
			if(i==255) 
			{
				onew_dev_num--;
				break;
			}

			/*
			 * Rewrite all bits before conflict bit
			 */
			for(j=0;j<i;j++)
			{
				onew_dev_list[onew_dev_num][j/8] |= (onew_dev_list[onew_dev_num-1][j/8] & (1<<(j%8)));
				
				onew_read_bit();
				onew_read_bit();
				onew_write_bit(onew_dev_list[onew_dev_num][j/8]&(1<<(j%8)));
			}

			/*
			 * In conflict position choose other option = 0
			 */
			if(i<64)
			{
				onew_dev_list[onew_dev_num][i/8] &= ~(1<<(i%8));
				
				onew_read_bit();
				onew_read_bit();
				onew_write_bit(0);

				conf_mask[i/8] &= ~(1<<(i%8));
			}


		}

		for(j=i+1;j<64;j++) 
		{
			retcode = onew_read_bit();
			retcode <<= 1;
			retcode |= 
			onew_read_bit();

			if (retcode == 1) // all zeros
			{
				onew_write_bit(0);
			}
			if (retcode == 2) // all ones
			{
				onew_dev_list[onew_dev_num][j/8] |= (1<<(j%8));
				onew_write_bit(1);
			}
			if (!retcode) // conflict
			{
				conf_mask[j/8] |= (1<<(j%8));
				onew_dev_list[onew_dev_num][j/8] |= (1<<(j%8));
				onew_write_bit(1);
			}

		}
	}

	if (onew_dev_num!=ONEW_MAX_DEVICE_COUNT) onew_dev_num++;
}

void onew_write_addr(uint8_t dev)
{
	uint8_t j;

	for(j=0;j<8;j++)
	{
		onew_write_byte(onew_dev_list[dev][j]);
	}
}

void ds18b20_convert_temp(uint8_t dev) 
{
	onew_reset();
	onew_write_byte(DS18B20_ROM_MATCH);
	onew_write_addr(dev);
	onew_write_byte(DS18B20_CMD_CONVERT);

	/*
	 * We will not wait until conversion is done, so the line remain commented
	 */
	//while(!onew_read_bit());
}

int16_t ds18b20_read_temp(uint8_t dev) 
{
	uint8_t scratchpad[9];
	uint8_t  i, try=0;
	bool crc_ok;

	do {
		crc_ok = true;
		for (i = 0; i < 9; i++) scratchpad[i] = 0;
		onew_reset();
		onew_write_byte(DS18B20_ROM_MATCH);
		onew_write_addr(dev);
		onew_write_byte(DS18B20_CMD_READ_SCRATCHPAD); 
		for (i = 0; i < 9; i++) 
		{
			scratchpad[i] = onew_read_byte();
		}

		if(scratchpad[8] != onew_calc_crc(scratchpad,8))
			crc_ok=false;

		try++;
	} while (!crc_ok && try<MAX_1WIRE_TRIES);
	return (((((int16_t)scratchpad[1]) << 8) | scratchpad[0]));
}

int32_t ds18b20_temp_to_decimal(int16_t temp)
{
	int32_t decimal_temp=0;
	uint8_t is_negative=0;
	
	if(temp<0) 
	{	
		temp *= -1;
		is_negative=1;
	}

	decimal_temp  = (((uint32_t) (temp&0x07F0))>>4 ) * 10000;

	decimal_temp += ((temp & 0x08)?5000:0);
	decimal_temp += ((temp & 0x04)?2500:0);
	decimal_temp += ((temp & 0x02)?1250:0);
	decimal_temp += ((temp & 0x01)?625:0);

	if(is_negative) decimal_temp *= -1;
	return decimal_temp;
}

int16_t ds18b20_temp_from_decimal(int32_t decimal_temp)
{
	int16_t temp=0;
	uint8_t is_negative=0;
	
	if(decimal_temp<0) 
	{	
		decimal_temp *= -1;
		is_negative=1;
	}

	temp = (decimal_temp/10000);
	decimal_temp -= ((uint32_t)temp)*10000;
	temp <<= 4;
	
	if(decimal_temp>=5000)
	{
		decimal_temp-=5000;
		temp |= 0x08;
	}	
	if(decimal_temp>=2500)
	{
		decimal_temp-=2500;
		temp |= 0x04;
	}	
	if(decimal_temp>=1250)
	{
		decimal_temp-=1250;
		temp |= 0x02;
	}	
	if(decimal_temp>=625)
	{
		temp |= 0x01;
	}

	if(is_negative) temp*=-1;
	return temp;

}
//		PORTB = ~(fahrenheit>>4);
