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
import pulprogs
dev.pulprog = pulprogs.hard.single()
```

Some pulse programs will have optional arguments. For example:

```python
import pulprogs
def.pulprog = pulprogs.misc.acquire(32768, 22.05)
```

## Pulse program syntax

Each pulse program instruction has a specific set of arguments that it requires
when used in a complete program. For example, delays and dead times require
time values to be passed along with the instruction. The following details
each instruction's required arguments.

### `pyppm.DEADTIME`

The 'deadtime' instruction tells the device to execute a short, imprecise
delay on the order of one to a few hundred milliseconds. It takes a single
argument, which is the delay time in milliseconds:

```python
P = [... [pyppm.DEADTIME, 15] ...]
```

### `pyppm.DELAY`

The 'delay' instruction tells the device to execute a precisely timed delay
on the order of one-half to sixty seconds. It takes a single argument, which
is the delay time in seconds:

```python
P = [... [pyppm.DELAY, 2.5] ...]
```

### `pyppm.POLARIZE`

The 'polarize' instruction takes a single boolean argument, which states
whether to enable or disable the non-adiabatic polarization current sink:

```python
P = [... [pyppm.POLARIZE, True] ...]
```

### `pyppm.RELAY`

The 'relay' instruction takes a single boolean argument, which states
whether to enable or disable the receive coil relay. When the relay is
enabled, the receive coil is connected to the non-adiabatic polarization
current sink. When the relay is disabled, the receive coil is connected
to the low-noise analog signal chain for acquisition:

```python
P = [... [pyppm.RELAY, False] ...]
```

### `pyppm.ACQUIRE`

The 'acquire' instruction tells the device to read a set number of digital
samples from the analog signal chain, and it takes two arguments. The first
argument to 'acquire' is an integer that holds the number of digital samples
to read, and should be a power of two. The second argument is the sampling
rate, in kilosamples per second:

```python
P = [... [pyppm.ACQUIRE, 65536, 22.05] ...]
```

### `pyppm.TX_RISE`

_FIXME_: This instruction has yet to be finalized.

### `pyppm.TX_FALL`

_FIXME_: This instruction has yet to be finalized.

### `pyppm.TX_PULSE`

_FIXME_: This instruction has yet to be finalized.

### `pyppm.TUNE`

_FIXME_: This instruction has yet to be finalized.

### `pyppm.SHIM_X`, `pyppm.SHIM_Y` and `pyppm.SHIM_Z`

_FIXME_: These instructions have yet to be finalized.

### `pyppm.END`

The 'end' instruction signals immediate termination of pulse program execution
and requires no arguments:

```python
P = [... [pyppm.END]]
```

