

# Program prefix
PRG            = xarias

# Programmer settings. Please see avrdude manpage for more details.
PROGRAMMER     = pony-stk200
PORT           = /dev/parport0

# Main board settings
MAIN_MCU_TARGET     = atmega32
MAIN_F_CPU     	    = 16000000UL

# AC board settings
AC_MCU_TARGET     = atmega8
AC_F_CPU          = 8000000UL

OBJ_B01        = xarias_b01.o hd44780_b01.o lcd_b01.o
OBJ_B02        = xarias_b02.o s6b0108_b02.o gLCD_b02.o twi_devs_b02.o
OBJ_AC         = xarias_ac.o one_wire_ac.o #twi_slave_ac.o 
OPTIMIZE       = -Os -mcall-prologues


#DEFS           = -D DEBUG 
LIBS           =
CC             = avr-gcc 

CFLAGS         = -g -Wall $(OPTIMIZE) -ffunction-sections ${DEFS}
LDFLAGS        = -Wl,--gc-sections,-Map,$(PRG).map -L /usr/lib/binutils/avr/2.18/

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump


CFLAGS        += $(if $(findstring _b01,$@),-mmcu=${MAIN_MCU_TARGET} -D F_CPU=${MAIN_F_CPU},)
CFLAGS        += $(if $(findstring _b02,$@),-mmcu=${MAIN_MCU_TARGET} -D F_CPU=${MAIN_F_CPU},)
CFLAGS        += $(if $(findstring _ac,$@),-mmcu=${AC_MCU_TARGET} -D F_CPU=${AC_F_CPU},)

LFUSE          = $(if $(findstring _ac,$@),FF,FF)
HFUSE          = $(if $(findstring _ac,$@),D9,CF)
AVRTARGET      = $(if $(findstring _ac,$@),m8,m32)

T1	       = $(patsubst %_ac.o,%.c,$@)
T2	       = $(patsubst %_b01.o,%.c,$(T1))
TARGET	       = $(patsubst %_b02.o,%.c,$(T2))


help:
	@echo "Usage: make XXX        - build firmware for XXX xarias board"
	@echo "       make inst_XXX   - build and install firmware for XXX board"
	@echo "       make fuses_XXX  - write fuse bits"
	@echo "Possible boards are b01, b02 and ac"
	@echo "See README for more info"

%:
	@echo "Invalid make target: $@"
	@echo "Try: make help"

#debug: clean
#	@echo "Debug enabled"

inst_%: %
	avrdude -V -p ${AVRTARGET} -c ${PROGRAMMER} -P ${PORT} -U flash:w:${PRG}_$*.hex:i -v -E noreset

fuses_%:
	avrdude -p ${AVRTARGET} -c ${PROGRAMMER} -P ${PORT} -U hfuse:w:0x${HFUSE}:m -v -E noreset 
	avrdude -p ${AVRTARGET} -c ${PROGRAMMER} -P ${PORT} -U lfuse:w:0x${LFUSE}:m -v -E noreset 

xarias_%.o: xarias_%.c xarias_%.h 
	$(CC) $(CFLAGS) -c $(subst .o,.c,$@) -o $@

%.o: prereqs_% 
	$(CC) $(CFLAGS) -c $(TARGET) -o $@

prereqs_%_ac prereqs_%_b01 prereqs_%_b02: %.c %.h
	@cat /dev/null	

b01: ${PRG}_b01.bin ${PRG}_b01.hex info_${PRG}_b01
	@cat /dev/null	

b02: ${PRG}_b02.bin ${PRG}_b02.hex info_${PRG}_b02
	@cat /dev/null	

ac:  ${PRG}_ac.bin ${PRG}_ac.hex info_${PRG}_ac
	@cat /dev/null	

xarias_b01.elf: ${OBJ_B01}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

xarias_b02.elf: ${OBJ_B02}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

xarias_ac.elf: ${OBJ_AC}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

	

clean:
	rm -rf *.o *.elf *.eps *.png *.bak *.hex *.bin
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)



%.lst: %.elf 
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

info_%: %.elf
	@echo -n "Size of the $*.bin: "
	@ls -l $*.bin | cut -d ' ' -f 5 -

%.hex: %.lst
	$(OBJCOPY) -j .text -j .data -O ihex $*.elf $*.hex

%.bin: %.lst
	$(OBJCOPY) -j .text -j .data -O binary $*.elf $*.bin

%.srec: %.lst
	$(OBJCOPY) -j .text -j .data -O srec $*.elf $*.srec

# Rules for building the .eeprom rom images

#eeprom: ehex ebin esrec

#ehex:  $(PRG)_eeprom.hex
#ebin:  $(PRG)_eeprom.bin
#esrec: $(PRG)_eeprom.srec

#%_eeprom.hex: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

#%_eeprom.srec: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@
#
#%_eeprom.bin: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

