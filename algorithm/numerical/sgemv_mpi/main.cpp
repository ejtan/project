/* NOTE: CHECK FOR NONSQUARE MATRICES
 */

#include <iostream>
#include <string>
#include <vector>
#include <mpi.h>

#include "vector.h"
#include "matrix.h"
#include "mpi_utility.h"


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATION
 *-----------------------------------------------------------------------------------------------*/
void row_replicated_sgemv(const std::string &mat, const std::string &vec, int p, int id);
void row_block_sgemv(const std::string &mat, const std::string &vec, int p, int id);
void col_replicated_sgemv(const std::string &mat, const std::string &vec, int p, int id);
void col_block_sgemv(const std::string &mat, const std::string &vec, int p, int id);


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
    col_replicated_sgemv(argv[1], argv[2], p, id);
    col_block_sgemv(argv[1], argv[2], p, id);

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


/* col_replicated_sgemv()
 *
 * @param: mat = matrix filenmame
 * @param: vec = vector filename
 * @param: p = number of procs
 * @param: id = proc rank
 *
 * Implamentation of SGEMV between a col striped matrix and a replicated vector. Output vector will
 * also be replicated.
 */
void col_replicated_sgemv(const std::string &mat, const std::string &vec, int p, int id)
{
    float *A, *b;

    dim2 dim = read_col_matrix(mat, &A, MPI_COMM_WORLD);
    int n = read_replicated_vector(vec, &b, MPI_COMM_WORLD);

    if (dim.second != n) {
        if (!id)
            std::cerr << "Error: Mismatched column and vector dimension.\n" << "Matrix dim = "
                << dim.first << " x " << dim.second << " Vector dim = " << n << '\n';
        delete[] A;
        delete[] b;
        return;
    } // Check if dimensions are the same

//    print_col_matrix(A, dim, MPI_COMM_WORLD);
//    print_replicated_vector(b, n, MPI_COMM_WORLD);

    // SGEMV Implamentation
    int local_cols = block_size(id, p, n);
    float *partial_c = new float[dim.first];
    std::vector<int> cnt_out, disp_out, cnt_in, disp_in;

    // Generate transfer arrays for alltoallv
    make_mixed_xfer_array(p, dim.first, cnt_out, disp_out);
    make_uniform_xfer_array(id, p, dim.first, cnt_in, disp_in);

    // Compute partial results of c. There are local_cols worth of elements to perform
    // the matrix vector multiplication, so we will only have the partial result.
    // Note that elements in the replicated vector needs to be offset using disp_out[id].
    for (int i = 0; i < dim.first; i++) {
        partial_c[i] = 0.0;
        for (int j = 0; j < local_cols; j++)
            partial_c[i] += A[i * local_cols + j] * b[disp_out[id] + j];
    } // Loop over rows

    float *partial_c_blk = new float[p * local_cols];
    float *c_blk = new float[local_cols];
    float *c = new float[dim.first];

    // Gather all partial sums into partial_blk_c. Then, we perform a sum of all these
    // partial vectors. The elements of partial_blk_c are further reduced into c_blk and
    // reconstructed from blocks into a replicated vector c.
    MPI_Alltoallv(partial_c, cnt_out.data(), disp_out.data(), MPI_FLOAT, partial_c_blk,
            cnt_in.data(), disp_in.data(), MPI_FLOAT, MPI_COMM_WORLD);

    for (int i = 0; i < local_cols; i++) {
        c_blk[i] = 0.0;
        for (int j = 0; j < p; j++)
            c_blk[i] += partial_c_blk[i + j * local_cols];
    } // Loop over number of local elements.

    replicate_block_vector(c_blk, dim.first, &c, MPI_COMM_WORLD);
    print_replicated_vector(c, dim.first, MPI_COMM_WORLD);

    delete[] A;
    delete[] b;
    delete[] partial_c;
    delete[] partial_c_blk;
    delete[] c_blk;
    delete[] c;
}


/* col_block_sgemv()
 *
 * @param: mat = matrix filenmame
 * @param: vec = vector filename
 * @param: p = number of procs
 * @param: id = proc rank
 *
 * Implamentation of sgemv with a column striped matrix and block vector.
 */
void col_block_sgemv(const std::string &mat, const std::string &vec, int p, int id)
{
    float *A, *b;

    dim2 dim = read_col_matrix(mat, &A, MPI_COMM_WORLD);
    int n = read_block_vector(vec, &b, MPI_COMM_WORLD);

    if (dim.second != n) {
        if (!id)
            std::cerr << "Error: Mismatched column and vector dimension.\n" << "Matrix dim = "
                << dim.first << " x " << dim.second << " Vector dim = " << n << '\n';
        delete[] A;
        delete[] b;
        return;
    } // Check if dimensions are the same

//    print_col_matrix(A, dim, MPI_COMM_WORLD);
//    print_block_vector(b, n, MPI_COMM_WORLD);

    // SGEMV Implamentation
    int local_cols = block_size(id, p, n);
    float *partial_c = new float[dim.first];

    // Compute partial results. We will dot every row in our portion of A with the corresponding
    // block b.
    for (int i = 0; i < dim.first; i++) {
        partial_c[i] = 0.0;
        for (int j = 0; j < local_cols; j++)
            partial_c[i] += A[i * local_cols + j] * b[j];
    } // Loop over rows

    // In order to get the blocks for c, we need to do an all to all with partial_c so that
    // the correct procs get every partial_c element to sum.
    std::vector<int> cnt_out, disp_out, cnt_in, disp_in;
    float *partial_c_blk = new float[p * local_cols];
    float *c = new float[local_cols];

    make_mixed_xfer_array(p, dim.first, cnt_out, disp_out);
    make_uniform_xfer_array(id, p, dim.first, cnt_in, disp_in);
    MPI_Alltoallv(partial_c, cnt_out.data(), disp_out.data(), MPI_FLOAT, partial_c_blk,
            cnt_in.data(), disp_in.data(), MPI_FLOAT, MPI_COMM_WORLD);

    for (int i = 0; i < local_cols; i++) {
        c[i] = 0.0;
        for (int j = 0; j < p; j++)
            c[i] += partial_c_blk[i + j * local_cols];
    } // Loop over elements in c

    print_block_vector(c, dim.first, MPI_COMM_WORLD);

    delete[] A;
    delete[] b;
    delete[] partial_c;
    delete[] partial_c_blk;
    delete[] c;
}
