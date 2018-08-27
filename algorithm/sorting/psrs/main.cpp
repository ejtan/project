#include <iostream>
#include <random>
#include <functional>
#include <algorithm>
#include <cstdlib>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/timer.hpp>

#include "psrs/mpi_vector.h"
#include "psrs/sort.h"


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 *-----------------------------------------------------------------------------------------------*/
void test_double(const boost::mpi::communicator &comm, int N);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;

    // Get vector size (default 100)
    int N;

    if (argc == 2)
        N = atoi(argv[1]);
    else
        N = 100;

    test_double(world, N);
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

    psrs::mpi_vector<double> v;
    v.distribute(data.begin(), data.end(), 0);

    psrs::mpi_vector<double> v_reverse(v);

    // Time sort with default compare
    boost::mpi::timer sort_timer;
    psrs::sort(v.get_comm(), v);
    double elapsed_time = sort_timer.elapsed();
    v.gather(0);

    // Time sort with std::greater compare
    boost::mpi::timer reverse_sort_time;
    psrs::sort(v_reverse.get_comm(), v_reverse, std::greater<double>());
    double reverse_elapsed_time = reverse_sort_time.elapsed();
    v_reverse.gather(0);

    if (!comm.rank()) {
        std::cout << "Normal Sort time: " << elapsed_time << "s : ";
        std::cout << (std::is_sorted(v.begin(), v.end()) ? "Sorted correctly\n" : "Not sorted\n");
        std::cout << "Reverse Sort time: " << reverse_elapsed_time << "s : ";
        std::cout << (std::is_sorted(v_reverse.begin(), v_reverse.end(), std::greater<double>()) ?
                "Sorted correctly\n" : "Not sorted\n");
    }
}
