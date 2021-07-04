# PyPPM

An open source Earth's Field NMR Spectrometer.

## Introduction

The PyPPM project includes a set of hardware and software designs that enable
anyone to perform [Nuclear Magnetic Resonance](
http://en.wikipedia.org/wiki/Nuclear_magnetic_resonance) experiments using
the Earth's magnetic field.

There are two principal hardware designs in the PyPPM project. The first,
PyPPMv1, is a [Proton Precession Magnetometer](
http://en.wikipedia.org/wiki/Proton_magnetometer) that uses the
[non-adiabatic](http://en.wikipedia.org/wiki/Adiabatic_theorem) method of
Russell Varian and Martin Packard to observe NMR signals at Earth's field:

> M. Packard, R. Varian, _Free Nuclear Induction in the Earth's Magnetic
> Field_, Physical Review, 1954, 93(4): 941.

The second hardware design, PyPPMv2, is more appropriately termed an [Earth's
Field NMR Spectrometer](http://en.wikipedia.org/wiki/Earth's_field_NMR), as
it adds the adiabatic polarization features required to perform more complex
NMR experiments.

*Note:* At the moment, only PyPPMv1 has been verified to function at spec.

## Source files

This GitHub repository (or tarball, depending on how you downloaded the source
tree) contains all the files necessary for either reproducing or modifying the
PyPPM designs. The following sections break down exactly what you can expect
to find in each subdirectory of the repo. Each subdirectory also contains it's
own `README.md` file that provides further explication.

### Designs

The `designs` directory holds hardware (schematic and printed circuit board),
coil, and enclosure designs. Anything physically constructable is likely to
have design files living in this subdirectory.

### Firmware

The `firmware` directory contains all device firmware, both for PyPPM devices
and any other devices that use megaAVR microcontrollers.

### Footprints

The PyPPM project uses the [gEDA PCB](http://pcb.geda-project.org/) design
tool for all printed circuit boards. While the gEDA footprint library is
fairly extensive, a few custom device footprints had to be made to suit
the more exotic/modern parts that went into PyPPM boards.

### Models

The [models](models) directory holds all SPICE models used by simulations
of PyPPM circuit behavior. To see the actual simulation files, look in
[simulations](simulations).

### Notes

The [notes](notes) directory holds a few random jots of information that
don't really fit into any of the other subdirectories, like recommended
color schemes for Gerber rendering, _etc_. Documentation related to the
PyPPM's entry into The Hackaday Prize will also be placed here.

### Scripts

Miscellaneous support scripts that I wrote to make my own life easier during
PyPPM development are co-located in the [scripts](scripts) directory.

### Simulations

The [simulations](simulations) directory contains all SPICE simulations
I've made to analyze the behavior of subcircuit modules in PyPPM.

### Software

The [software](software) directory holds all software written during the
course of the PyPPM project's development. Anything from command-line
utilities to python modules to calculations of free induction decays
is here.

### Symbols

The [symbols](symbols) directory holds all custom [gEDA gschem](
http://www.geda-project.org/) schematic symbols used in the PyPPM
schematics.

## Licensing

All software in the PyPPM project is released under the [GNU GPL 3.0](
http://www.gnu.org/copyleft/gpl.html), and all hardware designs (PCB and case
designs) are released under the [CC-BY-SA 2.0](
https://creativecommons.org/licenses/by-sa/2.0/).
