/* Written by : Eric Tan
 */

#include <iostream>
#include <cmath>
#include <array>
#include <cuda.h>

#define MAX_MASK_SIZE 50
#define TILE_SIZE 512
#define N_TILE 4


/*-------------------------------------------------------------------------------------------------
 * GLOBAL CONSTANTS
 *-----------------------------------------------------------------------------------------------*/
__constant__ float mask[MAX_MASK_SIZE];


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATION
 *-----------------------------------------------------------------------------------------------*/
__global__ void tiled_convolution_kernel(const float *input, float *output, int N_data, int N_mask);
__global__ void cache_convolution_kernel(const float *input, float *output, int N_data, int N_mask);
void convolution_tiled_gpu(const float *input, float *output, int N_data, int N_mask);
void convolution_cache_gpu(const float *input, float *output, int N_data, int N_mask);
void convolution_cpu(const float *input, float *output, const float *mask, int N_data, int N_mask);
double check_convolution(const float *A, const float *B, int N);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(void)
{
    const int mask_size = 9;
    const int N_data = 1000;
    const std::array<float, mask_size> mask_kernel = {3.0, 4.0, 5.0, 6.0, 7.0, 6.0, 5.0, 4.0, 3.0};

    // Copy to constant memory
    cudaMemcpyToSymbol(mask, mask_kernel.data(), mask_size * sizeof(float));

    float *input = new float[N_data];
    float *cpu_output = new float[N_data];
    float *gpu_output = new float[N_data];

    for (int i = 0; i < N_data; i++)
        input[i] = i + 1.0;

    convolution_cpu(input, cpu_output, mask_kernel.data(), N_data, mask_size);

    convolution_tiled_gpu(input, gpu_output, N_data, mask_size);
    std::cout << " Error = " << check_convolution(cpu_output, gpu_output, N_data) << '\n';

    convolution_cache_gpu(input, gpu_output, N_data, mask_size);
    std::cout << " Error = " << check_convolution(cpu_output, gpu_output, N_data) << '\n';

    delete[] input;
    delete[] cpu_output;
    delete[] gpu_output;
}


/*-------------------------------------------------------------------------------------------------
 * KERNELS
 *-----------------------------------------------------------------------------------------------*/

/* tiled_convolution_kernel()
 * Performs convolution by tiling the input and loading halo elements.
 */
__global__ void tiled_convolution_kernel(const float *input, float *output, int N_data, int N_mask)
{
    int idx  = blockIdx.x * blockDim.x + threadIdx.x;
    int half = N_mask / 2;
    __shared__ float tile[TILE_SIZE + MAX_MASK_SIZE - 1];

    // Left load
    // Maps the last N_mask/2 threads to the previous block (or 0 if at the edge)
    int left_idx = (blockIdx.x - 1) * blockDim.x + threadIdx.x;
    if (threadIdx.x >= blockDim.x - half)
        tile[threadIdx.x + half - blockDim.x] = (left_idx < 0) ? 0 : input[left_idx];

    // Middle load
    tile[half + threadIdx.x] = input[blockIdx.x * blockDim.x + threadIdx.x];

    // Right load
    // Maps the first N_mask/2 threads to the next block (or 0 if at the edge)
    int right_idx = (blockIdx.x + 1) * blockDim.x + threadIdx.x;
    if (threadIdx.x < half)
        tile[threadIdx.x + blockDim.x + half] = (right_idx >= N_data) ? 0 : input[right_idx];

    __syncthreads();

    // Convolution calculation
    float sum = 0.0;
    for (int i = 0; i < N_mask; i++)
        sum += tile[threadIdx.x + i] * mask[i];

    output[idx] = sum;
}


/* cache_convolution_kernel()
 * Performs convolution by loading multiple tiles of the input data and performing the convolution
 * operation. Halo elements are assumed to be loaded in the L2 cache so if theya re needed, we
 * will go to L2 cache to grabe them.
 */
