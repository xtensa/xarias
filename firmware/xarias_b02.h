#ifndef XARIAS_B02_h
#define XARIAS_B02_h

//#define F_CPU 8000000UL
#define F_CPU 4000000UL

#define MERGE(a,b) a##b
#define DUMMY(a,b) MERGE(a,b)
#define B02_PORT(a) DUMMY(PORT,B02_P_##a)
#define B02_PIN(a) DUMMY(PIN,B02_P_##a)
#define B02_DDR(a) DUMMY(DDR,B02_P_##a)

#define B02_UP(a) B02_PORT(a)|=_BV(B02_##a)
#define B02_DOWN(a) B02_PORT(a)&=~_BV(B02_##a)


// resolution of the gLCD display
#define GLCD_RES_X	128
#define GLCD_RES_Y	64

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

#endif
