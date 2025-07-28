# EVOCA
EVOCA tuning method source and example files. EVOCA was proposed in 10.1109/CEC.2013.6557972.

Source files:
1) globales.h: Main structure definitions
2) globales.cpp: Main structure procedures
3) includes.h: Includes
4) main.cpp: Main functions.
5) Makefile: Implements make clean and make exe commands to test EVOCA.

Example files:
a) AK.config: EVOCA configuration file to tune five ACO algorithm parameters: alpha, beta, rho, ph_max, and TotalAnts.
b) AK.sh: EVOCA translator with the ACO algorithm to tune. In its last lines, AK.sh executes the target algorithm. The execution line is commented and replaced with a random number to enable execution of the EVOCA example.
c) instances_file: EVOCA list of instances to tune.
d) training/: directory containing instances to tune.
