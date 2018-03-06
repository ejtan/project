#pragma once

#include <string>
#include <mpi.h>


// Input
int read_block_vector(const std::string &filename, float **v, MPI_Comm comm);
int read_replicated_vector(const std::string &filename, float **v, MPI_Comm comm);

// Output
void print_block_vector(const float *v, int n, MPI_Comm comm);
void print_replicated_vector(const float *v, int n, MPI_Comm comm);
void print_subvector(const float *v, int n);

// Misc
void replicate_block_vector(const float *v_block, int n, float **v_rep, MPI_Comm comm);
