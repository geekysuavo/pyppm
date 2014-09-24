# pyppm/software/pulprogs

This directory contains example pulse programs that can be used in `pyppm`
Python scripts.

## Introduction

All experiments performed on the PyPPM hardware are written in the language of
'pulse programs'. Because it's tricky to write a pulse program, and because
it's equally easy to write one that can potentially damage the PyPPM hardware,
I've started the (long) process of writing a small set of predefined pulse
programs for general use.

Each pulse program asks a different 'question' of the spin system, _i.e._ about
T1/T2 relaxation properties or connectivities. The information gained, and a
short overview of how the program works, will be attached to each pulse
program.

To use any of these pulse programs, you just need to import their Python code
and run the `pp` method. For example:

```python
import ppNonAdiabatic
dev.pulprog = ppNonAdiabatic.pp()
```

