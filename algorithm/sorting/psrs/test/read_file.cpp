#include <iostream>
#include <iterator>
#include <string>
#include <algorithm>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/timer.hpp>

#include "../psrs/mpi_vector.h"


namespace mpi = boost::mpi;


int main(void)
{
    mpi::environment env;
    mpi::communicator comm;
    psrs::mpi_vector<std::string> data(comm);

    mpi::timer read_timer;
    data.read_file("/usr/share/dict/words");
    double read_time = read_timer.elapsed();

    std::ostream_iterator<std::string> os(std::cout, "\n");

    if (!comm.rank())
        std::cout << "read_file() time taken: " << read_time << "s." << std::endl;

    comm.barrier();

    for (int i = 0; i < comm.size(); i++) {
        if (comm.rank() == i)
            std::copy(data.begin(), data.end(), os);
        comm.barrier();
    } // Loop over procs
}
