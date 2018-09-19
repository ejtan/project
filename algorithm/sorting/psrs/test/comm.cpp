#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <numeric>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "../psrs/mpi_vector.h"
#include "../psrs/mpi_utility.h"


namespace mpi = boost::mpi;


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 *-----------------------------------------------------------------------------------------------*/
void test_int(const mpi::communicator &comm);
void test_string(const mpi::communicator &comm);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(void)
{
    try {
        mpi::environment env;
        mpi::communicator comm;

        test_int(comm);
        test_string(comm);
    } catch (std::exception &e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cout << "Unknown exception caught in main()" << std::endl;
        return EXIT_FAILURE;
    }
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/
void test_int(const mpi::communicator &comm)
{
    int N = 100;
    std::vector<int> data(N);

    // Eacho proc needs data filled to compare with later
    std::iota(data.begin(), data.end(), 0);

    psrs::mpi_vector<int> v;

    // Distribute from proc 0
    v.distribute(data.begin(), data.end(), 0);

    int p = comm.size();
    int id = comm.rank();
    int local_n = psrs::utility::blk_size(id, p, N);

    // Test size of distributed vector
    if (local_n != v.size())
        throw std::runtime_error("Error: distrubite().\n"
                "Expected local_n and v.size() to be equal on proc " + std::to_string(id) + ".\n" +
                "local_n = " + std::to_string(local_n) + " v.size() = " + std::to_string(v.size()));

    int start = psrs::utility::blk_low(id, p, N);
    int end = psrs::utility::blk_high(id, p, N);

    // Test vector data
    if (!std::equal(data.begin() + start, data.begin() + end, v.begin()))
        throw std::runtime_error("Error: distribute().\nExpected proc " +
                std::to_string(id) + " data section [" + std::to_string(start) + ", " +
                std::to_string(end) + ") to be equal.");

    comm.barrier();

    // Create a copy of v while data is spread to use later
    psrs::mpi_vector<int> v_tmp(v);

    v.gather(0);

    // Test gather data to proc 0
    if (!id)
        if (!std::equal(data.begin(), data.end(), v.begin()))
            throw std::runtime_error("Error: gather().\nExpected data to be equal on proc 0.");

    v.scatter();

    // Test scatter size
    if (local_n != v.size())
        throw std::runtime_error("Error: scatter().\n"
                "Expected local_n and v.size() to be equal on proc " + std::to_string(id) + ".\n" +
                "local_n = " + std::to_string(local_n) + " v.size() = " + std::to_string(v.size()));

    // Test scatter data
    if (!std::equal(v.begin(), v.end(), v_tmp.begin()))
        throw std::runtime_error("Error: scatter().\n"
                "Expected data to be equally scattered on proc " + std::to_string(id) + '.');
}


void test_string(const mpi::communicator &comm)
{
    std::vector<std::string> data = {"01", "02", "03", "04", "05", "06", "07", "08", "09",
        "10", "11", "12", "13", "14", "15"};
   int N = data.size();
   psrs::mpi_vector<std::string> v(comm);

   v.distribute(data.begin(), data.end(), 0);

   int p = comm.size();
   int id = comm.rank();
   int local_n = psrs::utility::blk_size(id, p, N);

    // Test size of distributed vector
    if (local_n != v.size())
        throw std::runtime_error("Error: distrubite() in test_string.\n"
                "Expected local_n and v.size() to be equal on proc " + std::to_string(id) + ".\n" +
                "local_n = " + std::to_string(local_n) + " v.size() = " + std::to_string(v.size()));

    int start = psrs::utility::blk_low(id, p, N);
    int end = psrs::utility::blk_high(id, p, N);

    // Test vector data
    if (!std::equal(data.begin() + start, data.begin() + end, v.begin()))
        throw std::runtime_error("Error: distribute().\nExpected proc " +
                std::to_string(id) + " data section [" + std::to_string(start) + ", " +
                std::to_string(end) + ") to be equal.");

    comm.barrier();

    // Create a copy of v while data is spread to use later
    psrs::mpi_vector<std::string> v_tmp(v);

    v.gather(0);

    // Test gather data to proc 0
    if (!id)
        if (!std::equal(data.begin(), data.end(), v.begin()))
            throw std::runtime_error("Error: gather().\nExpected data to be equal on proc 0.");

    v.scatter();

    // Test scatter size
    if (local_n != v.size())
        throw std::runtime_error("Error: scatter().\n"
                "Expected local_n and v.size() to be equal on proc " + std::to_string(id) + ".\n" +
                "local_n = " + std::to_string(local_n) + " v.size() = " + std::to_string(v.size()));

    // Test scatter data
    if (!std::equal(v.begin(), v.end(), v_tmp.begin()))
        throw std::runtime_error("Error: scatter().\n"
                "Expected data to be equally scattered on proc " + std::to_string(id) + '.');
}
