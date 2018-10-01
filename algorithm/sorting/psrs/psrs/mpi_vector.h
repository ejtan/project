#ifndef MPI_VECTOR_H
#define MPI_VECTOR_H

#include <vector>
#include <optional>

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
        typedef typename std::vector<T>::value_type             value_type;
        typedef typename std::vector<T>::size_type              size_type;
        typedef typename std::vector<T>::difference_type        difference_type;
        typedef typename std::vector<T>::reference              reference;
        typedef typename std::vector<T>::const_reference        const_reference;
        typedef typename std::vector<T>::iterator               iterator;
        typedef typename std::vector<T>::const_iterator         const_iterator;
        typedef typename std::vector<T>::reverse_iterator       reverse_iterator;
        typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;

        // Constructor / assiangment
        mpi_vector();
        mpi_vector(const boost::mpi::communicator &c);
        template <typename InputIt>
            mpi_vector(const boost::mpi::communicator &c, InputIt begin, InputIt end,
                    int root_process);
        mpi_vector(const mpi_vector &other);
        mpi_vector<T>& operator=(const mpi_vector &rhs);

        void assign(size_type count, const T &val);
        template <typename InputIt>
            void assign(InputIt begin, InputIt end);
        void assign(std::initializer_list<T> lst);

        // Communication
        template <typename InputIt>
            void distribute(InputIt begin, InputIt end, int root_process);
        void gather(int root_process);
        void scatter();

        // Modify
        void clear() noexcept;
        iterator insert(const_iterator pos, const T &val);
        iterator insert(const_iterator pos, T &&val);
        iterator insert(const iterator pos, size_type n, const T &val);
        template <typename InputIt>
            iterator insert(const_iterator pos, InputIt begin, InputIt end);
        iterator insert(const_iterator pos, std::initializer_list<T> lst);
        template <typename ...Args>
            iterator emplace(const_iterator pos, Args &&...args);
        iterator erase(const_iterator pos);
        iterator erase(const_iterator begin, const_iterator end);
        void push_back(const T& val);
        void push_back(T &&val);
        template <typename ...Args>
            reference emplace_back(Args &&...args);
        void pop_back();
        void resize(size_type count);
        void resize(size_type count, const T &val);
        void swap_vector(std::vector<T> &other);

        // Element / data access / communicator access
        reference at(size_type pos);
        const_reference at(size_type pos) const;
        reference operator[](size_type pos);
        const_reference operator[](size_type pos) const;
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;
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
        size_type max_size() const noexcept;
        void reserve(size_type new_size);
        size_type capacity() const noexcept;
        void shrink_to_fit();

        // Iterators
        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        const_iterator cbegin() const noexcept;
        iterator end() noexcept;
        const_iterator end() const noexcept;
        const_iterator cend() const noexcept;
        reverse_iterator rbegin() noexcept;
        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator crbegin() const noexcept;
        reverse_iterator rend() noexcept;
        const_reverse_iterator rend() const noexcept;
        const_reverse_iterator crend() const noexcept;

        // Other
        void read_file(const std::string &filepath);
        void output(std::ostream &os, int root_process, const std::string &delim=" ") const;

    private:
        boost::mpi::communicator comm;
        std::vector<T> arr;
        std::optional<int> is_gathered;
};


}; // Namespace psrs

#include "mpi_vector.cpp"


#endif

