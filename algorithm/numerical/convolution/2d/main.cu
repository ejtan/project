#include <iostream>
#include <random>
#include <iomanip>
#include <cuda.h>

#define MASK_DIM_SIZE 10 // Number of elements for one spacial dimension
#define TILE_SIZE 16

__constant__ float d_mask[MASK_DIM_SIZE * MASK_DIM_SIZE];


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATION
 *-----------------------------------------------------------------------------------------------*/
__global__ void naive_convolution_kernel(const float *d_in, float *d_out,
        int M, int N, int mask_m, int mask_n);
__global__ void tiled_convolution_kernel(const float *d_in, float *d_out,
        int M, int N, int mask_m, int mask_n);
__global__ void cache_convolution_kernel(const float *d_in, float *d_out,
        int M, int N, int mask_m, int mask_n);

void cpu_convolution(const float *A, const float *mask, float *out,
        int M, int N, int mask_m, int mask_n);
float convolution_naive(const float *in, float *out, int M, int N, int mask_m, int mask_n);
float convolution_tiled(const float *in, float *out, int M, int N, int mask_m, int mask_n);
float convolution_cache(const float *in, float *out, int M, int N, int mask_m, int mask_n);
float check_convolution(const float *A, const float *B, int M, int N);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    const int M = 2000, N = 2000;
    const int mask_m = 3, mask_n = 3;
    float *A = new float[M * N];
    float *cpu_out = new float[M * N];
    float *naive_out = new float[M * N];
    float *tiled_out = new float[M * N];
    float *cache_out = new float[M * N];
    float *mask = new float[mask_m * mask_n];

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<float> rand0(0, 10.0);

    for (int i = 0; i < M * N; i++)
        A[i] = rand0(engine);

    mask[0] = 1.0;
    mask[1] = 2.0;
    mask[2] = 1.0;

    mask[3] = 2.0;
    mask[4] = 4.0;
    mask[5] = 2.0;

    mask[6] = 1.0;
    mask[7] = 2.0;
    mask[8] = 1.0;

    cudaMemcpyToSymbol(d_mask, mask, mask_m * mask_n * sizeof(float));

    cpu_convolution(A, mask, cpu_out, M, N, mask_m, mask_n);
    float naive_time = convolution_naive(A, naive_out, M, N, mask_m, mask_n);
    float tiled_time = convolution_tiled(A, tiled_out, M, N, mask_m, mask_n);
    float cache_time = convolution_cache(A, cache_out, M, N, mask_m, mask_n);

    float naive_err = check_convolution(cpu_out, naive_out, M, N);
    std::cout << "Naive convolution error = " << naive_err << ". Time = " << naive_time << "ms.\n";

    float tiled_err = check_convolution(cpu_out, tiled_out, M, N);
    std::cout << "tiled convolution error = " << tiled_err<< ". Time = " << tiled_time << "ms.\n";

    float cache_err = check_convolution(cpu_out, cache_out, M, N);
    std::cout << "cache convolution error = " << cache_err << ". Time = " << cache_time << "ms.\n";

    delete[] A;
    delete[] cpu_out;
    delete[] naive_out;
    delete[] tiled_out;
    delete[] cache_out;
    delete[] mask;
}


/*-------------------------------------------------------------------------------------------------
 * KERNELS
 *-----------------------------------------------------------------------------------------------*/

__global__ void naive_convolution_kernel(const float *d_in, float *d_out,
        int M, int N, int mask_m, int mask_n)
{
    int m_idx = blockIdx.y * blockDim.y + threadIdx.y;
    int n_idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Check if indices are within matrix
    if (m_idx < M && n_idx < N) {
        int start_m = m_idx - (mask_m / 2);
        int start_n = n_idx - (mask_n / 2);
        float val = 0.0;

        // Loop over mask
        for (int m = 0; m < mask_m; m++) {
            for (int n = 0; n < mask_n; n++) {
                int i = start_m + m;
                int j = start_n + n;

                if (i >= 0 && i < M && j >= 0 && j < N)
                    val += d_in[i * N + j] * d_mask[m * mask_n + n];
            }
        }

        d_out[m_idx * N + n_idx] = val;
    }
}


