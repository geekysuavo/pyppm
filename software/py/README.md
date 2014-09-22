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

Consider this section as a whirlwind tutorial to using the `pyppm` module in
your Python programs.

### Importing the module

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

### Connecting to the device

The `PPM` class in `pyppm` provides a direct connection to the PyPPM hardware
and may be instantiated like so:

```python
dev = pyppm.PPM()
```

On Linux, this assumes you're trying to connect to `/dev/ttyACM0`. On OSX,
it assumes `/dev/cu.usbmodemPyPPM`. If you have multiple attached PyPPM
devices, or if your device was assigned a different device filename than
the default, you can specify a filename:

```python
dev = pyppm.PPM(filename = '/dev/file')
```

If all goes well, you should have no errors and a new `dev` object to play
with. A simple test is to print the device firmware version:

```python
print(dev.version)
```

A proper response is a 2-tuple containing the major and minor version of the
attached device's firmware, _e.g._ `(1, 3)`.

### Acquisition parameters

If the connected hardware is a PyPPMv1, you can read and/or modify the
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

The PyPPMv1 hardware boots up with a sane set of default acquisition
parameters. Using these parameters will result in a one-second polarization
and 16,384 acquired data points at 20 kS/s. If you want to run without
polarization, the following two commands will turn it off on a PyPPMv1
device:

```python
dev.setparm(pyppm.POLARIZE_CURRENT, 0)
dev.setparm(pyppm.POLARIZE_TIME, 0)
```

### Acquiring data

Once the acquisition parameters are set as desired, an experiment may be
performed using the following (blocking) command:

```python
(t, a) = dev.acquire()
```

This command will block until the entire experiment is complete. If multiple
scans were set in the acquisition parameters, the program will block until
all scans are completed.

The acquisition will return two tuples, `t` and `a`. The first tuple holds
the time at which every data point was acquired, and the second holds the
voltage (or average voltage, in the case of multiple scans) of each data
point.

### Fourier transforming data

Acquired data may be Fourier transformed using the `fft` or `hrft` methods
in the `pyppm` module. While you may use any other external method, these
are known to output the proper results with correct frequency axes:

```python
(f, A) = pyppm.fft(t, a)
(fx, Ax) = pyppm.hrft(t, a, 950, 1050)
```

The `hrft` method computes a highly interpolated Fourier transform over a
short spectral sub-region:

> R. Hollos, _C Program Magnifies Spectrum When An FFT Can't Hack It_,
> Electronic Design, 2003.

In the above example, the HRFT was computed from 950 Hz to 1050 Hz, which
would be appropriate if a signal peak were idenfied near 1000 Hz in the FFT
spectrum and greater frequency resolution was desired.

It's important to note, however, that the HRFT requires fairly long-decaying
signals (and thus high SNR and high field homogeneity), as it windows the
time-domain data with a Blackman apodization function prior to computation.

One final transform is available in `pyppm`, called the [Short-time Fourier
transform] (http://en.wikipedia.org/wiki/Short-time_Fourier_transform). It
computes multiple FFT's over short time windows of the time-domain data,
effectively creating a time-course of the frequency content in the data:

```python
(tz, fz, Az) = pyppm.stft(t, a)
```

This may be useful in identifying the decay profile of a true signal in the
presence of steady-state interference.

### Digitally minimizing interference

The PyPPMv1 has no input frequency tuning, which means that the resulting
acquired data will contain significant interference, unless it was taken
on a desert island free of any other technology. One way to minimize this
interference is to "subtract out" any frequency content that is present when
polarization is on _and_ off. This should leave only signals that arose due
to the NMR experiment:

```python
def rmnoise(t, aon, aoff):
  (f, Aon) = pyppm.fft(t, aon)
  (f, Aoff) = pyppm.fft(t, aoff)

  X = np.array(Aon)
  Y = np.array(Aoff)

  alpha = sum(X * Y) / sum(Y * Y)
  A = tuple(X - alpha * Y)

  return (f, A)
```

The above method implements a minimization of the background noise in `Aoff`
that is present in `Aon`, in a least-squares sense. It can only return the
frequency-domain data, however, and not the time-domain result.

### A complete `pyppm` example

Here is an example of how to use the `pyppm` module from start to finish
on a connected PyPPMv1 device:

```python
# import the pyppm module.
import pyppm

# import numpy and matplotlib.
import numpy as np
import matplotlib.pyplot as plt

# connect to the device.
dev = pyppm.PPM()

# extend the polarization time.
dev.setparm(pyppm.POLARIZE_TIME, 10)

# acquire a scan with polarization.
(t, aon) = dev.acquire()

# turn off polarization.
dev.setparm(pyppm.POLARIZE_CURRENT, 0)
dev.setparm(pyppm.POLARIZE_TIME, 0)

# acquire a scan without polarization.
(t, aoff) = dev.acquire()

# Fourier transform the data.
(f, Aon) = pyppm.fft(t, aon)
(f, Aoff) = pyppm.fft(t, aoff)

# plot the data.
l, = plt.plot(np.array(f), np.array(Aon))
l, = plt.plot(np.array(f), np.array(Aoff))
plt.show()
```

