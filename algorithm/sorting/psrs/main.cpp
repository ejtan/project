#include <iostream>
#include <string>
#include <random>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <cctype>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/timer.hpp>

#include "psrs/mpi_vector.h"
#include "psrs/sort.h"


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 *-----------------------------------------------------------------------------------------------*/
void test_double(const boost::mpi::communicator &comm, int N);
void test_string(const boost::mpi::communicator &comm);
template <typename T>
double time_sort(psrs::mpi_vector<T> &v);
template <typename T, class Compare>
double time_sort(psrs::mpi_vector<T> &v, Compare cmp);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    // Get vector size (default 100)
    int N = (argc == 2) ? atoi(argv[1]) : 100;

    test_double(world, N);
    test_string(world);
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/
void test_double(const boost::mpi::communicator &comm, int N)
{
    std::vector<double> data;

    if (!comm.rank()) {
        std::random_device rd;
        std::mt19937 engine(rd());
        std::uniform_real_distribution<double> range(-100.0, 100.0);

        data.resize(N);
        std::generate(data.begin(), data.end(),
                [&range, &engine]()->double { return range(engine); });
    } // Generate data on proc 0

    psrs::mpi_vector<double> v(comm, data.begin(), data.end(), 0);

    psrs::mpi_vector<double> v_reverse(v);

    // Time sort with default compare
    double elapsed_time = time_sort(v);
    v.gather(0);

    // Time sort with std::greater compare
    double reverse_elapsed_time = time_sort(v_reverse, std::greater<double>());
    v_reverse.gather(0);

    if (!comm.rank()) {
        std::cout << "Test double:\n";
        std::cout << "Normal Sort time: " << elapsed_time << "s : ";
        std::cout << (std::is_sorted(v.begin(), v.end()) ? "Sorted correctly\n" : "Not sorted\n");
        std::cout << "Reverse Sort time: " << reverse_elapsed_time << "s : ";
        std::cout << (std::is_sorted(v_reverse.begin(), v_reverse.end(), std::greater<double>()) ?
                "Sorted correctly\n" : "Not sorted\n");
        std::cout << std::endl;
    }
}


void test_string(const boost::mpi::communicator &comm)
{
    psrs::mpi_vector<std::string> data(comm);

    data.read_file("/usr/share/dict/words");

    // Gather, randomly shuffle data, and redistribute
    data.gather(0);
    if (!comm.rank()) {
        std::random_device rd;
        std::mt19937 engine(rd());
        std::shuffle(data.begin(), data.end(), engine);
    }
    data.scatter();

    psrs::mpi_vector<std::string> reverse_data(data), case_insensitive_data(data);

    double elapsed_time = time_sort(data);
    data.gather(0);

    double reverse_elapsed_time = time_sort(reverse_data, std::greater<std::string>());
    reverse_data.gather(0);

    auto case_insensitive_cmp = [](const std::string &lhs, const std::string &rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                [](const char a, const char b) { return std::tolower(a) < std::tolower(b); });
    };
    double case_insensitive_time = time_sort(case_insensitive_data, case_insensitive_cmp);
    case_insensitive_data.gather(0);


    if (!comm.rank()) {
        std::cout << "Test string\n";
        std::cout << "Normal sort time: " << elapsed_time << "s : " <<
            (std::is_sorted(data.begin(), data.end()) ? "Sorted correctly\n" : "Not sorted\n");
        std::cout << "Reverse sort time: " << reverse_elapsed_time << "s : " <<
            (std::is_sorted(reverse_data.begin(), reverse_data.end(), std::greater<std::string>()) ?
                "Sorted correctly\n" : "Not sorted\n");
        std::cout << "Case insensitive sort time: " << case_insensitive_time << "s : " <<
            (std::is_sorted(case_insensitive_data.begin(), case_insensitive_data.end(),
                            case_insensitive_cmp) ? "Sorted correctly\n" : "Not sorted\n");
        std::cout << std::endl;
    }
}


template <typename T>
double time_sort(psrs::mpi_vector<T> &v)
{
    boost::mpi::timer sort_timer;
    psrs::sort(v);

    return sort_timer.elapsed();
}


template <typename T, class Compare>
double time_sort(psrs::mpi_vector<T> &v, Compare cmp)
{
    boost::mpi::timer sort_timer;
    psrs::sort(v, cmp);

    return sort_timer.elapsed();
}
