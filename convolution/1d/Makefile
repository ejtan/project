convolution.out: main.cu
	nvcc -arch=sm_52 -o convolution.out main.cu -std=c++11 -Xcompiler -Wall,-Werror,-O2

clean:
	rm -f convolution.out
