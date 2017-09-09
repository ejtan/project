/* Written by : Eric Tan
 *
 * main.cpp
 *
 * A program to test the timing of different sorting algorithms (and mergesort as a comparsion).
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>
#include <random>
#include <chrono>

#include "sort.h"
#include "sort_util.h"


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATION
 *-----------------------------------------------------------------------------------------------*/
void copy_array(double *A, double *B, int N);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(void)
{
    std::ofstream of("timing.txt");
    std::array<int, 16> data_size = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000,
                                      20000, 50000, 100000, 200000, 500000, 1000000};
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> rand0(-100.0, 100.0);

    of << "# n | hybrid quicksort | task quicksort | random quicksort | quicksort | mergesort\n";

    for (auto &n : data_size) {
        double *init_data = new double[n];
        double *sort_data = new double[n];

        for (int i = 0; i < n; i++)
            init_data[i] = rand0(engine);

        of << std::left << std::setw(8) << n << ' ';

        copy_array(init_data, sort_data, n);
        auto start = std::chrono::high_resolution_clock::now();
        hybrid_quicksort(sort_data, 0, n-1, 50);
        auto end = std::chrono::high_resolution_clock::now();
        if (is_sorted(sort_data, n))
            of << std::setw(8) <<
                std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << ' ';
        else
            of << "NA ";

        copy_array(init_data, sort_data, n);
        start = std::chrono::high_resolution_clock::now();
        task_quicksort(sort_data, 0, n-1, 50);
        end = std::chrono::high_resolution_clock::now();
        if (is_sorted(sort_data, n))
            of << std::setw(8) <<
                std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << ' ';
        else
            of << "NA ";

        copy_array(init_data, sort_data, n);
        start = std::chrono::high_resolution_clock::now();
        random_quicksort(sort_data, 0, n-1);
        end = std::chrono::high_resolution_clock::now();
        if (is_sorted(sort_data, n))
            of << std::setw(8) <<
                std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << ' ';
        else
            of << "NA ";

        copy_array(init_data, sort_data, n);
        start = std::chrono::high_resolution_clock::now();
        quicksort(sort_data, 0, n-1);
        end = std::chrono::high_resolution_clock::now();
        if (is_sorted(sort_data, n))
            of << std::setw(8) <<
                std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << ' ';
        else
            of << "NA ";

        copy_array(init_data, sort_data, n);
        start = std::chrono::high_resolution_clock::now();
        mergesort(sort_data, 0, n-1);
        end = std::chrono::high_resolution_clock::now();
        if (is_sorted(sort_data, n))
            of << std::left <<
                std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << '\n';
        else
            of << "NA\n";

        delete[] init_data;
        delete[] sort_data;
    }

    of.close();
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* copy_array()
 * Copies array A into array B
 */
void copy_array(double *A, double *B, int N)
{
    for (int i = 0; i < N; i++)
        B[i] = A[i];
}
