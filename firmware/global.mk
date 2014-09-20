# Makefile: global compilation and programming parameters for all firmware
# ========================================================================

# General parameters
# ---------------------------------------------------------------------
#  MCU: target microcontroller
#  F_CPU: system clock frequency, in hertz
#  F_CLK: external oscillator frequency, in hertz
MCU=atmega32u2
F_CPU=16000000
F_CLK=$(F_CPU)

# External binary filenames
# ---------------------------------------------------------------------
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
REMOVE=rm -f
DFU=dfu-programmer

# C Defines
# ---------------------------------------------------------------------
DEFS=-DF_CPU=$(F_CPU)UL -DF_CLK=$(F_CLK)UL
DEFS+= -DPPM_FW_VER_MAJOR=$(VER)
DEFS+= -DPPM_FW_VER_MINOR=$(REV)

# Compiler flags
# ---------------------------------------------------------------------
CFLAGS=-mmcu=$(MCU) -Wall -Wundef -Os $(DEFS)
ASFLAGS=-mmcu=$(MCU) $(DEFS)

# Linker flags
# ---------------------------------------------------------------------
LDFLAGS=

# Hex file creation flags
# ---------------------------------------------------------------------
HEXFLAGS=-R .eeprom -R .fuse -R .lock

# EEPROM file creation flags
# ---------------------------------------------------------------------
EEPFLAGS=-j .eeprom
EEPFLAGS+= --set-section-flags=.eeprom="alloc,load"
EEPFLAGS+= --change-section-lma .eeprom=0 --no-change-warnings

# Flash and EEPROM file format
# ---------------------------------------------------------------------
FORMAT=ihex

# Core object files for use in every firmware build.
# ---------------------------------------------------------------------
#COREOBJ=clk.o usb.o
OBJECTS+= $(COREOBJ)

# Makefile rules and targets begin below...
# ---------------------------------------------------------------------

# define the filename suffixes to be interpreted as such by 'make'.
.SUFFIXES: .c .o .S

# Default makefile target to build all targets
all: $(OBJECTS) $(PROJ).elf $(PROJ).hex $(PROJ).eep $(PROJ).lss

# Compile a core object file from a C source file.
$(COREOBJ):
	@echo " CC $(addsuffix .c,$(basename $@))"
	@$(CC) -c $(CFLAGS) ../lib/$(addsuffix .c,$(basename $@)) -o $@

# Compile an object file from a C source file
.c.o:
	@echo " CC $<"
	@$(CC) -c $(CFLAGS) $< -o $@

# Compile an object file from an assembly source file.
.S.o:
	@echo " AS $<"
	@$(CC) -c $(ASFLAGS) $< -o $@

# Link an ELF binary file from all compiled object files
%.elf: $(OBJECTS)
	@echo " LD $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Create a flash image from the linked ELF binary
%.hex: %.elf
	@echo " HEX $@"
	@$(OBJCOPY) -O $(FORMAT) $(HEXFLAGS) $< $@

# Create a hex EEPROM image from the linked ELF binary
%.eep: %.elf
	@echo " EEP $@"
	@$(OBJCOPY) $(EEPFLAGS) -O $(FORMAT) $< $@ || exit 0

# Create an extended listing file from the linked ELF binary
%.lss: %.elf
	@echo " LSS $@"
	@$(OBJDUMP) -h -S -z $< > $@

# Print the size of the linked ELF binary
size: $(PROJ).elf
	@echo ""
	@$(SIZE) -C --mcu=$(MCU) $(PROJ).elf

# Clean all built object and target files
clean:
	@echo " CLEAN"
	@$(REMOVE) $(PROJ).elf $(PROJ).hex
	@$(REMOVE) $(PROJ).eep $(PROJ).lss
	@$(REMOVE) $(OBJECTS)

# Program the built hex flash image to the device via DFU
dfu: dfu-erase dfu-flash dfu-eeprom dfu-reset

# Erase the device via DFU
dfu-erase:
	@echo " DFU erase"
	@$(DFU) $(MCU) erase

# Flash the device via DFU
dfu-flash: $(PROJ).hex
	@echo " DFU flash $(PROJ).hex"
	@$(DFU) $(MCU) flash $(PROJ).hex

# Flash the device EEPROM via DFU
dfu-eeprom: $(PROJ).eep
	@echo " DFU eeprom $(PROJ).eep"
	@$(DFU) $(MCU) flash-eeprom $(PROJ).eep

# Reset the device via DFU
dfu-reset:
	@echo " DFU reset"
	@$(DFU) $(MCU) reset

# Rebuild all objects and targets
again: clean all

# count source code lines
linecount:
	@echo " WC"
	@wc -l *.[chS]

# count firmware bytes.
sz: $(PROJ).hex
	@echo " SZ"
	@python3 -c \
	 "print(' {} {}'.format(sum([int('0x' + ll[1:3], \
	  16) for ll in open('$(PROJ).hex').readlines()]), '$(PROJ).hex'))"

# search for statements regarding necessary changes
fixme:
	@echo
	@echo ' * * * FIXME statements: * * *'
	@grep -RHni fixme *.[chS] || echo "None found"
	@echo
	@echo ' * * * NOTE statements: * * *'
	@grep -RHni note *.[chS] || echo "None found"
	@echo

