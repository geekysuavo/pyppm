# PyPPM
## An open source Earth's Field NMR Spectrometer

### Introduction

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
> Field_, Physical Review Letters, 1954, 93(4): 941.

The second hardware design, PyPPMv2, is more appropriately termed an [Earth's
Field NMR Spectrometer] (http://en.wikipedia.org/wiki/Earth's_field_NMR), as
it adds the adiabatic polarization features required to perform more complex
NMR experiments.

*Note:* At the moment, only PyPPMv1 has been verified to function at spec.

### 
