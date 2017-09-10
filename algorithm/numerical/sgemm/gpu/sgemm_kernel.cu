#include <iostream>
#include <cstdio>
#include <cmath>
#include <cuda.h>

#include "sgemm_kernel.h"


/*-------------------------------------------------------------------------------------------------
 * KERNELS
 *-----------------------------------------------------------------------------------------------*/

/* sgemm_basic_kernel()
 * Basic SGEMM kernel where each thread performs a corresponding dot product.
 */
__global__ void sgemm_basic_kernel(const float *A, const float *B, float *C, int N)
{
    // Compute the row and col indices
    int r = blockIdx.y * blockDim.y + threadIdx.y;
    int c = blockIdx.x * blockDim.x + threadIdx.x;

    if ((r < N) && (c < N)) {
        float sum = 0.0;

        for (int i = 0; i < N; i++)
            sum += A[r * N + i] * B[i * N + c];

        C[r * N + c] = sum;
    } // Check if row and col indices is a part of the matrix
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* sgemm_basic()
 * Function for calling SGEMM.
 */
void sgemm_basic(const float *A, const float *B, float *C, int N)
{
    int size = N * N * sizeof(float);
    float *d_A, *d_B, *d_C;

    cudaMalloc((void**)&d_A, size);
    cudaMalloc((void**)&d_B, size);
    cudaMalloc((void**)&d_C, size);

    cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);

    int N_thd = 16;
    int N_blk = ceil(static_cast<float>(N) / static_cast<float>(N_thd));
    dim3 grid_dim(N_blk, N_blk, 1);
    dim3 blk_dim(N_thd, N_thd, 1);

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    sgemm_basic_kernel<<<grid_dim, blk_dim>>>(d_A, d_B, d_C, N);
    cudaEventRecord(stop);

    cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);
    cudaEventSynchronize(stop);

    float time_ms = 0.0;
    cudaEventElapsedTime(&time_ms, start, stop);

    std::cout << "Basic GPU SGEMM runtime: " << time_ms << "ms ";

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}
