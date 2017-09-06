CXX=g++
WARN=-Wall -Werror -ansi
CXXFLAGS=-std=c++11 -O2 -mavx2 -fopenmp

PROGRAM=hybrid_quicksort mergesort

all: $(PROGRAM)

hybrid_quicksort: hybrid_quicksort.cpp
	$(CXX) $(WARN) $(CXXFLAGS) -o hybrid_quicksort hybrid_quicksort.cpp

mergesort: mergesort.cpp
	$(CXX) $(WARN) $(CXXFLAGS) -o mergesort mergesort.cpp

clean:
	rm -f $(PROGRAM)
