#include <iostream>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "psrs/mpi_vector.h"
#include "psrs/sort.h"


int main()
{
    boost::mpi::environment env;
    boost::mpi::communicator world;

    std::vector<int> data;
    psrs::mpi_vector<int> v(world);

    if (world.rank() == 0)
        data = {15, 46, 48, 93, 39,  6, 72, 91, 14,
                36, 69, 40, 89, 61, 97, 12, 21, 54,
                53, 97, 84, 58, 32, 27, 33, 72, 20};

    v.distribute(data.begin(), data.end(), 0);

    psrs::sort(world, v);

    world.barrier();
    v.gather(0);

    if (!world.rank()) {
        if (std::is_sorted(v.begin(), v.end()))
            std::cout << "Sorted correctly" << std::endl;
        else
            std::cout << "Not sorted" << std::endl;
    }
}
