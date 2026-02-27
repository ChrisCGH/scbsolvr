CXX=clang++
OPT=-O3
GTK_CFLAGS=$(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS=$(shell pkg-config --libs gtk+-3.0)

all: mono scbsolvr

mono.o: mono.cxx mono_solver.h mono.h scorer.h top.h fixed.h callback.h crib.h
	$(CXX) -g -c $(OPT) mono.cxx

hash.o: hash.cxx hash.h
	$(CXX) -g -c $(OPT) hash.cxx

scorer.o: scorer.cxx hashtable.h N_Graph.h scorer.h
	$(CXX) -g -c $(OPT) scorer.cxx

mono: mono.o hash.o scorer.o
	$(CXX) -o mono -g $(OPT) mono.o hash.o scorer.o

MainWindow.o: MainWindow.cpp MainWindow.h mono_solver.h fixed.h crib.h callback.h \
              NgraphDialog.h FixKeyDialog.h CribDialog.h
	$(CXX) -g -c $(OPT) $(GTK_CFLAGS) MainWindow.cpp

NgraphDialog.o: NgraphDialog.cpp NgraphDialog.h ngraph.h callback.h
	$(CXX) -g -c $(OPT) $(GTK_CFLAGS) NgraphDialog.cpp

FixKeyDialog.o: FixKeyDialog.cpp FixKeyDialog.h fixed.h
	$(CXX) -g -c $(OPT) $(GTK_CFLAGS) FixKeyDialog.cpp

CribDialog.o: CribDialog.cpp CribDialog.h crib.h mono_solver.h
	$(CXX) -g -c $(OPT) $(GTK_CFLAGS) CribDialog.cpp

main_gtk.o: main_gtk.cpp MainWindow.h
	$(CXX) -g -c $(OPT) $(GTK_CFLAGS) main_gtk.cpp

scbsolvr: main_gtk.o MainWindow.o NgraphDialog.o FixKeyDialog.o CribDialog.o \
          hash.o scorer.o
	$(CXX) -o scbsolvr -g $(OPT) main_gtk.o MainWindow.o NgraphDialog.o \
	    FixKeyDialog.o CribDialog.o hash.o scorer.o \
	    $(GTK_LIBS) -lpthread

TESTS=tests/test_hash tests/test_mono tests/test_fixed tests/test_crib \
      tests/test_mono_solver

tests/test_hash: tests/test_hash.cxx hash.o tests/test.h
	$(CXX) -g $(OPT) -I. -o $@ tests/test_hash.cxx hash.o

tests/test_mono: tests/test_mono.cxx tests/test.h
	$(CXX) -g $(OPT) -I. -o $@ tests/test_mono.cxx

tests/test_fixed: tests/test_fixed.cxx tests/test.h
	$(CXX) -g $(OPT) -I. -o $@ tests/test_fixed.cxx

tests/test_crib: tests/test_crib.cxx tests/test.h
	$(CXX) -g $(OPT) -I. -o $@ tests/test_crib.cxx

tests/test_mono_solver: tests/test_mono_solver.cxx hash.o tests/test.h \
                        mono_solver.h
	$(CXX) -g $(OPT) -I. -o $@ tests/test_mono_solver.cxx hash.o

test: $(TESTS)
	@for t in $(TESTS); do \
		echo "Running $$t ..."; \
		./$$t || exit 1; \
	done
	@echo "All tests passed!"

clean:
	rm -f *.o mono scbsolvr $(TESTS)
