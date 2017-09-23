# pyppm/firmware

This directory contains all the firmware required to use the PyPPM hardware.

Global compilation configuration information is stored in `global.mk` here,
and each subdirectory sources that file before defining new make rules.

### Core source code

A core set of functionality, located in `lib`, has been written in the C
language to keep much of the base functions (clocking, USB, _etc_) in one
central location. Not all firmware source trees pull in core code from `lib`,
but all the `scb` and `ppm` firmware trees use at least some of that code.

### Building firmware

All firmware source trees follow the same pattern of compilation and flashing.
To compile the firmware using [AVR GCC](http://www.nongnu.org/avr-libc/), run
`make`:

> `make`

To flash the compiled firmware onto a device with
[dfu-programmer](http://dfu-programmer.github.io/), use the `dfu` target:

> `sudo make dfu`

Of course, you'll have to boot the target device into the DFU bootloader before
running the above command. To do that on PyPPM devices, just hold the HWB
pushbutton while toggling the RST pushbutton once.

### Direct Digital Synthesis (DDS) code

Several firmware trees were created during work with DDS sinewave generation,
and the results are kept here. Final DDS code used in the PyPPMv2 is slated
to be kept in the `ppm-X.Y` trees.

### PyPPM code

PyPPM boards in the `designs` directory correspond to firmware source trees in
this directory. In other words, the firmware in `firmware/ppm-1.3` is written
for the circuitry in `designs/ppm-1.3`.

