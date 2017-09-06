/* Written by : Eric Tan
 *
 * mergesort.cpp
 *
 * A basic implementation of mergesort. Recursively divides an input array and performs a merge
 * operation when the base case is reached (array of size 1 or 0).
 */
#include <iostream>
#include <random>
#include <limits>


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATION
 *-----------------------------------------------------------------------------------------------*/
void mergesort(double *A, int lo, int hi);
void merge(double *A, int lo, int mid, int hi);
void check_sorted(double *A, int N);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(void)
{
    const int N = 5000;
    double *A = new double[N];

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> rand0(-100.0, 100.0);

    for (int i = 0; i < N; i++)
        A[i] = rand0(engine);

    mergesort(A, 0, N-1);
    check_sorted(A, N);
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* mergesort()
 * Performs mergesort by recursively divinding the array and perform a merge operation to sort
 * the array.
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


/* merge()
 * Merges two subarrays into a sorted one.
 */
void merge(double *A, int lo, int mid, int hi)
{
    double *B = new double[mid - lo + 2];
    double *C = new double[hi - mid + 1];

    for (int k = lo; k <= mid; k++)
        B[k-lo] = A[k];

    for (int k = mid+1; k<= hi; k++)
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


/* check_sorted()
 * Checks if the array is sorted correctly.
 */
void check_sorted(double *A, int N)
{
    for (int i = 1; i < N; i++) {
        if (A[i] < A[i-1]) {
            std::cout << "Error: Array is not sorted correctly.\n";
            return;
        }
    }

    std::cout << "Array is sorted correctly.\n";
}
