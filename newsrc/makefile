OPT=-O3
all: mono

mono.o: mono.cxx mono_solver.h mono.h scorer.h top.h fixed.h callback.h crib.h
	g++ -g -c $(OPT) mono.cxx

hash.o: hash.cxx hash.h
	g++ -g -c $(OPT) hash.cxx

scorer.o: scorer.cxx hashtable.h N_Graph.h scorer.h
	g++ -g -c $(OPT) scorer.cxx

mono: mono.o hash.o scorer.o
	g++ -o mono -g $(OPT) mono.o hash.o scorer.o -Wl,--stack,41943040

clean:
	rm -f *.o mono.exe
