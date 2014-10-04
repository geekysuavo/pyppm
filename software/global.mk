
# define the path in which we may install binaries.
DESTDIR=/usr/local
BINDIR=$(DESTDIR)/bin

# define default binary names and flags for commonly used executables.
CC=gcc
RM=rm -f
WC=wc -l

# define the installation executable statement.
INSTALL=install -m 755 -o root -g root

# define default compilation and linking flags.
CFLAGS=-Wall -I../core -fPIC
LDFLAGS=-lm

# define the core 'library' objects used in all compiled executables.
COREOBJ=ppm.o ppm-tune.o ppm-prog.o ppm-hw.o ppm-math.o ppm-data.o ppm-fall.o

# add the core objects onto the list of target objects.
OBJECTS+= $(COREOBJ)

# define the filename suffixes to be interpreted as such by 'make'.
.SUFFIXES: .c .o .pc

# == define functions for managing packages ==

# define a function to read package CFLAGS from pkg-config.
pkg_cflags=$(shell pkg-config --cflags $(1))

# define a function to read package LDFLAGS from pkg-config.
pkg_libs=$(shell pkg-config --libs $(1))

# == define targets for building all output files ==

# define the default make target when no targets are explicitly specified.
.DEFAULT_GOAL:=all

# define all non-file ("phony") make targets.
.PHONY: all packages clean again linecount fixme

# define the default make target. builds all binary software targets.
all: packages $(OBJECTS) $(TARGETS)

%.pc:
	@pkg-config --exists $(basename $@) || \
	 (echo "Required package $(basename $@) is not installed"; exit 1)

# define a target to build all required core object files.
$(COREOBJ):
	@echo " CC $(addsuffix .c,$(basename $@))"
	@$(CC) $(CFLAGS) -c ../core/$(addsuffix .c,$(basename $@)) -o $@

# define a rule to build all target binary executables.
$(TARGETS): $(OBJECTS)
	@echo " LD $@"
	@$(CC) $(LDFLAGS) $(COREOBJ) $(addsuffix .o,$@) -o $@

# define the default target for building objects from source files.
.c.o:
	@echo " CC $^"
	@$(CC) $(CFLAGS) -c $^ -o $@

# define a rule for cleaning any previously built files.
clean:
	@echo " CLEAN"
	@$(RM) $(TARGETS) $(OBJECTS) $(COREOBJ) pp fid spect wf fid.wav

# define a rule to quickly rebuild all target files.
again: clean all

# define a rule to run a line count of all source code.
linecount:
	@echo " WC"
	@$(WC) *.[ch]

# define a rule to search all source code for fixme statements.
fixme:
	@echo
	@echo ' * * * FIXME statements * * *'
	@grep -RHni fixme *.[ch] || echo "None found"
	@echo

