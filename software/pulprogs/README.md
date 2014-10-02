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

* The dead time argument may be any positive value less than 1500
milliseconds.

### `pyppm.DELAY`

The 'delay' instruction tells the device to execute a precisely timed delay
on the order of one-half to sixty seconds. It takes a single argument, which
is the delay time in seconds:

```python
P = [... [pyppm.DELAY, 2.5] ...]
```

* The delay time argument may be any positive value less than 65 seconds.

### `pyppm.POLARIZE`

The 'polarize' instruction takes a single boolean argument, which states
whether to enable or disable the non-adiabatic polarization current sink:

```python
P = [... [pyppm.POLARIZE, True] ...]
```

* The polarization argument must be a boolean.

### `pyppm.RELAY`

The 'relay' instruction takes a single boolean argument, which states
whether to enable or disable the receive coil relay. When the relay is
enabled, the receive coil is connected to the non-adiabatic polarization
current sink. When the relay is disabled, the receive coil is connected
to the low-noise analog signal chain for acquisition:

```python
P = [... [pyppm.RELAY, False] ...]
```

* The relay argument must be a boolean.

### `pyppm.ACQUIRE`

The 'acquire' instruction tells the device to read a set number of digital
samples from the analog signal chain, and it takes two arguments. The first
argument to 'acquire' is an integer that holds the number of digital samples
to read, and should be a power of two. The second argument is the sampling
rate, in kilosamples per second:

```python
P = [... [pyppm.ACQUIRE, 65536, 22.05] ...]
```

* The sample count argument may be any positive integer less than around
four billion.
* The sample rate argument may be any positive value less than a value that
depends on the hardware platform:
  * PyPPM 1.x: 30 kS/s
  * PyPPM 2.x: 100 kS/s

### `pyppm.TX_RISE` and `pyppm.TX_FALL`

The 'tx-rise' and 'tx-fall' instructions tell the device to ramp the
high-current transmit coil output voltage up or down, respectively. The
instructions both take two arguments. The first argument is a float that
holds the total time of the edge, in milliseconds. The second argument is
a float that holds the final amplitude of the edge:

```python
P = [... [pyppm.TX_RISE, 250, -0.8] ...]
```

* The rise/fall time argument may be any positive value less than 10,000.
* The amplitude argument may be any value between -1 and +1.

### `pyppm.TX_PULSE`

The 'tx-pulse' instruction initiates a sinusoidal pulse on the high-current
transmit coil output channel. The instruction takes three arguments. The
first argument to 'tx-pulse' is a float that holds the total time the pulse
will be generated, in seconds. The second argument is a float that holds the
output frequency in Hz. The final argument is a floating-point scale factor
(range [0,1]) that is used to set the pulse amplitude.

Note that a scale factor of `0.0` does not indicate a zero-amplitude pulse,
but rather a pulse without any digital gain applied. A scale factor of `1.0`
corresponds to 256-fold digital voltage gain.

```python
P = [... [pyppm.TX_PULSE, 0.1, 2275.0, 0.8] ...]
```

* The pulse time argument may be any positive value less than 10,000.
* The pulse frequency may be any positive value less than 10,000.
* The pulse gain may be any value between 0 and +1.

### `pyppm.TUNE`

The 'tune' instruction instructs the device to change the currently set
tuning frequency in the capacitive switch bank. The instruction takes a
single argument that holds the desired tuning frequency, in Hertz. If a
frequency value of zero is passed, the tuning bank is switched off:

```python
P = [... [pyppm.TUNE, 1250] ...]
```

* The frequency argument may be any positive value less than 10,000.

### `pyppm.SHIM_X`, `pyppm.SHIM_Y` and `pyppm.SHIM_Z`

The 'shim-x', 'shim-y' and 'shim-z' instructions instruct the device to change
the currently set shim value, on the desired axis and provided the hardware
supports shimming along said axis. The instructions take a single value that
holds the shim output value:

```python
P = [... [pyppm.SHIM, -0.2] ...]
```

* The shim argument may be any value between -1 and +1.

### `pyppm.END`

The 'end' instruction signals immediate termination of pulse program execution
and requires no arguments:

```python
P = [... [pyppm.END]]
```

