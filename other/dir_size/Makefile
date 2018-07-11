CXX=g++
CXXFLAGS=-std=c++17 -O2 -pipe -march=native -flto
WARN=-Wall -Wextra
LIBS=-lstdc++fs -lboost_program_options -lfmt
OBJ=main.o count_filesize.o cmd_options.o

dir_size.out: $(OBJ)
	$(CXX) $(OBJ) $(CXXFLAGS) $(WARN) $(LIBS) -o dir_size.out

main.o: main.cpp
	$(CXX) main.cpp $(CXXFLAGS) $(WARN) $(LIBS) -c

count_filesize.o: count_filesize.cpp
	$(CXX) count_filesize.cpp $(CXXFLAGS) $(WARN) $(LIBS) -c

cmd_options.o: cmd_options.cpp
	$(CXX) cmd_options.cpp $(CXXFLAGS) $(WARN) $(LIBS) -c

oclean:
	rm -f $(OBJ)

clean:
	rm -f $(OBJ) dir_size.out
