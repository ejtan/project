/* Written by : Eric Tan
 *
 * task_quicksort.cpp
 *
 * Task based parallelism implementation of quicksort with median of three as the pivot choice.
 * Esentially the same as quicksort but each recursive call is set as a new task on a work queue.
 * Defaults to insertsort for a small subarray. Uses OpenMP to generate the task parallelism.
 */

#include <iostream>
#include <random>
#include <utility>


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATION
 *-----------------------------------------------------------------------------------------------*/
void task_quicksort(double *A, int cutoff, int lo, int hi, bool is_parallel=false);
void insertsort(double *A, int lo, int hi);
void check_sorted(double *A, int N);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(void)
{
    const int N = 1000;
    const int cutoff = 50;
    double *A = new double[N];

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> rand0(-100.0, 100.0);

    for (int i = 0; i < N; i++)
        A[i] = rand0(engine);

    task_quicksort(A, cutoff, 0, N-1);
    check_sorted(A, N);

    delete[] A;
}

/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* task_quicksort()
 * Performa quicksort but with task parallelism.
 */
void task_quicksort(double *A, int cutoff, int lo, int hi, bool is_parallel)
{
    #pragma omp parallel
    {
        if (!is_parallel) {
            #pragma omp single nowait
            task_quicksort(A, cutoff, lo, hi, true);
        } else {
            if (hi - lo <= cutoff) {
                #pragma omp task
                insertsort(A, lo, hi);
            }

            if (lo < hi) {
                int mid = (hi + lo) / 2;
                int i = lo - 1;

                // Finds the median of three.
                // If the median is in A[hi], none of the if statements execute

                // Mid is the median
                if (A[mid] > A[lo] && A[mid] < A[hi])
                    std::swap(A[mid], A[hi]);

                // Lo is the median
                if (A[lo] > A[mid] && A[lo] < A[hi])
                    std::swap(A[lo], A[hi]);


                for (int j = lo; j < hi; j++) {
                    if (A[j] < A[hi]) {
                        i++;
                        std::swap(A[i], A[j]);
                    }
                }
                std::swap(A[++i], A[hi]);

                #pragma omp task
                task_quicksort(A, cutoff, lo, i-1, false);
                #pragma omp task
                task_quicksort(A, cutoff, i+1, hi, false);
            }
        }
    }
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
 * Checks if an array is sorted.
 */
void check_sorted(double *A, int N)
{
    for (int i = 1; i < N; i++) {
        if (A[i] < A[i-1]) {
            std::cout << "Error: Array is not sorted correctly.\n" << std::endl;
            return;
        }
    }

    std::cout << "Array is sorted correctly." << std::endl;
}
