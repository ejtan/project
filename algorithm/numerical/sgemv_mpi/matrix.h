#pragma once

#include <string>
#include <utility>
#include <mpi.h>


// typedef
typedef std::pair<int, int> dim2;

// Input
dim2 read_row_matrix(const std::string &filename, float **A, MPI_Comm comm);
dim2 read_col_matrix(const std::string &filename, float **A, MPI_Comm comm);

// Output
void print_row_matrix(const float *A, const dim2 &dim, MPI_Comm comm);
void print_col_matrix(const float *A, const dim2 &dim, MPI_Comm comm);
void print_submatrix(const float *A, int m, int n);

