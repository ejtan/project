#include <fstream>
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


/* mpi_vector::mpi_vector() constructor
 * @INPUT: c = communicator to copy
 * @INPUT: begin, end = pair of iterators to construct
 * @INPUT: root_process = proc where data is from
 *
 * Constructs and distributes data. Calls mpi_vector::distribute().
 */
template <typename T>
template <typename InputIt>
mpi_vector<T>::mpi_vector(const boost::mpi::communicator &c, InputIt begin,
        InputIt end, int root_process) : comm(c)
{
    distribute(begin, end, root_process);
}


/* mpi_vector::mpi_vector() copy constructor
 * @INPUT: other = mpi_vector to copy
 */
template <typename T>
mpi_vector<T>::mpi_vector(const mpi_vector &other) : comm(other.comm), arr(other.arr)
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
    is_gathered = rhs.is_gathered;

    return *this;
}


/* operator=
 * @INPUT: rhs = object to move
 */
template <typename T>
mpi_vector<T>& mpi_vector<T>::operator=(mpi_vector<T> &&rhs)
{
    if (this == &rhs)
        return *this;

    // Note: Communicator is copied since opve assiangment operator is not supported
    comm = rhs.comm;
    arr = std::move(rhs.arr);
    is_gathered = std::move(rhs.is_gathered);

    return *this;
}


/* operator=
 * @INPUT: lst = initalizer list
 */
template <typename T>
mpi_vector<T>& mpi_vector<T>::operator=(std::initializer_list<T> lst)
{
    // Note: comm will be default initalized
    arr = lst;

    return *this;
}


/* assign()
 * @INPUT: count = number of elements to assign
 * @INPUT: val = value to copy
 */
template <typename T>
void mpi_vector<T>::assign(size_type count, const T &val)
{
    arr.assign(count, val);
}


/* assign()
 * @INPUT: begin, end = pair of iterators to assign
 */
template <typename T>
template <typename InputIt>
void mpi_vector<T>::assign(InputIt begin, InputIt end)
{
    arr.assign(begin, end);
}


/* assign()
 * @INPUT: lst = initalizer list
 */
template <typename T>
void mpi_vector<T>::assign(std::initializer_list<T> lst)
{
    arr.assign(lst);
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

    // Check if data is gathered on on processor
    if (is_gathered)
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

    // Set is_gathered to the proc which all data resides in
    is_gathered = root_process;
}


/* scatter()
 * Gathers all data into proc 0 and scatters evenlly among all processors. Makes no
 * assumption about how the data is distributed, so all data is gathered into one
 * proc and distributed.
 */
