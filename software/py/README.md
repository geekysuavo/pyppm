# pyppm/software/py

This directory contains the `pyppm` Python module source code.

## Introduction

The `pyppm` module links the PyPPM hardware and its capabilities directly
into the powerful Python language, allowing almost any use imaginable of
PPM/EFNMR data acquired from the device. How you use `pyppm` is completely
up to your own imagination! **:)**

## Compilation & Installation

To build `pyppm`, simply run `make`:

> `make`

Easy, right? If you get an error about Python not being found or if you want
to compile the module for a different Python version than the default, you'll
have to specify the Python version during compilation, _e.g._:

> `make PYVER=3.2`

To install, just run it as the super user and tack on an `install` command:

> `sudo make install`

or, for different Python versions:

> `sudo make PYVER=3.2 install`

The `pyppm` Python module will now be installed onto your system, and may
be imported just like any other module. Hooray!

## Example usage

To use `pyppm` in your Python programs, you'll first need to import the
module:

```python
import pyppm
```

It'll probably be useful to have `numpy` and `matplotlib` available as well.
All the following code that uses those two modules assumes they've been
imported as follows:

```python
import numpy as np
import matplotlib.pyplot as plt
```

The `PPM` class in `pyppm` provides a direct connection to the PyPPM hardware
and may be instantiated like so:

```python
dev = pyppm.PPM()
```

On Linux, this assumes you're trying to connect to `/dev/ttyACM0`. On OSX, it
assumes `/dev/cu.usbmodemPPMv1r1`. If you have multiple attached PyPPM
devices, or if your device was assigned a different device filename than
the default, you can specify a filename:

```python
dev = pyppm.PPM(filename = '/dev/file')
```

If all goes well, you should have no errors and a new `dev` object to play
with. If the connected hardware is a PyPPMv1, you can read and/or modify the
acquisition parameters using the `parms` dictionary:

```python
par = dev.parms
par[pyppm.SCAN_COUNT] = 4
dev.parms = par
```

Note that direct modification of a single element in the dictionary does not
work. _e.g._:

```python
dev.parms[pyppm.SCAN_COUNT] = 4   # THIS DOES NOT WORK!
```

The way to modify one parameter at a time is using the `setparm` method:

```python
dev.setparm(pyppm.SCAN_COUNT, 4)
```

