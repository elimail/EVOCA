# EVOCA
EVOCA tuning method source and example files. EVOCA was proposed in 10.1109/CEC.2013.6557972.

Source files:
1) globales.h: Main structure definitions
2) globales.cpp: Main structure procedures
3) includes.h: Includes
4) main.cpp: Main functions.
5) Makefile: Implements make clean and make exe commands to test EVOCA.
Hyperparameters of Evoca are listed in Makefile, such as the target algorithm (exe), the budget of resources (maximum number of executions of the target algorithm, maxEvals), seed, the maximum population size (maxM), the parameter configuration file (conf_file), the details of the instances used for the tuning process (inst_file), the number of pairs instance-seed to compute the quality of a configuration (nSeeds), a flag to determine if EVOCA will minimize (cost) or maximize (benefit) the output of the target algorithm (minimize), and a file containing candidate configurations that can be considered in the initialization of EVOCA (cand_file).

Example files:
a) AK.config: EVOCA configuration file contains the details of the number of parameters that will be tuned, their names, lower and upper bounds, and the precision that will be considered (value 1 means integer/categorical). The last line specifies the maximum precision that EVOCA can consider when generating new values for configurations. The example included aims to tune five ACO algorithm parameters: alpha, beta, rho, ph_max, and TotalAnts. All parameters but TotalAnts are real-valued parameters that can take values between 0.1 and 10.0 (1/10 - 100/10). This precision level is considered for the initialization process. Maximum precision level considers two decimal positions (10² = 100). TotalAnts is an integer-valued parameter that can take values between 2 and 50. 
b) AK.sh: EVOCA translator with the ACO algorithm to tune. In its last lines, AK.sh executes the target algorithm. The execution line is commented and replaced with a random number to enable execution of the EVOCA example.
c) instances_file: EVOCA list of instances to tune.
d) training/: directory containing instances to tune.
