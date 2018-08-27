#ifndef MPI_VECTOR_H
#define MPI_VECTOR_H

#include <vector>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>


namespace psrs {


/* class: mpi_vector
 *
 * @Template T = type
 * Wrapper around std::vector to use for MPI programs.
 */
template <typename T>
class mpi_vector
{
    public:
        // Typedef
        typedef typename std::vector<T>::value_type      value_type;
        typedef typename std::vector<T>::size_type       size_type;
        typedef typename std::vector<T>::difference_type difference_type;
        typedef typename std::vector<T>::reference       reference;
        typedef typename std::vector<T>::const_reference const_reference;
        typedef typename std::vector<T>::iterator        iterator;
        typedef typename std::vector<T>::const_iterator  const_iterator;

        // Constructor / assiangment
        mpi_vector();
        mpi_vector(const boost::mpi::communicator &c);
        mpi_vector(const mpi_vector &other);
        mpi_vector<T>& operator=(const mpi_vector &rhs);

        // Communication
        template <typename InputIt>
            void distribute(InputIt begin, InputIt end, int root_process);
        void gather(int root_process);

        // Modify
        void clear() noexcept;
        template <typename InputIt>
            iterator insert(const_iterator pos, InputIt begin, InputIt end);

        // Element / data access / communicator access
        reference operator[](size_type pos);
        const_reference operator[](size_type pos) const;
        T* data() noexcept;
        const T* data() const noexcept;
        std::vector<T> get_vector() noexcept;
        const std::vector<T> get_vector() const noexcept;
        const boost::mpi::communicator get_comm() const noexcept;

        // Capacity
        bool empty()const noexcept;
        size_type total_size(int root_proc) const;
        size_type total_size() const;
        size_type size() const noexcept;
        void reserve(size_type new_size);

        // Iterators
        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        const_iterator cbegin() const noexcept;
        iterator end() noexcept;
        const_iterator end() const noexcept;
        const_iterator cend() const noexcept;

    private:
        boost::mpi::communicator comm;
        std::vector<T> arr;
};


}; // Namespace psrs

#include "mpi_vector.cpp"


#endif