__global__ void tiled_convolution_kernel(const float *d_in, float *d_out,
        int M, int N, int mask_m, int mask_n)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    int m_half = mask_m / 2;
    int n_half = mask_n / 2;

    __shared__ float tile[TILE_SIZE + MASK_DIM_SIZE - 1][TILE_SIZE + MASK_DIM_SIZE - 1];

    int left_idx = (blockIdx.x - 1) * blockDim.x + threadIdx.x;
    int right_idx = (blockIdx.x + 1) * blockDim.x + threadIdx.x;
    int top_idx = (blockIdx.y - 1) * blockDim.y + threadIdx.y;
    int bottom_idx = (blockIdx.y + 1) * blockDim.y + threadIdx.y;

    // Load block matrix
    tile[threadIdx.y + m_half][threadIdx.x + n_half] = (row < M || col < N) ? d_in[row * N + col] : 0;

    // Load left and right tiles
    if (threadIdx.x >= blockDim.x - n_half)
        tile[threadIdx.y + m_half][threadIdx.x - blockDim.x + n_half] =
            (left_idx < 0) ? 0 : d_in[row * N + left_idx];

    if (threadIdx.x < n_half)
        tile[threadIdx.y + m_half][threadIdx.x + blockDim.x + n_half] =
            (right_idx >= N) ? 0 : d_in[row * N + right_idx];

    // Load top and bottom tiles, including corners
    if (threadIdx.y >= blockDim.y - m_half) {
        tile[threadIdx.y - blockDim.y + m_half][threadIdx.x + n_half] =
            (top_idx < 0) ? 0 : d_in[top_idx * N + col];

        if (threadIdx.x >= blockDim.x - n_half)
            tile[threadIdx.y - blockDim.y + m_half][threadIdx.x - blockDim.x + n_half] =
                (top_idx < 0 || left_idx < 0) ? 0 : d_in[top_idx * N + left_idx];
        else if (threadIdx.x < n_half)
            tile[threadIdx.y - blockDim.y + m_half][threadIdx.x + blockDim.x + n_half] =
                (top_idx < 0 || right_idx >= N) ? 0 : d_in[top_idx * N + right_idx];
    }

    if (threadIdx.y < m_half) {
        tile[threadIdx.y + blockDim.y + m_half][threadIdx.x + n_half] =
            (bottom_idx >= M) ? 0 : d_in[bottom_idx * N + col];

        if (threadIdx.x >= blockDim.x - n_half)
            tile[threadIdx.y + blockDim.y + m_half][threadIdx.x - blockDim.x + n_half] =
                (bottom_idx >= M || left_idx < 0) ? 0 : d_in[bottom_idx * N + left_idx];
        else if (threadIdx.x < n_half)
            tile[threadIdx.y + blockDim.y + m_half][threadIdx.x + blockDim.x + n_half] =
                (bottom_idx >= M || right_idx >= N) ? 0 : d_in[bottom_idx * N + right_idx];
    }

    __syncthreads();

    if (row < M && col < N) {
        float sum = 0.0;

        // Perform convolution
        for (int i = 0; i < mask_m; i++)
            for (int j = 0; j < mask_n; j++)
                sum += tile[threadIdx.y + i][threadIdx.x + j] * d_mask[i * mask_n + j];

        d_out[row * N + col] = sum;
    }
}


__global__ void cache_convolution_kernel(const float *d_in, float *d_out,
        int M, int N, int mask_m, int mask_n)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int m_half = mask_m / 2;
    int n_half = mask_n / 2;

    __shared__ float tile[TILE_SIZE][TILE_SIZE];

    if (row < M && col < N)
        tile[threadIdx.y][threadIdx.x] = d_in[row * N + col];

    __syncthreads();

    int row_tile_start = blockIdx.y * blockDim.y;
    int row_tile_end = (blockIdx.y + 1) * blockDim.y;
    int col_tile_start = blockIdx.x * blockDim.x;
    int col_tile_end = (blockIdx.x + 1) * blockDim.x;

    int m_start = row - m_half;
    int n_start = col - n_half;
    float sum = 0.0;

    if (row < M && col < N) {
        // Loop over mask
        for (int i = 0; i < mask_m; i++) {
            for (int j = 0; j < mask_n; j++) {
                int m = m_start + i;
                int n = n_start + j;

                // Check if indicies with mask applied is within matrix
                if (m >= 0 && m < M && n >= 0 && n < N) {
                    // Check if elements are in the cache
                    if (m >= row_tile_start && m < row_tile_end &&
                            n >= col_tile_start && n < col_tile_end)
                        sum += tile[threadIdx.y + i - m_half][threadIdx.x + j - n_half] *
                            d_mask[i * mask_n + j];
                    else
                        sum += d_in[m * N + n] * d_mask[i * mask_n + j];
                }
            }
        }
        d_out[row * N + col] = sum;
    }
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

