
# define the python major version to use for distutils compilation.
PYVER=2
PY=python$(PYVER)

# define the software target and object filenames.
TARGETS=
OBJECTS=

# include all software-global make rules.
include ../global.mk

# redefine the default make target to include a distutils step.
all: build

# redefine the clean target to include a distutils cleaning.
clean: distutils-clean

# define a rule to clean the distutils tree.
distutils-clean:
	@echo " DISTCLEAN"
	@rm -rf build

# define a rule to check for required external packages.
packages:

# define the rule to build all python code.
build: $(OBJECTS)
	@echo " DISTUTILS"
	@$(PY) setup.py --quiet build

# define the rule to install the python module.
install:
	@echo " INSTALL"
	@$(PY) setup.py --quiet install

# define the rule to enter into a python sandbox.
sandbox: build
	@cd build/lib.* && $(PY); cd ../..
