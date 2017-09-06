/* Written by : Eric Tan
 *
 * hybrid_quicksort.cpp
 *
 * A hybrid implementation of quicksort. Performs the typical quicksort algorithm until a cutoff
 * has been reached. Once the cutoff for the number of elements in the subarray is reached, we
 * switch to insertsort since insertsort is faster than quicksort for small enough arrays.
 * Uses a random pivot insead of median of 3.
 */

#include <iostream>
#include <algorithm>
#include <random>
#include <utility>


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATION
 *-----------------------------------------------------------------------------------------------*/
void hybrid_quicksort(double *A, int k, int lo, int hi);
void insertsort(double *A, int lo, int hi);
void check_sorted(double *A, int N);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(void)
{
    const int N = 5000;
    const int cutoff = 50; // Purely arbituary choice.
    double *A = new double[N];

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> rand0(-100.0, 100.0);

    for (int i = 0; i < N; i++)
        A[i] = rand0(engine);

    hybrid_quicksort(A, cutoff, 0, N-1);
    check_sorted(A, N);

    delete[] A;
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* hybrid_quicksort()
 * Performs an in place recursive quicksort. At some cutoff k, we perform insertsort.
 */
void hybrid_quicksort(double *A, int k, int lo, int hi)
{
    if (hi <= lo)
        return;

    if (hi - lo <= k) {
        insertsort(A, lo, hi);
        return;
    } // Switch to insertsort if cutoff is reached

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> rand0(0.0, 1.0);

    int i = lo - 1;
    int p_idx = lo + static_cast<int>(rand0(engine) * (hi - lo + 1));
    double pivot = A[p_idx];

    std::swap(A[p_idx], A[hi]);
    for (int j = lo; j < hi; j++) {
        if (A[j] < pivot) {
            i++;
            std::swap(A[i], A[j]);
        }
    }
    std::swap(A[++i], A[hi]);

    hybrid_quicksort(A, k, lo, i-1);
    hybrid_quicksort(A, k, i+1, hi);
}


/* insertsort()
 * Performs insertsort on an array from [lo, hi]
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
        A[j+1] = val;
    }
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
