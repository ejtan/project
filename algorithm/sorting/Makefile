CXX=g++
WARN=-Wall -Werror -ansi
CXXFLAGS=-std=c++11 -O2 -mavx2 -fopenmp

PROGRAM=hybrid_quicksort mergesort task_quicksort

all: $(PROGRAM)

hybrid_quicksort: hybrid_quicksort.cpp
	$(CXX) $(WARN) $(CXXFLAGS) -o hybrid_quicksort hybrid_quicksort.cpp

mergesort: mergesort.cpp
	$(CXX) $(WARN) $(CXXFLAGS) -o mergesort mergesort.cpp

task_quicksort: task_quicksort.cpp
	$(CXX) $(WARN) $(CXXFLAGS) -o task_quicksort task_quicksort.cpp

clean:
	rm -f $(PROGRAM)
