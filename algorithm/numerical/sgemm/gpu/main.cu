#include <iostream>
#include <chrono>

#include <openblas/cblas.h>

#include "sgemm_kernel.h"


float compute_error(const float *A, const float *B, int N);


int main(void)
{
    const int N = 4096;
    float *A     = new float[N * N];
    float *B     = new float[N * N];
    float *C_gpu = new float[N * N];
    float *C_ref = new float[N * N];

    for (int i = 0; i < N * N; i++) {
        A[i] = static_cast<float>(i % 50);
        B[i] = static_cast<float>(i % 10 + 1.0);
        C_ref[i] = 0.0;
    }

    // Compute reference on the CPU
    auto start = std::chrono::high_resolution_clock::now();
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, A, N, B, N, 0.0, C_ref, N);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Reference (openBLAS) runtime: " << elapsed << "ms.\n";

    sgemm_basic(A, B, C_gpu, N);
    std::cout << " Error per element = " << compute_error(C_gpu, C_ref, N) << '\n';

    sgemm_tiled(A, B, C_gpu, N);
    std::cout << " Error per element = " << compute_error(C_gpu, C_ref, N) << '\n';

    delete[] A;
    delete[] B;
    delete[] C_gpu;
    delete[] C_ref;
}


float compute_error(const float *A, const float *B, int N)
{
    float error = 0.0;

    for (int i = 0; i < N * N; i++)
        error += fabs(A[i] - B[i]);

    return error / static_cast<float>(N * N);
}
