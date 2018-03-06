#include <iostream>
#include <fstream>
#include <iomanip>

#include "mpi_utility.h"
#include "vector.h"


/*-------------------------------------------------------------------------------------------------
 * INPUT FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* read_block_vector()
 *
 * @param: filename = file to open
 * @param: v = pointer to array (unallocated)
 * @param: comm = MPI communicator
 *
 * @return: size of the vector
 * @return: block vector through v
 *
 * Reads in a file and distributes a vector in blocks amoung a MPI communicator.
 * Proc p - 1 handles reading and distributing files
 */
int read_block_vector(const std::string &filename, float **v, MPI_Comm comm)
{
    std::ifstream inf(filename);
    int p, id;
    int n;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &id);

    if (!inf.is_open())
        MPI_Abort(comm, OPEN_FILE_ERROR);

    if (id == (p - 1))
        inf >> n;

    MPI_Bcast(&n, 1, MPI_INT, p - 1, comm);

    // Allocate proc's memory
    int local_eles = block_size(id, p, n);
    *v = new float[local_eles];

    if (id == (p - 1)) {
        for (int i = 0; i < p - 1; i++) {
            int send_size = block_size(i, p, n);

            // Read block
            for (int j = 0; j < send_size; j++)
                inf >> (*v)[j];

            MPI_Send(*v, send_size, MPI_FLOAT, i, DATA_MSG, comm);
        } // Loop over procs

        // Read proc p-1's block
        for (int i = 0; i < local_eles; i++)
            inf >> (*v)[i];
    } else {
        MPI_Status stat;
        MPI_Recv(*v, local_eles, MPI_FLOAT, p - 1, DATA_MSG, comm, &stat);
    } // Proc p-1 reads blocks and sends to corresponding processors

    inf.close();

    return n;
}


/* read_replicated_vector()
 *
 * @param: filename = file to open
 * @param: v = pointer to array (unallocated)
 * @param: comm = MPI communicator
 *
 * @return: size of the vector
 * @return: replicated vector through v
 *
 * Reads in a vector and distributes to all processes so each proc has the entire vector.
 */
int read_replicated_vector(const std::string &filename, float **v, MPI_Comm comm)
{
    std::ifstream inf(filename);
    int id, p;
    int n;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &id);

    if (!inf.is_open())
        MPI_Abort(comm, OPEN_FILE_ERROR);

    if (id == (p - 1))
        inf >> n;

    MPI_Bcast(&n, 1, MPI_INT, p - 1, comm);

    *v = new float[n];

    // Proc p-1 reads the vector, then broadcast
    if (id == (p - 1))
        for (int i = 0; i < n; i++)
            inf >> (*v)[i];

    MPI_Bcast(*v, n, MPI_FLOAT, p - 1, comm);

    inf.close();

    return n;
}


/*-------------------------------------------------------------------------------------------------
 * OUTPUT FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* read_block_vector()
 *
 * @param: v = input array
 * @param: n = number of elements to print in input array
 * @param: comm = MPI communicator
 *
 * Prints out a block vector. Collects blocks into proc 0 and prints.
 */
void print_block_vector(const float *v, int n, MPI_Comm comm)
{
    MPI_Status stat;
    int id, p;
    int dummy; // dummy message to get blocks in order

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &id);

    int local_eles = block_size(id, p, n);

    if (!id) {
        print_subvector(v, local_eles);

        if (p > 1) {
            int max_buffer = block_size(p - 1, p, n);
            std::vector<float> buffer(max_buffer);

            for (int i = 1; i < p; i++) {
                int recv_size = block_size(i, p, n);
                MPI_Send(&dummy, 1, MPI_INT, i, PROMPT_MSG, comm);
                MPI_Recv(buffer.data(), recv_size, MPI_FLOAT, i, DATA_MSG, comm, &stat);
                print_subvector(buffer.data(), recv_size);
            } // Loop over all procs
        } // Check if there are more than 1 process

        std::cout << std::endl;
    } else {
        MPI_Recv(&dummy, 1, MPI_INT, 0, PROMPT_MSG, comm, &stat);
        MPI_Send(v, local_eles, MPI_FLOAT, 0, DATA_MSG, comm);
    } // Print vector on proc 0, send blocks on other procs.
}


/* print_replicated_vector()
 *
 * @param: v = input array
 * @param: n = number of elements to print in input array
 * @param: comm = MPI communicator
 *
 * Prints replicated vector (proc 0 prints the vector).
 */
void print_replicated_vector(const float *v, int n, MPI_Comm comm)
{
    int id;

    MPI_Comm_rank(comm, &id);

    if (!id) {
        print_subvector(v, n);
        std::cout << std::endl;
    } // Proc 0 prints vector
}


/* print_subvector()
 *
 * @param: v = input array
 * @param: n = number of elements to print in input array
 *
 * Outputs array.
 */
void print_subvector(const float *v, int n)
{
    for (int i = 0; i < n; i++)
        std::cout << std::setw(6) << std::setprecision(4) << std::right << v[i] << ' ';
}


/*-------------------------------------------------------------------------------------------------
 * MISC. FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* replicate_block_vector()
 *
 * @param: v_block = block vectors to replicate
 * @param: n = size of vector
 * @param: v_rep = replicated vector
 * @param: comm = MPI Communicator
 *
 * @return: replicated vector through v_rep
 *
 * Combines blocks among procs into a replicated vectors. Returns through v_rep.
 */
void replicate_block_vector(const float *v_block, int n, float **v_rep, MPI_Comm comm)
{
    int id, p;
    std::vector<int> cnt, disp;

    *v_rep = new float[n];

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &id);
    make_mixed_xfer_array(p, n, cnt, disp);
    MPI_Allgatherv(v_block, cnt[id], MPI_FLOAT, *v_rep, cnt.data(), disp.data(), MPI_FLOAT, comm);
}
