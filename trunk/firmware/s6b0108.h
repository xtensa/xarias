
#ifndef S6B0108_H
#define S6B0108_H

#include "xarias_b02.h"
#include <avr/io.h>

#define MERGE(a,b) a##b

#define PORT(a) 	MERGE(PORT,a)
#define PIN(a) 		MERGE(PIN,a)
#define DDR(a) 		MERGE(DDR,a)

#define S6B0108_DISPLAY_ON	0x3F
#define S6B0108_DISPLAY_OFF	0x3E
#define S6B0108_SETY_MASK	0x40
#define S6B0108_SETX_MASK	0xB8
#define S6B0108_START_MASK	0xC0
#define S6B0108_BUSY_FLAG 	0x80
#define S6B0108_ON_FLAG 	0x20
#define S6B0108_RESET_FLAG	0x10

#define OUTPUT	0xFF
#define INPUT	0x00

#define s6b0108_outcmd(a)  s6b0108_outbyte(a,0)
#define s6b0108_outdata(a) s6b0108_outbyte(a,1)
#define s6b0108_incmd()   s6b0108_inbyte(0)
#define s6b0108_indata()  s6b0108_inbyte(1)


void    s6b0108_outbyte(uint8_t n, uint8_t rs);
uint8_t s6b0108_inbyte(uint8_t rs);

#endif // S6B0108_H
