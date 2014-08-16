
PROJ=dds
OBJECTS=dds.o main.o

MCU=atmega32u2
F_CPU=16000000
F_CLK=$(F_CPU)

CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
REMOVE=rm -f
DFU=dfu-programmer

DEFS=-DF_CPU=$(F_CPU)UL -DF_CLK=$(F_CLK)UL
CFLAGS=-mmcu=$(MCU) -Wall -Wundef -Os $(DEFS)
ASFLAGS=-mmcu=$(MCU) $(DEFS)
LDFLAGS=

HEXFLAGS=-R .eeprom -R .fuse -R .lock
FORMAT=ihex

EEPFLAGS=-j .eeprom
EEPFLAGS+= --set-section-flags=.eeprom="alloc,load"
EEPFLAGS+= --change-section-lma .eeprom=0 --no-change-warnings

.SUFFIXES: .c .o .S

all: $(OBJECTS) $(PROJ).elf $(PROJ).hex $(PROJ).eep $(PROJ).lss

.c.o:
	@echo " CC $<"
	@$(CC) -c $(CFLAGS) $< -o $@

.S.o:
	@echo " AS $<"
	@$(CC) -c $(ASFLAGS) $< -o $@

%.elf: $(OBJECTS)
	@echo " LD $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.hex: %.elf
	@echo " HEX $@"
	@$(OBJCOPY) -O $(FORMAT) $(HEXFLAGS) $< $@

%.eep: %.elf
	@echo " EEP $@"
	@$(OBJCOPY) $(EEPFLAGS) -O $(FORMAT) $< $@ || exit 0

%.lss: %.elf
	@echo " LSS $@"
	@$(OBJDUMP) -h -S -z $< > $@

size: $(PROJ).elf
	@echo ""
	@$(SIZE) -C --mcu=$(MCU) $(PROJ).elf

clean:
	@echo " CLEAN"
	@$(REMOVE) $(PROJ).elf $(PROJ).hex
	@$(REMOVE) $(PROJ).eep $(PROJ).lss
	@$(REMOVE) $(OBJECTS)

dfu: dfu-erase dfu-flash dfu-eeprom dfu-reset

dfu-erase:
	@echo " DFU erase"
	@$(DFU) $(MCU) erase

dfu-flash: $(PROJ).hex
	@echo " DFU flash $(PROJ).hex"
	@$(DFU) $(MCU) flash $(PROJ).hex

dfu-eeprom: $(PROJ).eep
	@echo " DFU eeprom $(PROJ).eep"
	@$(DFU) $(MCU) flash-eeprom $(PROJ).eep

dfu-reset:
	@echo " DFU reset"
	@$(DFU) $(MCU) reset

again: clean all

linecount:
	@echo " WC"
	@wc -l *.[ch]

fixme:
	@echo
	@echo ' * * * FIXME statements: * * *'
	@grep -RHni fixme *.[ch] || echo "None found"
	@echo
	@echo ' * * * NOTE statements: * * *'
	@grep -RHni note *.[ch] || echo "None found"
	@echo

