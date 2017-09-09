CXX=g++
WARN=-Wall -Werror -ansi
CXXFLAGS=-std=c++11 -O2 -mavx2 -fopenmp -march=native
OBJ=main.o sort.o sort_util.o

sort.out: main.o sort.o sort_util.o
	$(CXX) $(WARN) $(CXXFLAGS) -o sort.out $(OBJ)

main.o: main.cpp
	$(CXX) $(WARN) $(CXXFLAGS) -c main.cpp

sort.o: sort.cpp
	$(CXX) $(WARN) $(CXXFLAGS) -c sort.cpp

sort_util.o: sort_util.cpp
	$(CXX) $(WARN) $(CXXFLAGS) -c sort_util.cpp

oclean:
	rm -f $(OBJ)

clean:
	rm -f $(OBJ) sort.out
