
# define the software target and object filenames.
TARGETS=backspin
OBJECTS=backspin.o

# include all software-global make rules.
include ../global.mk

# define a rule to check for required external packages.
packages: sndfile.pc gtk+-3.0.pc gdk-3.0.pc

# add in sndfile compilation flags.
CFLAGS+= $(call pkg_cflags,sndfile) -DHAVE_SNDFILE
LDFLAGS+= $(call pkg_libs,sndfile)

# add in gtk3 compilation flags.
CFLAGS+= $(call pkg_cflags,gtk+-3.0)
LDFLAGS+= $(call pkg_libs,gtk+-3.0)

# add in gdk3 compilation flags.
CFLAGS+= $(call pkg_cflags,gdk-3.0)
LDFLAGS+= $(call pkg_libs,gdk-3.0)

# define a rule to install all command-line tools into the path.
install:
	@for target in $(TARGETS); do \
	 echo " INSTALL $$target"; \
	 $(INSTALL) $$target $(BINDIR)/; \
	 done
