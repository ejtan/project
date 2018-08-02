#include <iterator>
#include <boost/mpi/collectives.hpp>

#include "mpi_vector.h"
#include "mpi_utility.h"


namespace psrs {


/* mpi_vector::mpi_vector() constructor
 * @INPUT: c = communciator to copy
 */
template <typename T>
mpi_vector<T>::mpi_vector(const boost::mpi::communicator &c) : comm(c)
{
}


/* void mpi_vector::distribute
 * @INPUT: begin, end = pair of iterators representing the range to copy from
 * @INPUT root_process = process containing to pair of iterators
 *
 * Distributes data from [begin, end) into each process's local vector. The data is distributed as
 * evenly as possiable.
 */
template <typename T>
template <typename InputIt>
void mpi_vector<T>::distribute(InputIt begin, InputIt end, int root_process)
{
    int n;

    // Compute the number of elements and distribute to all procs
    if (comm.rank() == root_process)
        n = static_cast<int>(std::distance(begin, end));

    boost::mpi::broadcast(comm, n, root_process);

    if (comm.rank() == root_process) {
        int p = comm.size();

        // Create temperory buffer. Largest blk_size occurs on the last processor
        // so we will use that size to ensure the buffer has enough elements, but won't waste memory
        std::vector<T> tmp(psrs::utility::blk_size(p - 1, p, n));

        // Track number of elements need to skip because we already processed them
        int skip = 0;

        for (int i = 0; i < comm.size(); ++i) {
            int send_size = psrs::utility::blk_size(i, p, n);

            if (i == root_process) {
                if (!arr.empty())
                    arr.clear();
                arr.insert(arr.begin(), begin + skip, begin + skip + send_size);
            } else {
                tmp.clear();
                tmp.insert(tmp.begin(), begin + skip, begin + skip + send_size);
                comm.send(i, psrs::utility::data_tag, tmp);
            } // Copy to arr if i == root_rpocess, else send to proc i
            skip += send_size;
        } // Loop over process
    } else {
        comm.recv(root_process, psrs::utility::data_tag, arr);
    } // root_process distributes process
}


}; // namespace psrs