template <typename T>
void mpi_vector<T>::scatter()
{
    int root_proc;

    if (is_gathered) {
        root_proc = is_gathered.value();
    } else {
        root_proc = 0;
        gather(0);
    }

    int p = comm.size();
    int id = comm.rank();
    int n = total_size();
    std::vector<int> send_count(p), send_disp(p);

    send_count[0] = utility::blk_size(0, p, n);
    send_disp[0] = 0;
    for (int i = 1; i < p; i++) {
        send_disp[i] = send_disp[i - 1] + send_count[i - 1];
        send_count[i] = utility::blk_size(i, p, n);
    }

    int local_n = utility::blk_size(id, p, n);

    T *tmp = new T[local_n];

    boost::mpi::scatterv(comm, arr, send_count, send_disp, tmp, local_n, root_proc);
    arr.clear();
    arr.insert(arr.begin(), tmp, tmp + local_n);

    delete[] tmp;
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
 * @INPUT: val = item to copy
 */
template <typename T>
typename mpi_vector<T>::iterator mpi_vector<T>::insert(const_iterator pos, const T &val)
{
    return arr.insert(pos, val);
}

/* insert()
 * @INPUT: pos = iterator position to insert
 * @INPUT: val = item to move
 */
template <typename T>
typename mpi_vector<T>::iterator mpi_vector<T>::insert(const_iterator pos, T &&val)
{
    return arr.insert(pos, std::move(val));
}


/* insert()
 * @INPUT: pos = iterator position to insert
 * @INPUT: n = number of items to insert
 * @INPUT: val = item to copy
 */
template <typename T>
typename mpi_vector<T>::iterator mpi_vector<T>::insert(const iterator pos, size_type n, const T &val)
{
    return arr.insert(pos, n, val);
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


/* insert()
 * @INPUT: pos = iterator position to insert
 * @INPUT: lst = initializer_list
 */
template <typename T>
typename mpi_vector<T>::iterator mpi_vector<T>::insert(const_iterator pos,
        std::initializer_list<T> lst)
{
    return arr.insert(pos, lst);
}


/* emplace()
 * @INPUT: pos = iterator position to insert
 * @INPUT: args = arguments to insert
 */
template <typename T>
template <typename ...Args>
typename mpi_vector<T>::iterator mpi_vector<T>::emplace(const_iterator pos, Args &&...args)
{
    return arr.insert(pos, std::forward(args)...);
}


/* erase()
 * @INPUT: pos = iterator to position to erase
 */
template <typename T>
typename mpi_vector<T>::iterator mpi_vector<T>::erase(const_iterator pos)
{
    return arr.erase(pos);
}


/* erase()
 * @INPUT: begin = begin iterator
 * @INPUT: end = end iterator
 */
template <typename T>
typename mpi_vector<T>::iterator mpi_vector<T>::erase(const_iterator begin, const_iterator end)
{
    return arr.erase(begin, end);
}


/* push_back()
 * @INPUT: val = value to add to the end
 */
template <typename T>
void mpi_vector<T>::push_back(const T &val)
{
    arr.push_back(val);
}


/* push_back()
 * @INPUT: val = value to move to the end
 */
template <typename T>
void mpi_vector<T>::push_back(T &&val)
{
    arr.push_back(std::move(val));
}


/* emplace_back()
 * @INPUT: args = elements to append to vector
 */
template <typename T>
template <typename ...Args>
typename mpi_vector<T>::reference mpi_vector<T>::emplace_back(Args &&...args)
{
    return arr.emplace_back(std::forward(args)...);
}


/* pop_back()
 */
template <typename T>
void mpi_vector<T>::pop_back()
{
    arr.pop_back();
}


/* resize()
 * @INPUT: count = new size
 */
template <typename T>
void mpi_vector<T>::resize(size_type count)
{
    arr.resize(count);
}


/* resize()
 * @INPUT: count = new size
 * @INPUT: val = value of all elements
 */
template <typename T>
void mpi_vector<T>::resize(size_type count, const T &val)
{
    arr.resize(count, val);
}


/* swap_vector()
 * @INPUT: other = vector to swap data with
 *
 * Swaps internal vector.
 */
template <typename T>
void mpi_vector<T>::swap_vector(std::vector<T> &other)
{
    arr.swap(other);
}


/* at()
 * @INPUT: pos = position
 * Returns a reference to the element at arr[pos];
 */
template <typename T>
typename mpi_vector<T>::reference mpi_vector<T>::at(size_type pos)
{
    return arr.at(pos);
}


/* at()
 * @INPUT: pos = position
 * Returns a const reference to the element at arr[pos];
 */
template <typename T>
typename mpi_vector<T>::const_reference mpi_vector<T>::at(size_type pos) const
{
    return arr.at(pos);
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


/* front()
 * Returns a reference to the front of the vector
 */
template <typename T>
typename mpi_vector<T>::reference mpi_vector<T>::front()
{
    return arr.front();
}


/* front()
 * Returns a const reference to the front of the vector
 */
template <typename T>
typename mpi_vector<T>::const_reference mpi_vector<T>::front() const
{
    return arr.front();
}


/* back()
 * Returns a reference to the back of the vector
 */
template <typename T>
typename mpi_vector<T>::reference mpi_vector<T>::back()
{
    return arr.back();
}


/* back()
 * Returns a const reference to the back of the vector
 */
template <typename T>
typename mpi_vector<T>::const_reference mpi_vector<T>::back() const
{
    return arr.back();
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


/* boost::mpi::communicator get_comm()
 * Returns const boost::mpi::communicator
 */
template <typename T>
const boost::mpi::communicator mpi_vector<T>::get_comm() const noexcept
{
    return comm;
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


/* max_size()
 * Returns the max size allowable on each processor.
 */
template <typename T>
typename mpi_vector<T>::size_type mpi_vector<T>::max_size() const noexcept
{
    return arr.max_size();
}


/* reserve()
 * @INPUT: new_size = numer of elements to reserve
 */
template <typename T>
void mpi_vector<T>::reserve(size_type new_size)
{
    arr.reserve(new_size);
}


/* capacity()
 * Returns the number of elements allocated
 */
template <typename T>
typename mpi_vector<T>::size_type mpi_vector<T>::capacity() const noexcept
{
    return arr.capacity();
}


/* shirnk_to_fit()
 * Shrinks vector to match its size
 */
template <typename T>
void mpi_vector<T>::shrink_to_fit()
{
    arr.shrink_to_fit();
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


/* rbegin()
 * Returns begin reverse iterator
 */
template <typename T>
typename mpi_vector<T>::reverse_iterator mpi_vector<T>::rbegin() noexcept
{
    return arr.rbegin();
}


/* rbegin()
 * Returns begin const reverse iterator
 */
template <typename T>
typename mpi_vector<T>::const_reverse_iterator mpi_vector<T>::rbegin() const noexcept
{
    return arr.crbegin();
}


/* crbegin()
 * Returns begin const reverse iterator
 */
template <typename T>
typename mpi_vector<T>::const_reverse_iterator mpi_vector<T>::crbegin() const noexcept
{
    return arr.crbegin();
}


/* rend()
 * Returns end reverse iterator
 */
template <typename T>
typename mpi_vector<T>::reverse_iterator mpi_vector<T>::rend() noexcept
{
    return arr.rend();
}


/* rend()
 * Returns end const reverse iterator
 */
template <typename T>
typename mpi_vector<T>::const_reverse_iterator mpi_vector<T>::rend() const noexcept
{
    return arr.crend();
}


/* crend()
 * Returns end const reverse iterator
 */
template <typename T>
typename mpi_vector<T>::const_reverse_iterator mpi_vector<T>::crend() const noexcept
{
    return arr.crend();
}


/* read_file()
 * @INPUT: filepath = path to file
 *
 * Opens a file, Reads its contents, and distributes it amoung each processor.
 */
template <typename T>
void mpi_vector<T>::read_file(const std::string &filepath)
{
    int p = comm.size();
    int id = comm.rank();
    int n = 0;
    std::ifstream in_file;

    if (id == (p-1)) {
        in_file.open(filepath);

        // Compute number of elements by going through the number of lines.
        std::string dummy_line;
        while (std::getline(in_file, dummy_line))
            n++;

        // Reset to beginning of file
        in_file.clear();
        in_file.seekg(0, std::ios::beg);
    } // Proc p-1 opens the file and gets line size

    // Broadcast n to all procs
    boost::mpi::broadcast(comm, n, p-1);

    int local_n = utility::blk_size(id, p, n);

    if (id == (p-1)) {
        // Resize proc p-1 as a buffer since it contains the largest possiable number of elements
        arr.resize(local_n);

        for (int i = 0; i < p; i++) {
            for (int j = 0; j < utility::blk_size(i, p, n); j++)
                in_file >> arr[j];

            if (i != p-1) {
                comm.send(i, utility::data_tag, arr);
            } // Send data if proc is not the last one
        } //Loop over procs
    } else {
        comm.recv(p-1, utility::data_tag, arr);

        // Resize to local_n to clear any un needed elements since each proc's arr is now the same
        // as arr on proc p-1
        arr.resize(local_n);
    } // Proc p-1 distriubtes data to all other processors

    if (in_file.is_open())
        in_file.close();
}


/* output()
 * @INPUT: os = ostream
 * @INPUT: root_process = process to output data
 * @INPUT: delim = delimiter between elements. Defaults to space
 *
 * root_process gathers each procs data, in order and outputs their contents to an ostream
 */
template <typename T>
void mpi_vector<T>::output(std::ostream &os, int root_process, const std::string &delim) const
{
    int p = comm.size();
    int id = comm.rank();
    int n = total_size();
    std::ostream_iterator<T> out_it(os, delim.c_str());

    if (p == 1) {
        std::copy(arr.begin(), arr.end(), out_it);
        return;
    } // Special case: 1 process

    if (id == root_process) {
        std::vector<T> buffer(utility::blk_size(p-1, p, n));

        for (int i = 0; i < p; i++) {
            if (i == root_process) {
                std::copy(arr.begin(), arr.end(), out_it);
            } else {
                comm.send(i, utility::prompt_tag);
                comm.recv(i, utility::data_tag, buffer);
                std::copy(buffer.begin(), buffer.end(), out_it);
            } // Send data to root_process, if necessary, and output
        } // Loop over all procs

        std::cout << std::endl;
    } else {
        comm.recv(root_process, utility::prompt_tag);
        comm.send(root_process, utility::data_tag, arr);
    } // root_process manages data movement and output which everyone else outputs data
}


}; // namespace psrs
