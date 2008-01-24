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

#define S6B0108_P_RS  	A
#define S6B0108_RS    	0
#define S6B0108_P_RW  	A
#define S6B0108_RW    	1
#define S6B0108_P_E   	A
#define S6B0108_E     	2
#define S6B0108_P_DB0 	A
#define S6B0108_DB0   	3
#define S6B0108_P_DB1 	A
#define S6B0108_DB1   	4
#define S6B0108_P_DB2 	A
#define S6B0108_DB2   	5
#define S6B0108_P_DB3 	A
#define S6B0108_DB3   	6
#define S6B0108_P_DB4 	A
#define S6B0108_DB4   	7
#define S6B0108_P_DB5 	C
#define S6B0108_DB5   	7
#define S6B0108_P_DB6 	C
#define S6B0108_DB6   	6
#define S6B0108_P_DB7 	C
#define S6B0108_DB7   	5
#define S6B0108_P_CS1 	C
#define S6B0108_CS1   	4
#define S6B0108_P_CS2 	C
#define S6B0108_CS2   	3
#define S6B0108_P_RST 	C
#define S6B0108_RST   	2

#define B02_P_LCD_POWER D
#define B02_LCD_POWER   1

#endif
