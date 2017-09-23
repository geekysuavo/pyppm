# pyppm/symbols

All custom [gEDA gschem](http://www.geda-project.org/) device schematic
symbols are placed in subdirectories here.

### Symbol files

All subdirectories of this location are interpreted to correspond one-to-one
with the subdirectories found in `/path/to/gEDA/sym`. Files in these
subdirectories will be installed into their respective locations in
that tree. For example, `linear/lt3015-1.sym` will be installed to
`/path/to/gEDA/sym/linear/lt3015-1.sym`.

### Installation script

To install the custom symbols into your `gschem` main directory, first edit
the `install` file. Make sure the path given on this line:

> `GEDADIR=/usr/share/gEDA`

is correct. Then you can simply run the install script:

> `./install`

