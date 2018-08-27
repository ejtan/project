#include <iterator>
#include <numeric>
#include <functional>
#include <boost/mpi/collectives.hpp>

#include "mpi_vector.h"
#include "mpi_utility.h"


namespace psrs {


/* mpi_vector::mpi_vector() constructor
 *
 * Constructs object with default communicator from boost::mpi::communicator (MPI_COMM_WORLD)
 */
template <typename T>
mpi_vector<T>::mpi_vector()
{
}


/* mpi_vector::mpi_vector() constructor
 * @INPUT: c = communciator to copy
 */
template <typename T>
mpi_vector<T>::mpi_vector(const boost::mpi::communicator &c) : comm(c)
{
}


/* mpi_vector& operator=
 * @INPUT: rhs = vector to copy into this object.
 */
template <typename T>
mpi_vector<T>& mpi_vector<T>::operator=(const mpi_vector<T> &rhs)
{
    // Quick return if rhs is the same object as lhs
    if (this == &rhs)
        return *this;

    comm = rhs.comm;
    arr = rhs.arr;

    return *this;
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
    // Special case: 1 proc
    if (comm.size() == 1) {
        arr.insert(arr.begin(), begin, end);
        return;
    }

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


/* mpi_vector::gather()
 * @INPUT: root_process = process to gather all data to
 *
 * Gathers all vectors spread out amoung procs to root_process.
 */
template <typename T>
void mpi_vector<T>::gather(int root_process)
{
    // Special case: 1 processor
    if (comm.size() == 1)
        return;

    std::vector<int> sizes, disp;
    int local_n = arr.size();

    // Gather each local processor's vector size to root_process
    boost::mpi::gather(comm, local_n, sizes, root_process);

    // Generate displacement and count vectors for mpi::gatherv
    if (comm.rank() == root_process) {
        // First displacement is always 0.
        disp.push_back(0);
        for (int i = 0; i < sizes.size() - 1; ++i)
            disp.push_back(disp[i] + sizes[i]);
    }

    // Create temp vector to gather data into
    int n = std::accumulate(sizes.begin(), sizes.end(), 0);
    std::vector<T> tmp(n);

    boost::mpi::gatherv(comm, arr, tmp.data(), sizes, disp, root_process);

    // Move tmp vector into arr on root_process and delete arr on other procs
    if (comm.rank() == root_process)
        arr = std::move(tmp);
    else
        arr.clear();
}


/* clear()
 * Clears proc's vector
 */
template <typename T>
void mpi_vector<T>::clear() noexcept
{
    arr.clear();
}


/* insert()
 * @INPUT: pos = iterator position to insert
 * @INPUT: begin, end = range of iterators to insert
 */
template <typename T>
template <typename InputIt>
typename mpi_vector<T>::iterator mpi_vector<T>::insert(const_iterator pos,
        InputIt begin, InputIt end)
{
    return arr.insert(pos, begin, end);
}


/* reference operator[]
 * @INPUT: pos = index
 *
 * Returns a reference to the position in the vector
 */
template <typename T>
typename mpi_vector<T>::reference mpi_vector<T>::operator[](size_type pos)
{
    return arr[pos];
}


/* const_reference operator[]
 * @INPUT: pos = index
 *
 * Returns a const reference to the position in the vector
 */
template <typename T>
typename mpi_vector<T>::const_reference mpi_vector<T>::operator[](size_type pos) const
{
    return arr[pos];
}


/* T* data()
 * Returns a pointer to the underlying data.
 */
template <typename T>
T* mpi_vector<T>::data() noexcept
{
    return arr.data();
}


/* const T* data()
 * Returns a const pointer to the underlying data.
 */
template <typename T>
const T* mpi_vector<T>::data() const noexcept
{
    return arr.data();
}


/* std::vector<T> get_vector()
 * Returns the vector arr
 */
template <typename T>
std::vector<T> mpi_vector<T>::get_vector() noexcept
{
    return arr;
}


/* std::vector<T> get_vector()
 * Returns const vector
 */
template <typename T>
const std::vector<T> mpi_vector<T>::get_vector() const noexcept
{
    return arr;
}


/* bool empty()
 * Returns true if empty and false otherwise
 */
template <typename T>
bool mpi_vector<T>::empty() const noexcept
{
    return arr.empty();
}


/* size_type total_size()
 * @INPUT: root_proc = processor to get total size
 */
template <typename T>
typename mpi_vector<T>::size_type mpi_vector<T>::total_size(int root_proc) const
{
    int total_n;
    int n = arr.size();
    boost::mpi::reduce(comm, n, total_n, std::plus<int>(), root_proc);

    return total_n;
}


/* size_type total_size()
 * Returns total size to all processors
 */
template <typename T>
typename mpi_vector<T>::size_type mpi_vector<T>::total_size() const
{
    int total_n;
    int n = arr.size();
    boost::mpi::all_reduce(comm, n, total_n, std::plus<int>());

    return total_n;
}


/* size_type size()
 * Returns the size of the proc's vector
 */
template <typename T>
typename mpi_vector<T>::size_type mpi_vector<T>::size() const noexcept
{
    return arr.size();
}


/* reserve()
 * @INPUT: new_size = numer of elements to reserve
 */
template <typename T>
void mpi_vector<T>::reserve(size_type new_size)
{
    arr.reserve(new_size);
}


/* begin()
 * Returns begin iterator
 */
template <typename T>
typename mpi_vector<T>::iterator mpi_vector<T>::begin() noexcept
{
    return arr.begin();
}


/* begin()
 * Returns begin const iterator
 */
template <typename T>
typename mpi_vector<T>::const_iterator mpi_vector<T>::begin() const noexcept
{
    return arr.cbegin();
}


/* cbegin()
 * Returns begin const iterator
 */
template <typename T>
typename mpi_vector<T>::const_iterator mpi_vector<T>::cbegin() const noexcept
{
    return arr.cbegin();
}


/* end()
 * Returns end iterator
 */
template <typename T>
typename mpi_vector<T>::iterator mpi_vector<T>::end() noexcept
{
    return arr.end();
}

/* end()
 * Returns end const iterator
 */
template <typename T>
typename mpi_vector<T>::const_iterator mpi_vector<T>::end() const noexcept
{
    return arr.cend();
}


/* cend()
 * Returns end const iterator
 */
template <typename T>
typename mpi_vector<T>::const_iterator mpi_vector<T>::cend() const noexcept
{
    return arr.cend();
}


}; // namespace psrs
