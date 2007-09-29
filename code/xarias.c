/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Stdio demo
 *
 * $Id: stdiodemo.c,v 1.1.2.1 2005/12/28 22:35:08 joerg_wunsch Exp $
 */

#include "defines.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <util/delay.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#include "lcd.h"

#define PIN_INJ   PIND&_BV(3)
#define PORT_INJ  PORTD|_BV(3)


/*
 * Injector flow constant in ml/min
 * For Toyota Corolla 1,3 4EFE it's 136 - 176
 * For Toyota Corolla 1,6 4AFE it's 160 - 200
 */
#define INJ_FLOW 156

/*
 * Number of ticks per one kilometer
 */
#define SPEED_TICKS 2548


FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

uint32_t passed_seconds=0, tcnt0_overs=0, passed_speed_ticks=0;
uint16_t clock_ticks=0, inj_ticks=0, tcnt0_overs_sec=0;



void error(char *msg)
{
	stderr = &lcd_str;

	lcd_locate(2,1);
	fprintf(stderr,"ERR: %s",msg);
}


/*
 * This function sends one byte through TWI bus.
 * It returns 0 on success and 1 if failed.
 */
uint8_t twi_write_data(uint8_t data)
{
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);

	/*
	 * waiting for TWINT flag set
	 */
	while (!(TWCR & _BV(TWINT)));

	if ( TW_STATUS != TW_MT_DATA_ACK) 
	{
		error("MT_DATA_ACK cmd");
		return 1;
	}
	return 0;
}




/*
 * Do all the startup-time peripheral initializations.
 */
static void ioinit(void)
{
	lcd_init();

	/*
	 * SCL = CPU_Freq / ( 16 + 2 * TWBR * 4^TWPS )
	 */

	TWBR = 98; // with prescaller = 1 gives us 100kHz TWI bus
	TWSR = 0;  // prescaller = 1

	/*
	 * enablibg TWI and sending START condition
	 */
	TWCR = _BV(TWINT)|_BV(TWSTA)|_BV(TWEN);

	/*
	 * waiting for TWINT flag set
	 */
	while (!(TWCR & _BV(TWINT)));

	
	if ( TW_STATUS != TW_START) error("TWI START cmd");


	/*
	 * writing SLA_W and device address 1101 000
	 */
	TWDR = TW_WRITE | 0xD0; 
	TWCR = _BV(TWINT) | _BV(TWEN);

	/*
	 * waiting for TWINT flag set
	 */
	while (!(TWCR & _BV(TWINT)));

	if ( TW_STATUS != TW_MT_SLA_ACK) error("MT_SLA_ACK cmd");

	/*
	 * writing address start 0
	 */
	twi_write_data(0);

	twi_write_data(0);
	twi_write_data(0);
	twi_write_data(0);
	twi_write_data(0);
	twi_write_data(0);
	twi_write_data(0);
	twi_write_data(0);

	/*
	 * writing control register
	 * turning on output (bit 7) and setting 32,768 kHz (bits 0 and 1)
	 */
	twi_write_data(_BV(4)|_BV(0)|_BV(1));

	TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWSTO);




	/*
	 * Enable external interrupt 0
	 */
	GICR = _BV(INT0);

	/*
	 * Falling edge will generate interrupt 0
	 */
	MCUCR |= _BV(1);


	/*
	 * enable pull-up resistors for ports D2 and D3
	 */
	PORTD |= _BV(2) | _BV(3);

	/*
	 * enable all interrupts
	 */
	sei();


	/*
	 * external clock source for Timer0, clock on falling edge
	 */
	TCCR0 |= _BV(2) | _BV(1);
	/*
	 * enable Timer0 overflow interrupt
	 */
	TIMSK |= _BV(0);

}

/*
 * This function return fuel consumption in l/h * 1000
 * We assume that fuel consumption is not greater than 65.536 l/h
 * Although l_inj_ticks is uint64_t, it cannot be greater than
 * xxxxxx and 
 */
uint16_t calc_fuel_h(uint64_t l_inj_ticks, uint32_t l_seconds)
{
	return (uint16_t)((l_inj_ticks*INJ_FLOW*15)/((uint64_t)2048*l_seconds));
}

