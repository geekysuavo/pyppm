
# define all phony (non-file) target names.
.PHONY: all clean again

# define the default target.
all: napkin

# define the compilation and linkage rule.
napkin: napkin.c
	@echo " CC napkin.c"
	@gcc -lm -o napkin napkin.c

# define the rule for cleaning the source tree.
clean:
	@echo " CLEAN"
	@rm -f napkin

# define the rule for rebuilding the source tree.
again: clean all

# define the rule to build and run the example input file.
test: all coil-1.0.txt
	@echo " TEST"
	@./napkin < coil-2.0.txt

