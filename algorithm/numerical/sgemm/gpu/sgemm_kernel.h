#ifndef SGEMM_KERNEL_H
#define SGEMM_KERNEL_H


#include <cstdio>


/* MACROS
 * Static sizes for tiles and number of tiles in kernels and error checkers.
 */
#define TILE_WIDTH 32
#define N_TILE 4


/* Kernels
 */
__global__ void sgemm_basic_kernel(const float *A, const float *B, float *C, int N);
__global__ void sgemm_tiled_kernel(const float *A, const float *B, float *C, int N);


/* Functions
 */
void sgemm_basic(const float *A, const float *B, float *C, int N);
void sgemm_tiled(const float *A, const float *B, float *C, int N);


#endif
