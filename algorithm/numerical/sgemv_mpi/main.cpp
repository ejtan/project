#include <iostream>
#include <string>
#include <mpi.h>

#include "vector.h"
#include "matrix.h"
#include "mpi_utility.h"


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATION
 *-----------------------------------------------------------------------------------------------*/
void row_replicated_sgemv(const std::string &mat, const std::string &vec, int p, int id);
void row_block_sgemv(const std::string &mat, const std::string &vec, int p, int id);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    int id, p;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if (argc != 3) {
        if (!id)
            std::cerr << "Error: Expected 2 inputs.\n" << argv[0] << "matrix vector\n";

        MPI_Finalize();
        exit(EXIT_FAILURE);
    } // Check for correct number of inputs

    row_replicated_sgemv(argv[1], argv[2], p, id);
    row_block_sgemv(argv[1], argv[2], p, id);

    MPI_Finalize();
}


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATION
 *-----------------------------------------------------------------------------------------------*/

/* row_replicated_sgemv()
 *
 * @param: mat = matrix filenmame
 * @param: vec = vector filename
 * @param: p = number of procs
 * @param: id = proc rank
 *
 * Implamentation of sgemv with a row decomposed matrix and a replicated vector. Output vector
 * will also be a row replicated vector.
 */
void row_replicated_sgemv(const std::string &mat, const std::string &vec, int p, int id)
{
    float *A, *b;

    dim2 dim = read_row_matrix(mat, &A, MPI_COMM_WORLD);
    int n = read_replicated_vector(vec, &b, MPI_COMM_WORLD);

    if (dim.second != n) {
        if (!id)
            std::cerr << "Error: Mismatched column and vector dimension.\n" << "Matrix dim = "
                << dim.first << " x " << dim.second << " Vector dim = " << n << '\n';
        delete[] A;
        delete[] b;
        return;
    } // Check if dimensions are the same

//    print_row_matrix(A, dim, MPI_COMM_WORLD);
//    print_replicated_vector(b, n, MPI_COMM_WORLD);

    // SGEMV implamentation
    int local_rows = block_size(id, p, dim.first);
    float *c_blk = new float[local_rows];
    float *c;

    // Since each process controls local_rows number of rows and have a full vector, when we perform
    // matrix vector multiply with the submatrix, and the full vector, so we are left with
    // local_rows elements in the resulting vector c. Thus, we need to combine the blocks of c into
    // a full vector.
    for (int i = 0; i < local_rows; i++) {
        c_blk[i] = 0.0;
        for (int j = 0; j < n; j++)
            c_blk[i] += A[i * n + j] * b[j];
    } // Loop over local_rows

    replicate_block_vector(c_blk, n, &c, MPI_COMM_WORLD);
    print_replicated_vector(c, n, MPI_COMM_WORLD);

    delete[] A;
    delete[] b;
    delete[] c_blk;
    delete[] c;
}


/* row_block_sgemv()
 *
 * @param: mat = matrix filenmame
 * @param: vec = vector filename
 * @param: p = number of procs
 * @param: id = proc rank
 *
 * Implamentation of SGEMV between a row striped matrix and a block vector. Output vector will
 * also be a block vector
 */
void row_block_sgemv(const std::string &mat, const std::string &vec, int p, int id)
{
    float *A, *b;

    dim2 dim = read_row_matrix(mat, &A, MPI_COMM_WORLD);
    int n = read_block_vector(vec, &b, MPI_COMM_WORLD);

    if (dim.second != n) {
        if (!id)
            std::cerr << "Error: Mismatched column and vector dimension.\n" << "Matrix dim = "
                << dim.first << " x " << dim.second << " Vector dim = " << n << '\n';
        delete[] A;
        delete[] b;
        return;
    } // Check if dimensions are the same

//    print_row_matrix(A, dim, MPI_COMM_WORLD);
//    print_block_vector(b, n, MPI_COMM_WORLD);

    // SGEMV implamentation
    int n_blk = block_size(id, p, n);
    float *c = new float[n_blk];
    float *replicate_b = new float[n];
    std::vector<int> disp, cnt;

    // In order to perform matrix vector multiplication with the subvector, we need to
    // gather blocks of b first. After performing the multiplication, we are left with a block
    // vector representing c.
    make_mixed_xfer_array(p, n, cnt, disp);
    MPI_Allgatherv(b, cnt[id], MPI_FLOAT, replicate_b,
            cnt.data(), disp.data(), MPI_FLOAT, MPI_COMM_WORLD);

    for (int i = 0; i < n_blk; i++) {
        c[i] = 0.0;
        for (int j = 0; j < n; j++)
            c[i] += A[i * n + j] * replicate_b[j];
    } // Loop over rows in block

    print_block_vector(c, n, MPI_COMM_WORLD);

    delete[] A;
    delete[] b;
    delete[] replicate_b;
    delete[] c;
}
