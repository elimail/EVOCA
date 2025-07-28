maxM=20
maxEvals=5000
seed=0
flags=-Wno-deprecated -O3 -g -ggdb
prog=EVOCA
comp=g++
conf_file=AK.config
inst_file=instances_file
exe=AK.sh
nSeeds=3
maxTime=0
minimize=1 #1=minimization, 0=maximization
cand_file=candidates.config #This is not mandatory

$(prog): main.o globales.o
	$(comp) globales.o main.o -o $(prog) $(flags)
	
globales.o: globales.cpp includes.h globales.h 
	$(comp) -c globales.cpp $(flags)	
	
main.o: main.cpp includes.h globales.h 
	$(comp) -c main.cpp $(flags)

clean:
	rm -f *~
	rm -f *.o
	rm -f EVOCA EVOCA.log resultados.res

exe:$(prog)
	time ./$(prog) $(exe) $(conf_file) $(inst_file) $(seed) $(nSeeds) $(maxM) $(maxEvals) $(maxTime) $(minimize) $(cand_file)
