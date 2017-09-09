/* Written by : Eric Tan
 *
 * sort_util.cpp
 *
 * Utility routines for sorting algorithms and checking if array is sorted.
 */

#include <utility>
#include <limits>

#include "sort_util.h"


/* partition()
 * Partitions two arrays into values higher than A[hi] and values lower than A[hi].
 */
int partition(double *A, int lo, int hi)
{
    int i = lo - 1;

    for (int j = lo; j < hi; j++)
        if (A[j] < A[hi])
            std::swap(A[++i], A[j]);

    std::swap(A[++i], A[hi]);

    return i;
}


/* swap_median()
 * Picks three elements A[lo, (hi + lo) / 2, hi] and picks the median. The median is than
 * swapped with A[hi]. If A[hi] is the median, nothing is done.
 */
void swap_median(double *A, int lo, int hi)
{
    int mid = (hi + lo) / 2;

    if (A[mid] > A[lo] && A[mid] < A[hi]) {
        std::swap(A[mid], A[hi]);
        return;
    }

    if (A[lo] > A[mid] && A[lo] < A[hi]) {
        std::swap(A[lo], A[hi]);
        return;
    }
}


/* merge()
 * Merge operation for mergesort.
 */
void merge(double *A, int lo, int mid, int hi)
{
    double *B = new double[mid - lo + 2];
    double *C = new double[hi - mid + 1];

    for (int k = lo; k <= mid; k++)
        B[k-lo] = A[k];
    for (int k = mid+1; k <= hi; k++)
        C[k-mid-1] = A[k];

    B[mid-lo+1] = std::numeric_limits<double>::max();
    C[hi - mid] = std::numeric_limits<double>::max();

    int i = 0, j = 0;

    for (int k = lo; k <= hi; k++) {
        if (C[j] < B[i])
            A[k] = C[j++];
        else
            A[k] = B[i++];
    }

    delete[] B;
    delete[] C;
}


/* is_sorted()
 * Checks if an array is sorted.
 */
bool is_sorted(double *A, int N)
{
    for (int i = 1; i < N; i++) {
        if (A[i] < A[i-1]) {
            return false;
        }
    }

    return true;
}
