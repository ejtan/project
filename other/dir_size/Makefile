CXX=g++
CXXFLAGS=-std=c++17 -O2 -pipe -march=native -flto
WARN=-Wall -Wextra
LIBS=-lstdc++fs -lboost_program_options

dir_size.out: dir_size.cpp
	$(CXX) dir_size.cpp $(CXXFLAGS) $(WARN) $(LIBS) -o dir_size.out

clean:
	rm -f dir_size.out
