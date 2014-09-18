# PyPPM

An open source Earth's Field NMR Spectrometer.

## Introduction

The PyPPM project includes a set of hardware and software designs that enable
anyone to perform [Nuclear Magnetic Resonance] (
http://en.wikipedia.org/wiki/Nuclear_magnetic_resonance) experiments using
the Earth's magnetic field.

There are two principal hardware designs in the PyPPM project. The first,
PyPPMv1, is a [Proton Precession Magnetometer] (
http://en.wikipedia.org/wiki/Proton_magnetometer) that uses the
[non-adiabatic] (http://en.wikipedia.org/wiki/Adiabatic_theorem) method of
Russell Varian and Martin Packard to observe NMR signals at Earth's field:

> M. Packard, R. Varian, _Free Nuclear Induction in the Earth's Magnetic
> Field_, Physical Review, 1954, 93(4): 941.

The second hardware design, PyPPMv2, is more appropriately termed an [Earth's
Field NMR Spectrometer] (http://en.wikipedia.org/wiki/Earth's_field_NMR), as
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
FIXME

### Firmware
FIXME

### Footprints
FIXME

### Models
FIXME

### Notes

The `notes` directory holds a few random jots of information that don't really
fit into any of the other subdirectories, like recommended color schemes for
Gerber rendering, _etc_.

### Scripts
FIXME

### Simulations
FIXME

### Software
FIXME

### Symbols
FIXME

## Licensing

All software in the PyPPM project is released under the [GNU GPL 3.0] (
http://www.gnu.org/copyleft/gpl.html), and all hardware designs (PCB and case
designs) are released under the [CC-BY-SA 2.0] (
https://creativecommons.org/licenses/by-sa/2.0/).

Enjoy!

*~ Brad.*