uint16_t calc_fuel_100(uint16_t l_fuel_h, uint64_t l_speed_ticks, uint32_t l_seconds)
{
	return (uint16_t)(((uint64_t)l_fuel_h*SPEED_TICKS*l_seconds)/(l_speed_ticks*36));
}

uint16_t calc_speed_m(uint64_t l_speed_ticks, uint32_t l_seconds)
{
	return (uint16_t)(l_speed_ticks*1000/(SPEED_TICKS*l_seconds));
}


uint32_t power(uint32_t x, uint8_t y)
{
	uint8_t i;
	uint32_t retval=1;
	for(i=0;i<y;i++,retval*=x);
	return retval;
}

/*
 * Cuts last p digits of the value
 */
#define CUT(val,mult) ((uint16_t)((val)/power(10,(uint8_t)mult)))

#define ROUND(val,mult,prec) ((uint16_t)(val+5*power(10,(uint8_t)mult-prec-1)))

#define ROUND1(val,mult,prec) ((uint16_t)CUT(ROUND(val,mult,prec),mult))

#define ROUND2(val,mult,prec) (uint16_t)(CUT(ROUND(val,mult,prec)-CUT(ROUND(val,mult,prec),mult)*power(10,(uint8_t)mult),mult-prec))


int main()
{
	stderr = &lcd_str;

	ioinit();

	return 0;
}


SIGNAL(SIG_INTERRUPT0)
{
	static uint64_t passed_inj_ticks=0;

	uint8_t pin_inj_state=PIN_INJ;
	if(!pin_inj_state) inj_ticks++;

	if(!clock_ticks)
	{
		uint16_t m_speed_m, m_speed_km, avg_speed_m, avg_speed_km;
		uint16_t m_fuel_h, m_fuel_100, avg_fuel_h, avg_fuel_100, speed_ticks;
		uint32_t passed_distance;
		uint8_t  tcnt0;
		
		tcnt0=TCNT0;
		TCNT0=0;
		tcnt0_overs_sec=0;

		speed_ticks=tcnt0_overs_sec*256+tcnt0;
		passed_speed_ticks += speed_ticks;
		passed_inj_ticks += inj_ticks;
		passed_distance=passed_speed_ticks*1000/SPEED_TICKS;

		m_fuel_h     = calc_fuel_h(inj_ticks,1);
		m_fuel_100   = calc_fuel_100(m_fuel_h,speed_ticks,1);
		avg_fuel_h   = calc_fuel_h(passed_inj_ticks,passed_seconds);
		avg_fuel_100 = calc_fuel_100(avg_fuel_h,passed_speed_ticks,passed_seconds);

		m_speed_m    = calc_speed_m(speed_ticks,1);
		m_speed_km   = m_speed_m * 36 / 10;
		avg_speed_m  = calc_speed_m(passed_speed_ticks,passed_seconds);
		avg_speed_km = avg_speed_m * 36 / 10;

	
		lcd_locate(1,1);
		fprintf(stderr, "%3u km/h  %3u m/s",m_speed_km,m_speed_m);

		lcd_locate(2,1);
		fprintf(stderr, " %3u.%u l/100km %2u.%u l/h",ROUND1(m_fuel_100,3,1),ROUND2(m_fuel_100,3,1),ROUND1(m_fuel_h,3,1),ROUND2(m_fuel_h,3,1));

		if(ROUND2(m_fuel_100,3,1)>9) { lcd_locate(1,1); fprintf(stderr,"%u",m_fuel_100); while(1);}
		
		lcd_locate(3,1);
		fprintf(stderr, "%3u km/h %3u.%u l/100",avg_speed_km,ROUND1(avg_fuel_100,3,1),ROUND2(avg_fuel_100,3,1));
		
		lcd_locate(4,1);
		//fprintf(stderr, "Passed %3u",speed_ticks);
		//fprintf(stderr, "Passed %lu km.",passed_speed_ticks);
		fprintf(stderr, "Passed %u.%3u km.",ROUND1(passed_distance,3,3),ROUND2(passed_distance,3,3));
			
	
		passed_seconds++;
		inj_ticks=0;
	}

	clock_ticks++;
	if(clock_ticks==32768)
	{
		clock_ticks=0;
	}
}


SIGNAL(SIG_OVERFLOW0)
{
	tcnt0_overs++;
	tcnt0_overs_sec++;
}
