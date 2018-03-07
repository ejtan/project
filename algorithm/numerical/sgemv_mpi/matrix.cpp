#include <iostream>
#include <fstream>
#include <iomanip>

#include "matrix.h"
#include "vector.h"
#include "mpi_utility.h"


/*-------------------------------------------------------------------------------------------------
 * INPUT FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* read_row_matrix()
 *
 * @param: filename = input filename
 * @param: A = point to matrix (as array)
 * @param: comm = MPI communicator
 *
 * @return: dimension of matrix
 * @return: matrix returned through A
 *
 * Reads in and decomposes a matrix by rows. Proc p-1 handles reading and distributing matrix.
 */
dim2 read_row_matrix(const std::string &filename, float **A, MPI_Comm comm)
{
    std::ifstream inf(filename);
    int p, id;
    int m, n;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &id);

    if (!inf.is_open())
        MPI_Abort(comm, OPEN_FILE_ERROR);

    if (id == (p - 1))
        inf >> m >> n;

    // Broadcast m and n
    MPI_Bcast(&m, 1, MPI_INT, p - 1, comm);
    MPI_Bcast(&n, 1, MPI_INT, p - 1, comm);

    // Allocate buffer
    int local_rows = block_size(id, p, m);
    *A = new float[local_rows * n];

    if (id == (p - 1)) {
        for (int i = 0; i < p - 1; i++) {
            int send_size = block_size(i, p, m) * n;
            for (int j = 0; j < send_size; j++)
                inf >> (*A)[j];

            MPI_Send(*A, send_size, MPI_FLOAT, i, DATA_MSG, comm);
        } // Reads and distributes matrix to all procs except p-1

        // Proc p-1 read its peice
        for (int i = 0; i < local_rows * n; i++)
            inf >> (*A)[i];

    } else {
        MPI_Status stat;
        MPI_Recv(*A, local_rows * n, MPI_FLOAT, p - 1, DATA_MSG, comm, &stat);
    } // Distribute matrix rows

    inf.close();

    return std::make_pair(m, n);
}


/* read_col_matrix()
 *
 * @param: filename = input filename
 * @param: A = point to matrix (as array)
 * @param: comm = MPI communicator
 *
 * @return: dimension of matrix
 * @return: matrix returned through A
 *
 * Reads in and decomposes a matrix by cols. Proc p-1 handles reading the matrix and elements are
 * distributed through scatterv.
 */
dim2 read_col_matrix(const std::string &filename, float **A, MPI_Comm comm)
{
    std::ifstream inf(filename);
    int id, p;
    int m, n;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &id);

    if (!inf.is_open())
        MPI_Abort(comm, OPEN_FILE_ERROR);

    if (id == (p - 1))
        inf >> m >> n;

    // Broadcast m and n
    MPI_Bcast(&m, 1, MPI_INT, p - 1, comm);
    MPI_Bcast(&n, 1, MPI_INT, p - 1, comm);

    // Allocate buffer
    int local_cols = block_size(id, p, n);
    *A = new float[m * local_cols];

    // create arrays for transfering rows
    std::vector<int> cnt, disp;
    make_mixed_xfer_array(p, n, cnt, disp);

    // Use a buffer for reading rows
    std::vector<float> buffer(n);

    for (int i = 0; i < m; i++) {
        // Read into buffer
        if (id == (p - 1))
            for (int j = 0; j < n; j++)
                inf >> buffer[j];

        // Distribute row to corresponding columns
        MPI_Scatterv(buffer.data(), cnt.data(), disp.data(), MPI_FLOAT, (*A) + (i * local_cols),
                local_cols, MPI_FLOAT, p - 1, comm);
    } // Loop over all rows

    inf.close();

    return std::make_pair(m, n);
}


/*-------------------------------------------------------------------------------------------------
 * OUTPUT FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* print_row_matrix()
 *
 * @param: A = matrix to output
 * @param: dim = dimensions of the matrix
 * @param: comm = MPI Communicator
 *
 * Prints a row decomposed matrix. Proc 0 recieves the submatrix from other processes in order.
 */
void print_row_matrix(const float *A, const dim2 &dim, MPI_Comm comm)
{
    int p, id;
    int prompt; // Dummy message to get matrix elements in order
    MPI_Status stat;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &id);

    int local_rows = block_size(id, p, dim.first);

    if (!id) {
        print_submatrix(A, local_rows, dim.second);

        if (p > 1) {
            int max_blk_size = block_size(p - 1, p, dim.first);
            std::vector<float> buffer(max_blk_size * dim.second);

            for (int i = 1; i < p; i++) {
                int recv_blks = block_size(i, p, dim.first);

                MPI_Send(&prompt, 1, MPI_INT, i, PROMPT_MSG, comm);
                MPI_Recv(buffer.data(), recv_blks * dim.second, MPI_FLOAT, i, DATA_MSG, comm, &stat);
                print_submatrix(buffer.data(), recv_blks, dim.second);
            } // Recv submatrices and output
        } // Recv submatrices if there is more than 1 proc

        std::cout << std::endl;
    } else {
        MPI_Recv(&prompt, 1, MPI_INT, 0, PROMPT_MSG, comm, &stat);
        MPI_Send(A, local_rows * dim.second, MPI_FLOAT, 0, DATA_MSG, comm);
    } // Gather submatrix to proc 0 and output
}


/* print_col_matrix()
 *
 * @param: A = matrix to output
 * @param: dim = dimensions of the matrix
 * @param: comm = MPI Communicator
 *
 * Prints a col decomposed matrix. Gathers rows into a buffer so that the buffer stores an entire
 * row of a matrix.
 */
void print_col_matrix(const float *A, const dim2 &dim, MPI_Comm comm)
{
    int id, p;
    std::vector<int> cnt, disp;

    MPI_Comm_rank(comm, &id);
    MPI_Comm_size(comm, &p);
    make_mixed_xfer_array(p, dim.second, cnt, disp);

    int local_cols = block_size(id, p, dim.second);
    std::vector<float> buffer(dim.second);

    for (int i = 0; i < dim.first; i++) {
        MPI_Gatherv(A + (i * local_cols), block_size(id, p, dim.second), MPI_FLOAT, buffer.data(),
                    cnt.data(), disp.data(), MPI_FLOAT, 0, comm);

        if (!id) {
            print_subvector(buffer.data(), dim.second);
            std::cout << '\n';
        } // Proc 0 prints the subvector corresponding to rows
    } // Loop over rows

    if (!id)
        std::cout << std::endl;
}


/* print_submatrix()
 *
 * @param: A = matrix to output
 * @param: m = number of rows
 * @param: n = number of cols
 *
 * Output a m x n matrix
 */
void print_submatrix(const float *A, int m, int n)
{
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << std::right << std::setw(6) << std::setprecision(4) << A[i * n + j] << ' ';
        } // Col loop
        std::cout << '\n';
    } // Row loop
}