void cpu_convolution(const float *A, const float *mask, float *out,
        int M, int N, int mask_m, int mask_n)
{
    int half_m = mask_m / 2;
    int half_n = mask_n / 2;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {

            int start_m = i - half_m;
            int start_n = j - half_n;
            float val = 0.0;

            for (int m = 0; m < mask_m; m++) {
                for (int n = 0; n < mask_n; n++) {
                    int m_idx = start_m + m;
                    int n_idx = start_n + n;

                    if (m_idx >= 0 && m_idx < M && n_idx >= 0 && n_idx < N)
                        val += A[m_idx * N + n_idx] * mask[m * mask_n + n];
                }
            }
            out[i * N + j] = val;
        }
    }
}


float convolution_naive(const float *in, float *out, int M, int N, int mask_m, int mask_n)
{
    int size = M * N * sizeof(float);
    float *d_in, *d_out;

    cudaMalloc((void**) &d_in, size);
    cudaMalloc((void**) &d_out, size);

    cudaMemcpy(d_in, in, size, cudaMemcpyHostToDevice);

    int N_thd = TILE_SIZE;
    int M_blk = ceil(static_cast<float>(M) / static_cast<float>(N_thd));
    int N_blk = ceil(static_cast<float>(N) / static_cast<float>(N_thd));

    dim3 grid_dim(M_blk, N_blk, 1);
    dim3 blk_dim(N_thd, N_thd, 1);

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    naive_convolution_kernel<<<grid_dim, blk_dim>>>(d_in, d_out, M, N, mask_m, mask_n);
    cudaEventRecord(stop);

    cudaMemcpy(out, d_out, size, cudaMemcpyDeviceToHost);
    cudaEventSynchronize(stop);

    float time_ms;
    cudaEventElapsedTime(&time_ms, start, stop);

    cudaFree(d_in);
    cudaFree(d_out);

    return time_ms;
}


float convolution_tiled(const float *in, float *out, int M, int N, int mask_m, int mask_n)
{
    int size = M * N * sizeof(float);
    float *d_in, *d_out;

    cudaMalloc((void**) &d_in, size);
    cudaMalloc((void**) &d_out, size);

    cudaMemcpy(d_in, in, size, cudaMemcpyHostToDevice);

    int N_thd = TILE_SIZE;
    int M_blk = ceil(static_cast<float>(M) / static_cast<float>(N_thd));
    int N_blk = ceil(static_cast<float>(N) / static_cast<float>(N_thd));

    dim3 grid_dim(M_blk, N_blk, 1);
    dim3 blk_dim(N_thd, N_thd, 1);

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    tiled_convolution_kernel<<<grid_dim, blk_dim>>>(d_in, d_out, M, N, mask_m, mask_n);
    cudaEventRecord(stop);

    cudaMemcpy(out, d_out, size, cudaMemcpyDeviceToHost);
    cudaEventSynchronize(stop);

    float time_ms;
    cudaEventElapsedTime(&time_ms, start, stop);

    cudaFree(d_in);
    cudaFree(d_out);

    return time_ms;
}


float convolution_cache(const float *in, float *out, int M, int N, int mask_m, int mask_n)
{
    int size = M * N * sizeof(float);
    float *d_in, *d_out;

    cudaMalloc((void**) &d_in, size);
    cudaMalloc((void**) &d_out, size);

    cudaMemcpy(d_in, in, size, cudaMemcpyHostToDevice);

    int N_thd = TILE_SIZE;
    int M_blk = ceil(static_cast<float>(M) / static_cast<float>(N_thd));
    int N_blk = ceil(static_cast<float>(N) / static_cast<float>(N_thd));

    dim3 grid_dim(M_blk, N_blk, 1);
    dim3 blk_dim(N_thd, N_thd, 1);

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    cache_convolution_kernel<<<grid_dim, blk_dim>>>(d_in, d_out, M, N, mask_m, mask_n);
    cudaEventRecord(stop);

    cudaMemcpy(out, d_out, size, cudaMemcpyDeviceToHost);
    cudaEventSynchronize(stop);

    float time_ms;
    cudaEventElapsedTime(&time_ms, start, stop);

    cudaFree(d_in);
    cudaFree(d_out);

    return time_ms;
}


float check_convolution(const float *A, const float *B, int M, int N)
{
    float sum = 0.0;

    for (int i = 0; i < N; i++)
        sum += (A[i] - B[i]) * (A[i] - B[i]);

    return sqrt(sum);
}
