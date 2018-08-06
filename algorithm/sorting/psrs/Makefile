CXX=mpicxx
CXXFLAGS=-std=c++17 -O2 -march=native -mavx2 -pipe
LIBS=-lboost_mpi -lboost_serialization
TARGET=psrs.out

psrs.out: main.cpp
	$(CXX) $(CXXFLAGS) $(LIBS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET)
