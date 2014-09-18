# pyppm/scripts

A few operations get performed fairly regularly when editing PyPPM source
files. To streamline the process as much as possible, I wrote a few support
scripts.

Here's a quick rundown of the support scripts, and what they do.

### board

The `board` script converts all schematic files in the current working
directory into a new empty printed circuit board design file set. This should
only be run when creating a new board, because it _completely destroys_ any
existing board designs in the current directory.

### bom

The `bom` script takes bill-of-materials CSV files and outputs a list of all
unique part numbers and their occurrence count. This is useful when ordering
parts for a design from Digi-Key, because any given part number may occur more
than once in different schematic sections.

### build

The `build` script runs any simulations in the current working directory,
generating any plot files that can be made. This is one of the uglier, less
flexible hacks in the `scripts` directory. You've been warned.

### clean

The `clean` script undoes the results of the `build` script.

### edit

The `edit` script is used to rapidly open either the schematics or the printed
circuit board of a given hardware design. It also simplifies the process of
purging temporary scratch files and making gerbers and postscripts.

### effbw

The `effbw` (Octave) script is used to compute effective bandwidth when given
a text file containing frequencies in column one and gains in column two.

### layer.awk

The `layer.awk` script a small script used by scripts in the `designs`
directory to locate the boundaries of a given layer in a PCB source file.

### print

The `print` script prints all schematics in the current working directory.

