/* Written by : Eric Tan
 *
 * reduction.cpp
 *
 * Program which implements a multi-threadded version of a parallel reduction.
 * Uses C++11 threads for the threadding and boost.SIMD for vectorization.
 * Compares the timing between a non-vectorized for loop and a vectorized for loop using OpenMP
 * simd construct. Written with a pack of 4 doubles (256-bit SIMD register length. Ran on a
 * i5-7200u with AVX2 support.
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>

#include <boost/simd/function/load.hpp>
#include <boost/simd/function/sum.hpp>
#include <boost/simd/pack.hpp>


/*-------------------------------------------------------------------------------------------------
 * GLOBAL VARIABLES
 *-----------------------------------------------------------------------------------------------*/
std::mutex reduction_barrier;
namespace bs = boost::simd;


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 *-----------------------------------------------------------------------------------------------*/
double reduction_sum(const double *A, int N);
void partial_reduction(const double *A, int i, int chunk, double &sum);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(void)
{
    int N = 10000001;
    double *A = new double[N];

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> rand0(-1.0, 1.0);

    for (int i = 0; i < N; i++)
        A[i] = rand0(engine);

    // Perform my impmenetation of a parallel reduction
    auto start = std::chrono::high_resolution_clock::now();
    double sum = reduction_sum(A, N);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Parallel reduction solution: " << sum << ". Time taken: " << elapsed << " us.\n";

    // OpenMP reduction with threads and simd
    double simd_sum = 0.0;
    start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for simd simdlen(4) reduction(+:simd_sum) schedule(static)
    for (int i = 0; i < N; i++)
        simd_sum += A[i];
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "OMP SIMD parallel loop: " << simd_sum << ". Time taken: " << elapsed << " us.\n";

    // Reference. Uses a basic loop
    double loop_sum = 0.0;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++)
        loop_sum += A[i];
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "For loop reduction solution: " <<
        loop_sum << ". Time taken: " << elapsed << " us.\n";

    delete[] A;
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* partial_reduction()
 * Reduces a part of an array using SIMD pack and reduce.
 */
void partial_reduction(const double *A, int i, int chunk, double &sum)
{
    bs::pack<double, 4> simd_sum{0};
    int stride = bs::pack<double, 4>::static_size;
    int k = i * chunk;

    // Pack 4 elements and add them into simd_sum.
    for (; k < (i+1) * chunk; k += stride, A += stride)
    {
        bs::pack<double, 4> vec = bs::load<bs::pack<double, 4>>(A);
        simd_sum += vec;
    }

    double local_sum = bs::sum(simd_sum);

    // Sum up remainder in the chunk
    if (chunk % stride != 0)
        for (; k < (i+1) * chunk; k++)
            local_sum += A[k];

    // Critical section for updating
    reduction_barrier.lock();
    sum += local_sum;
    reduction_barrier.unlock();
}


/* reduction_sum()
 * Creates threads and assiagns subarrays to a particular thread. Each thread reduces the subarray.
 */
double reduction_sum(const double *A, int N)
{
    int n_thd = std::thread::hardware_concurrency();
    int chunk = N / n_thd;

    std::vector<std::thread> thd;
    double result = 0.0;

    // Create threads to execute partial work on the array
    for (int i = 0; i < n_thd; i++)
        thd.push_back(std::thread(partial_reduction, A + (i * chunk), i, chunk, std::ref(result)));

    // Join arrays
    for (auto &t : thd)
        t.join();

    // If there is some leftover at the end, sum the result in serial
    if (N % n_thd != 0)
        for (int i = n_thd * chunk; i < N; i++)
            result += A[i];

    return result;
}
