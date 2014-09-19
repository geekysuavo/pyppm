# pyppm/software

This directory contains all the software required to use the PyPPM hardware.

Global compilation configuration information is stored in `global.mk` here,
and each subdirectory sources that file before defining new make rules.

### Core source code

A core set of functionality, located in `core`, has been written in the C
language to interface to the PyPPM via the character device (usually
`/dev/ttyACM0`) it creates on the host computer. All higher level code
(including the Python module) wraps this core C code.

*Note:* Depending on the ownership of the `/dev/ttyACM0` or `/dev/usbmodemXXX`
device file created by the operating system upon connection of the PyPPM to
the host computer, you may have to run programs that talk to the PyPPM as
root, _e.g.:_ `./liveft` would become `sudo ./liveft`, _etc_. This goes for
any Python programs that use the `pyppm` module too.

### Command-line utilities

The `cli` directory contains small command-line utilities that perform very
small chunks of work. Each one either has a short conversation with the
PyPPM hardware or does a small piece of data processing on acquired
signals.

As a general rule of thumb, utilities that have `h` suffixed onto them are the
human-readable versions of their simpler utilities. For example, while `rpar`
reads the current parameters from a PyPPMv1 device, `rparh` will read the
same parameters and output them in human-readable format.

These `cli` utilities are meant mainly for the purposes of debugging. If you
want to use the PyPPM for anything useful, the Python module is recommended.

A good test of the PyPPM and the `cli` utilities is to build them and run
(with the PyPPM connected) a few short commands:

```
make
./rparh
./zg
./ft
```

These commands should connect to the PyPPM, read the current set of parameters,
run a single acquisition, and Fourier transform the result. The time-domain
results will be in a file called `fid` and the frequency-domain results will
be in `spect`.

### Graphical device interface

The `gui` directory contains source code for the `backspin` graphical interface
to the PyPPM hardware. While `backspin` is technically a GUI application, it
is not super-friendly, and relies upon a set of text commands for operating
the device. Again, the Python module is recommended.

To use `backspin`, just build it and run (with a PyPPM attached):

> `make && ./backspin`

### Live acquisition view

The `liveft` directory contains source code for a small OpenGL application that
continuously acquires short data streams from the PyPPM and displays an FFT'ed
representation to the screen, with a bit of signal averaging. This program is
really just for debugging the hardware analog signal chain.

To use `liveft`, just build it and run (with a PyPPM attached):

> `make && ./liveft`

### Signal-to-noise calculator

The `napkin` directory contains a small C program that may be used to perform
a battery of calculations that predict experimental results (noise level,
signal level, coil resistance, inductance, and quality, _etc_) from a few
input values.

Example input values may be found in `napkin/coil-2.0.txt`. To compute the
results, just build and run like so:

> `make && ./napkin < coil-2.0.txt`

### Free Induction Decay solvers

The `solver` directory holds a few GNU Octave scripts that may be used to
solve for the resulting Free Induction Decay (FID) signal given a set of
polarization properties. I wrote these scripts to illustrate the effect of
long polarization turn-off times on FID intensity, a demonstration of the
[adiabatic theorem] (http://en.wikipedia.org/wiki/Adiabatic_theorem).

To run the solvers, just build:

> `make`

### Python modules

The Python modules in `py` are really where it's at. For more information on
how to build, install and use the Python modules, read `py/README.md`.

