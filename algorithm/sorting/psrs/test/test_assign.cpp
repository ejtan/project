#include <iostream>
#include <numeric>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "../psrs/mpi_vector.h"

namespace mpi = boost::mpi;


int main()
{
    mpi::environment env;
    mpi::communicator comm;

    std::vector<int> data(100);
    std::iota(data.begin(), data.end(), 1);
    psrs::mpi_vector<int> a(comm, data.begin(), data.end(), 0);


    // Test operator= (copy)
    psrs::mpi_vector<int> b;
    b = a;

    if (!comm.rank())
        std::cout << "Testing copy assiangment operator... ";

    for (int i = 0; i < comm.size(); i++) {
        if (i == comm.rank()) {
            if (b.size() != a.size()) {
                std::cout << "Error: Copy assiangment operator on proc " << i
                    << "b.size() doesn't match with reference data.\n";
                env.abort(-1);
            }

            if (!std::equal(a.begin(), a.end(), b.begin())) {
                std::cout << "Error: Copy assiangment operator on proc " << i
                    << "doesn't match with reference data.\n";
                env.abort(-1);
            }
        }
        comm.barrier();
    }

    if (!comm.rank())
        std::cout << "passed.\n";

    // Test operator= (move)
    psrs::mpi_vector<int> c;
    c = std::move(b);

    if (!comm.rank())
        std::cout << "Testing move assiangment operator... ";

    for (int i = 0; i < comm.size(); i++) {
        if (i == comm.rank()) {
            // Use a as a reference to compare to
            if (c.size() != a.size()) {
                std::cout << "Error: Move assiangment operator on proc " << i
                    << "c.size() doesn't match with reference data.\n";
                env.abort(-1);
            }

            if (b.size() != 0) {
                std::cout << "Error: Move assiangment operator on proc " << i
                    << "b.size() is nonzero.\n";
                env.abort(-1);
            }

            if (!std::equal(a.begin(), a.end(), c.begin())) {
                std::cout << "Error: Move assiangment operator on proc " << i
                    << "doesn't match with reference data.\n";
                env.abort(-1);
            }
        }
        comm.barrier();
    }

    if (!comm.rank())
        std::cout << "passed.\n";

    // Test move constructor
    psrs::mpi_vector<int> d(std::move(c));

    if (!comm.rank())
        std::cout << "Testing move constructor... ";

    for (int i = 0; i < comm.size(); i++) {
        if (i == comm.rank()) {
            // Use a as a reference to compare to
            if (d.size() != a.size()) {
                std::cout << "Error: Move assiangment operator on proc " << i
                    << "c.size() doesn't match with reference data.\n";
                env.abort(-1);
            }

            if (c.size() != 0) {
                std::cout << "Error: Move assiangment operator on proc " << i
                    << "c.size() is nonzero.\n";
                env.abort(-1);
            }

            if (!std::equal(a.begin(), a.end(), d.begin())) {
                std::cout << "Error: Move assiangment operator on proc " << i
                    << "doesn't match with reference data.\n";
                env.abort(-1);
            }
        }
        comm.barrier();
    }

    if (!comm.rank())
        std::cout << "passed.\n";
}
