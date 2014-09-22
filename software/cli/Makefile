
# define the software targets and object filenames.
TARGETS=rst wpp rpp zg ver ft hrft wfp snd
OBJECTS=$(addsuffix .o,$(TARGETS))

# include all software-global make rules.
include ../global.mk

# define a rule to check for required external packages.
packages: sndfile.pc

# add in sndfile compilation flags.
CFLAGS+= $(call pkg_cflags,sndfile) -DHAVE_SNDFILE
LDFLAGS+= $(call pkg_libs,sndfile)

# add in verbosity compilation flags.
CFLAGS+= -DPPM_CORE_VERBOSE

# define a rule to install all command-line tools into the path.
install:
	@for target in $(TARGETS); do \
	 echo " INSTALL $$target"; \
	 $(INSTALL) $$target $(BINDIR)/; \
	 done
