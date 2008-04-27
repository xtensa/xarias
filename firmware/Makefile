PRG            = xarias
OBJ_B01        = xarias_b01.o hd44780.o lcd.o
OBJ_B02        = xarias_b02.o s6b0108.o gLCD.o twi_devs.o
OBJ_AC         = xarias_ac.o lcd.o hd44780.o
MCU_TARGET     = atmega8
OPTIMIZE       = -Os -mcall-prologues
F_CPU_MAIN     = 16000000UL
F_CPU_AC       = 2000000UL

DEFS           =
LIBS           =

# You should not have to change anything below here.

CC             = avr-gcc 


CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -ffunction-sections -D "F_CPU=${F_CPU_AC}"
LDFLAGS       = -Wl,--gc-sections,-Map,$(PRG).map -L /usr/lib/binutils/avr/2.18/

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all:
	@echo "Usage: make XXX      - build firmware for XXX xarias board"
	@echo "       make inst_XXX - build and install firmware for XXX board"
	@echo "       make fuses    - write fuse bits"
	@echo "Possible boards are b01, b02 and ac"
	@echo "See README for more info"

inst_ac: ac
	avrdude -p m8 -c pony-stk200 -P /dev/parport0 -U flash:w:${PRG}_ac.hex:i -v -E noreset

inst_%: %
	avrdude -p m32 -c pony-stk200 -P /dev/parport0 -U flash:w:${PRG}_$*.hex:i -v -E noreset

fuses:
	avrdude -p m32 -c pony-stk200 -P /dev/parport0 -U hfuse:w:0xCF:m -v -E noreset
	avrdude -p m32 -c pony-stk200 -P /dev/parport0 -U lfuse:w:0xFF:m -v -E noreset


b01: ${PRG}_b01.lst text_${PRG}_b01

b02: ${PRG}_b02.lst text_${PRG}_b02

ac:  ${PRG}_ac.lst text_${PRG}_ac

xarias_b01.elf: ${OBJ_B01}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

xarias_b02.elf: ${OBJ_B02}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

xarias_ac.elf: ${OBJ_AC}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

	

clean:
	rm -rf *.o *.elf *.eps *.png *.bak *.hex
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)


%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text_%: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $*.hex
#	$(OBJCOPY) -j .text -j .data -O srec $< $*.srec
	$(OBJCOPY) -j .text -j .data -O binary $< $*.bin
	@echo -n "Size of the $*.bin: "
	@ls -l $*.bin | cut -d ' ' -f 5 -


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

