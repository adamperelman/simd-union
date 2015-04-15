CXX = clang++
CXXFLAGS += -mavx -std=c++11 -pedantic -O0 -Wall -Wno-unused-function -Wextra -march=native -mtune=native

union: union.o
	$(CXX) $(CXXFLAGS) union.o -o union.exe

union.o: union.cc
	$(CXX) $(CXXFLAGS) -c union.cc

clean:
	rm *.exe *.o
