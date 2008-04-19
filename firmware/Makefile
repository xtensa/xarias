PRG            = xarias
OBJ_B01        = xarias_b01.o hd44780.o lcd.o 
OBJ_B02        = xarias_b02.o s6b0108.o gLCD.o twi_devs.o 
MCU_TARGET     = atmega32
OPTIMIZE       = -Os
F_CPU          = 16000000UL

DEFS           =
LIBS           =

# You should not have to change anything below here.

CC             = avr-gcc 

CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -D "F_CPU=$(F_CPU)"
LDFLAGS       = -Wl,-Map,$(PRG).map -L /usr/lib/binutils/avr/2.16.1/ 

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all:
	@echo "Use either \"make b01\" or \"make b02\" ."

inst_b02: b02
	avrdude -p m32 -c pony-stk200 -P /dev/parport0 -U flash:w:xarias_b02.hex:i -v -E noreset

fuses:
	avrdude -p m32 -c pony-stk200 -P /dev/parport0 -U hfuse:w:0xCF:m -v -E noreset
	avrdude -p m32 -c pony-stk200 -P /dev/parport0 -U lfuse:w:0xFF:m -v -E noreset


b01: $(PRG)_b01.elf lst_b01 text_b01
#all: $(PRG).elf lst text eeprom


b02: $(PRG)_b02.elf lst_b02 text_b02

$(PRG)_b01.elf: $(OBJ_B01)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(PRG)_b02.elf: $(OBJ_B02)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak *.bin
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

lst_b01:  $(PRG)_b01.lst

lst_b02:  $(PRG)_b02.lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text_b01: hex_b01 
#text: hex bin srec

text_b02: hex_b02 bin_b02

hex_b01:  $(PRG)_b01.hex
#bin:  $(PRG).bin
#srec: $(PRG).srec

hex_b02:  $(PRG)_b02.hex
bin_b02:  $(PRG)_b02.bin

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

#%.srec: %.elf
#	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

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

