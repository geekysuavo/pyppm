
# define the default rule to compute all calculations.
all:
	octave --silent sol.m
	octave --silent multisol.m

# define a rule to clean all computed results.
clean:
	@echo " CLEAN"
	@rm -f sol-????????.txt
	@rm -f multisol-????????.txt
	@rm -f multisol-parms-????????.txt
