
#ifdef F_CPU
#undef F_CPU
#endif
#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

/* HD44780 LCD port connections */
#define HD44780_PORT D
#define HD44780_RS PORT0
#define HD44780_RW PORT1
#define HD44780_E  PORT2
/* The data bits have to be in ascending order. */
#define HD44780_D4 PORT4
#define HD44780_D5 PORT4
#define HD44780_D6 PORT6
#define HD44780_D7 PORT7


int main()
{

	uint8_t cmp=255, up=0, down=0,wasp=0,pos=1;
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
//	TCCR2 |= _BV(CS22) | _BV(CS21) | _BV(CS20); // 14 for 8Mhz
//	TCCR2 |= _BV(CS22) | _BV(CS21); // 122 for 8Mhz
//	TCCR2 |= _BV(CS22) | _BV(CS20); // 244 for 8Mhz
//	TCCR2 |= _BV(CS22) ; // 488 for 8Mhz
//	TCCR2 |= _BV(CS21) | _BV(CS20); // 976 for 8Mhz
//	TCCR2 |= _BV(CS21) ; // 1952 for 8Mhz
	TCCR2 |= _BV(CS20) ; // 15625caler for 8Mhz

	// Setting OC2 as output
	DDRB |= _BV(3);
	//PORTB |= _BV(3);

	// Set PORTC1 and PORTC2 as input
	DDRC &= ~(_BV(1)|_BV(2)|_BV(3));
	PORTC |= _BV(1)|_BV(2)|_BV(3);
	SFIOR &= ~_BV(PUD);


	lcd_init();
	#define REPEAT_STROKES 240

	while(1)
	{
		_delay_us(100);
		OCR2=cmp;
		lcd_locate(1,1);
		fprintf(stderr,"%3u",cmp);
		if(!(PINC&_BV(1)))
		{
			_delay_us(100);
			if(up<255 && !(PINC&_BV(1)))
			{
				up++;
//				PORTB|=_BV(3);
				_delay_ms(1);
			}
		}
		else up=0;
		
		if(!(PINC&_BV(2)))
		{
			_delay_us(100);
			if(down<255 && !(PINC&_BV(2)))
			{
				down++;
//				PORTB&=~_BV(3);
				_delay_ms(1);
			}
		}
		else down=0;

		if(up>REPEAT_STROKES) up=REPEAT_STROKES-70;
		if(down>REPEAT_STROKES) down=REPEAT_STROKES-70;

		if((up==1 || up==REPEAT_STROKES) && cmp<254) cmp++;
		if((down==1 || down==REPEAT_STROKES) && cmp) cmp--;

		if(!(PINC&_BV(3)))
		{
			_delay_us(100);
			if(down<255 && !(PINC&_BV(3)))
			{
				_delay_ms(1);
				wasp=1;
			}
		}
		else if(wasp)
		{
			wasp=0;
			pos++;
			if(pos==8) pos=1;
			TCCR2&=~(uint8_t)7;
			TCCR2|=pos;

			lcd_locate(2,1);
			fprintf(stderr,"%3u",pos);		
		}

	}



	return 0;
}
