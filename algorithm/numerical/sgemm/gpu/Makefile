CC=nvcc
CUDAFLAGS=-arch=sm_52 -std=c++11
CXXFLAGS=-Xcompiler -Wall,-Werror,-O2,-mavx2,-march=native
LIB=-lopenblas -lpthread
OBJ=sgemm_kernel.o main.o


main.out: $(OBJ)
	$(CC) $(CXXFLAGS) -o main.out $(OBJ) $(LIB) $(CUDAFLAGS)

main.o: main.cu
	$(CC) $(CXXFLAGS) -c main.cu $(CUDAFLAGS)

sgemm_kernel.o: sgemm_kernel.cu
	$(CC) $(CXXFLAGS) -c sgemm_kernel.cu $(CUDAFLAGS)

clean:
	rm -f main.out $(OBJ)
