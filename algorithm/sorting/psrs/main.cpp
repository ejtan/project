#include <iostream>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "psrs/mpi_vector.h"
#include "psrs/sort.h"


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

    std::vector<double> data;
    psrs::mpi_vector<double> v(world);

    if (world.rank() == 0) {
        std::random_device rd;
        std::mt19937 engine(rd());
        std::uniform_real_distribution<double> range(-100.0, 100.0);

        data.resize(N);
        std::generate(data.begin(), data.end(),
                [&range, &engine]()->double { return range(engine); });
    } // Generate data

    v.distribute(data.begin(), data.end(), 0);

    psrs::sort(world, v);

    v.gather(0);

    if (!world.rank()) {
        if (std::is_sorted(v.begin(), v.end()))
            std::cout << "Sorted correctly" << std::endl;
        else
            std::cout << "Not sorted" << std::endl;
    }
}
