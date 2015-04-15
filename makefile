CXX = g++
CXXFLAGS += -mavx -std=c++11 -pedantic -O3 -Wall -Wno-unused-function -Wextra -march=native -mtune=native -fopenmp

union: union.o
	$(CXX) $(CXXFLAGS) union.o -o union.exe

union.o: union.cc
	$(CXX) $(CXXFLAGS) -c union.cc

clean:
	rm *.exe *.o
