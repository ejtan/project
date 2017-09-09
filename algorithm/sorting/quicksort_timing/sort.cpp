/* Writtn by : Eric Tan
 *
 * sort.cpp
 *
 * Implementation of different sorting algorithms. Currently implemented are:
 * -- quicksort (various)
 *  > random pivot
 *  > hybrid
 *  > task based parellel
 *  > standard (median of three)
 * -- insertsort
 * -- mergesort
 */

#include <random>

#include "sort.h"
#include "sort_util.h"


/*hybrid_quicksort()
 * Peforms recursive quicksort until a subarray of cutoff is reached.
 * Then, the algorithm switches to insertsort since quicksort is slower for
 * slamm enough arrays.
 */
void hybrid_quicksort(double *A, int lo, int hi, int cutoff)
{
    if (hi <= lo)
        return;

    if (hi - lo <= cutoff) {
        insertsort(A, lo, hi);
        return;
    }

    swap_median(A, lo, hi);

    int mid = partition(A, lo, hi);

    hybrid_quicksort(A, lo, mid+1, cutoff);
    hybrid_quicksort(A, mid-1, hi, cutoff);
}


/* task_quicksort()
 * Hybrid quicksort with task based parallelism.
 */
void task_quicksort(double *A, int lo, int hi, int cutoff, bool is_parallel)
{
    #pragma omp parallel
    {
        if (!is_parallel) {
            #pragma omp single nowait
            task_quicksort(A, lo, hi, cutoff, true);
        } else {
            if (hi - lo <= cutoff) {
                #pragma omp task
                insertsort(A, lo, hi);
            }

            if (lo < hi) {
                swap_median(A, lo, hi);

                int mid = partition(A, lo, hi);

                #pragma omp task
                task_quicksort(A, lo, mid-1, cutoff, true);
                #pragma omp task
                task_quicksort(A, mid+1, hi, cutoff, true);
            }
        }
    }
}


/* random_quicksort()
 * Quicksort with a random pivot choice.
 */
void random_quicksort(double *A, int lo, int hi)
{
    if (hi <= lo)
        return;

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> rand0(0.0, 1.0);

    int p_idx = lo + static_cast<int>(rand0(engine) * (hi - lo + 1));
    std::swap(A[p_idx], A[hi]);

    int mid = partition(A, lo, hi);

    random_quicksort(A, lo, mid-1);
    random_quicksort(A, mid+1, hi);
}


/* quicksort()
 * Standard quicksort implementation. Uses median of three as the pivot.
 */
void quicksort(double *A, int lo, int hi)
{
    if (hi <= lo)
        return;

    swap_median(A, lo, hi);

    int mid = partition(A, lo, hi);

    quicksort(A, lo, mid-1);
    quicksort(A, mid+1, hi);
}


/* insertsort()
 * Basic sort algorithm using insertsort.
 */
void insertsort(double *A, int lo, int hi)
{
    for (int i = lo + 1; i <= hi; i++) {
        double val = A[i];
        int j = i - 1;

        while (j >= 0 && A[j] > val) {
            A[j+1] = A[j];
            j--;
        }

        A[j + 1] = val;
    }
}


/* mergesort()
 * Splits an array into two, recursively. Once base case is reached, merge operations is
 * perfrmed up.
 */
void mergesort(double *A, int lo, int hi)
{
    if (hi <= lo)
        return;

    int mid = lo + (hi - lo) / 2;
    mergesort(A, lo, mid);
    mergesort(A, mid+1, hi);
    merge(A, lo, mid, hi);
}
