# pyppm/footprints

All custom [gEDA PCB](http://pcb.geda-project.org/) device footprints are
placed in subdirectories here.

### Footprint files

All files in this directory ending with `.fp` are footprint files. When they
are installed into the `PCB` main directory, they will be stripped of this
extension.

For example, `DPAK.fp` may be referenced in schematics as `DPAK`, and will live
in `/path/to/pcb/newlib/sockets/DPAK`.

Yes, all footprints are installed into the `newlib/sockets` directory, because
I'm a bit lazy.

### Installation script

To install the custom footprints into your `PCB` main directory, first edit
the `install` file. Make sure the path given on this line:

> `PCBDIR=/usr/share/pcb`

is correct. Then you can simply run the install script:

> `./install`

