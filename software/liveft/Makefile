
# define the software target and object filenames.
TARGETS=liveft
OBJECTS=liveft.o

# include all software-global make rules.
include ../global.mk

# define a rule to check for required external packages.
packages:

# add in opengl compilation flags.
CFLAGS+= 
LDFLAGS+= -lglut -lGL

# define a rule to install all command-line tools into the path.
install:
	@for target in $(TARGETS); do \
	 echo " INSTALL $$target"; \
	 $(INSTALL) $$target $(BINDIR)/; \
	 done