__global__ void cache_convolution_kernel(const float *input, float *output, int N_data, int N_mask)
{
    int idx = (N_TILE * blockDim.x * blockIdx.x) + threadIdx.x;
    __shared__ float tile[N_TILE * TILE_SIZE];

    // Load Multiple tiles
    #pragma unroll
    for (int i = 0; i < N_TILE; i++)
        tile[threadIdx.x + i * TILE_SIZE] = input[idx + i * TILE_SIZE];

    __syncthreads();

    int tile_start = N_TILE * blockDim.x * blockIdx.x;
    int tile_end   = N_TILE * blockDim.x * (blockIdx.x + 1);

    #pragma unroll
    for (int i = 0; i < N_TILE; i++) {
        int start_point = idx + (i * TILE_SIZE) - (N_mask / 2);
        float sum = 0.0;

        for (int j = 0; j < N_mask; j++) {
            int input_idx = start_point + j;

            if (input_idx >= 0 && input_idx < N_data) {
                // Check if elements needed is in shared memory
                if (input_idx >= tile_start && input_idx < tile_end)
                    sum += tile[threadIdx.x + j - (N_mask / 2) + (i * TILE_SIZE)] * mask[j];
                else
                    sum += input[input_idx] * mask[j];

            } // Check if element is outside range of input data
        } // Loop over Mask elements

        output[idx + i * TILE_SIZE] = sum;
    } // Loop over multiple tiles
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* convolution_tiled_gpu()
 * Driver function for tiled convolution which loads halo elements.
 */
//void convolution_tiled_gpu(const float *input, float *output, int N_data, int N_mask)
void convolution_tiled_gpu(const float *input, float *output, int N_data, int N_mask)
{
    int size = N_data * sizeof(float);
    float *d_input, *d_output;

    cudaMalloc((void**)&d_input, size);
    cudaMalloc((void**)&d_output, size);

    cudaMemcpy(d_input, input, size, cudaMemcpyHostToDevice);

    int N_thd = TILE_SIZE;
    int N_blk = ceil(static_cast<float>(N_data) / static_cast<float>(N_thd));
    dim3 blk_dim(N_thd, 1, 1);
    dim3 grid_dim(N_blk, 1, 1 );

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    tiled_convolution_kernel<<<grid_dim, blk_dim>>>(d_input, d_output, N_data, N_mask);

    cudaEventRecord(stop);
    cudaMemcpy(output, d_output, size, cudaMemcpyDeviceToHost);
    cudaEventSynchronize(stop);

    float time_ms = 0.0;
    cudaEventElapsedTime(&time_ms, start, stop);

    std::cout << "Convolution with tiling and halo runtime: " << time_ms << "ms ";

    cudaFree(d_input);
    cudaFree(d_output);
}


/* convolution_tiled_gpu()
 * Driver function for tiled convolution without loading halo elements.
 */
void convolution_cache_gpu(const float *input, float *output, int N_data, int N_mask)
{
    int size = N_data * sizeof(float);
    float *d_input, *d_output;

    cudaMalloc((void**)&d_input, size);
    cudaMalloc((void**)&d_output, size);

    cudaMemcpy(d_input, input, size, cudaMemcpyHostToDevice);

    int N_thd = TILE_SIZE;
    int N_blk = ceil(static_cast<float>(N_data) / static_cast<float>(N_TILE * N_thd));
    dim3 blk_dim(N_thd, 1, 1);
    dim3 grid_dim(N_blk, 1, 1 );

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    cache_convolution_kernel<<<grid_dim, blk_dim>>>(d_input, d_output, N_data, N_mask);

    cudaEventRecord(stop);
    cudaMemcpy(output, d_output, size, cudaMemcpyDeviceToHost);
    cudaEventSynchronize(stop);

    float time_ms = 0.0;
    cudaEventElapsedTime(&time_ms, start, stop);

    std::cout << "Convolution with L2 cache use runtime: " << time_ms << "ms ";

    cudaFree(d_input);
    cudaFree(d_output);
}


/* convolution_cpu()
 * CPU based convolution operation. Used to compare GPU implamentations.
 */
void convolution_cpu(const float *input, float *output, const float *mask, int N_data, int N_mask)
{
    int half_point = N_mask / 2;

    for (int i = 0; i < N_data; i++) {
        float sum = 0.0;
        for (int j = 0; j < N_mask; j++) {
            if (i - half_point + j >= 0 && i - half_point + j < N_data)
                sum += input[i - half_point + j] * mask[j];
        }
        output[i] = sum;
    }
}


/* check_convolution()
 * Computes the difference between the two outputs of CPU and GPU convolutions.
 */
double check_convolution(const float *A, const float *B, int N)
{
    float sum = 0.0;

    for (int i = 0; i < N; i++)
        sum += fabs(A[i] - B[i]);

    return sum;
}
