CC=mpicxx
CFLAGS=-ansi -O2 -m64 -march=native -mavx2 -funroll-loops -std=c++11
WARNING=-Wall -Werror -Wextra -Wfloat-equal -pedantic
OBJ = main.o mpi_utility.o vector.o matrix.o

mpi_sgemv.out: $(OBJ)
	$(CC) $(CFLAGS) $(WARNING) $(OBJ) -o mpi_sgemv.out

main.o: main.cpp
	$(CC) $(CFLAGS) $(WARNING) main.cpp -c

mpi_utility.o: mpi_utility.cpp
	$(CC) $(CFLAGS) $(WARNING) mpi_utility.cpp -c

vector.o: vector.cpp
	$(CC) $(CFLAGS) $(WARNING) vector.cpp -c

matrix.o: matrix.cpp
	$(CC) $(CFLAGS) $(WARNING) matrix.cpp -c

oclean:
	rm -f $(OBJ)

clean:
	rm -f $(OBJ) mpi_sgemv.out
