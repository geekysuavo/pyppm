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

