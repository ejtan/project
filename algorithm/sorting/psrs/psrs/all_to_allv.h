/* Written by: Eric Tan
 *
 * Workaround for boost::mpi not implamenting MPI_Alltoallv.
 */
#ifndef MPI_ALLTOALLV
#define MPI_ALLTOALLV

#include <vector>
#include <numeric>
#include <mpi.h>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/datatype.hpp>
#include <boost/mpi/exception.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/allocator.hpp>


namespace psrs {
namespace mpi {

namespace detail {


template <typename T>
void all_to_allv_mpi_type_impl(const boost::mpi::communicator &comm,
        const T *in_values, const int *in_count, const int *in_disp,
        T *out_values, const int *out_count, const int *out_disp)
{
    MPI_Datatype dtype = boost::mpi::get_mpi_datatype<T>(*in_values);
    BOOST_MPI_CHECK_RESULT(MPI_Alltoallv,
            (const_cast<T*>(in_values), const_cast<int*>(in_count), const_cast<int*>(in_disp), dtype,
             out_values, const_cast<int*>(out_count), const_cast<int*>(out_disp), dtype, comm));
}


template <typename T>
void all_to_allv_general_impl(const boost::mpi::communicator &comm,
        const T *in_values, const int *in_count, const int *in_disp,
        T *out_values, const int *out_count, const int *out_disp)
{
    int p = comm.size();
    int rank = comm.rank();

    // Prepare data to send out
    std::vector<int> send_count(p), send_disp(p);
    std::vector<char, boost::mpi::allocator<char> > outgoing;

    int skip = 0;
    for (int dest = 0; dest < p; dest++) {
        send_disp[dest] = outgoing.size();

        if (dest != rank) {
            boost::mpi::packed_oarchive oa(comm, outgoing);
            for (int i = 0; i < in_count[dest]; i++)
                oa << in_values[skip + i];
        } // Pack data if needed

        skip += in_count[dest];
        send_count[dest] = outgoing.size() - send_disp[dest];
    } // Pack outgoing data

    // Prepare incoming data
    std::vector<int> recv_count(p), recv_disp(p);
    boost::mpi::all_to_all(comm, send_count, recv_count);

    int sum = 0;
    for (int src = 0; src < p; src++) {
        recv_disp[src] = sum;
        sum += recv_count[src];
    } // Compute recv_disp

    std::vector<char, boost::mpi::allocator<char> > incoming(sum > 0 ? sum : 1);

    // Incase outgoing vector is empty, put a value
    if (outgoing.empty())
        outgoing.push_back(0);

    // Perform data swap on the buffers
    BOOST_MPI_CHECK_RESULT(MPI_Alltoallv, (&outgoing[0], &send_count[0], &send_disp[0],
                MPI_PACKED, &incoming[0], &recv_count[0], &recv_disp[0], MPI_PACKED, comm));

    // Unpack buffer
    for (int proc = 0; proc < p; proc++) {
        if (proc != rank) {
            boost::mpi::packed_iarchive ia(comm, incoming,
                    boost::archive::no_header, recv_disp[proc]);
            for (int i = 0; i < out_count[proc]; i++)
                ia >> out_values[out_disp[proc] + i];
        } else {
            std::copy(in_values + in_disp[proc], in_values + in_disp[proc] + in_count[proc],
                    out_values + out_disp[proc]);
        } // Unpack data depending on process rank
    } // Loop over proc's data
}


}; // namespace detail


/* all_to_allv()
 * @INPUT: comm = MPI communicator
 * @INPUT: in_values = values to send
 * @INPUT: in_count, in_disp = count and displacement for values to send
 * @INPUT: out_values = output vector
 * @INPUT: out_values, out_count = count and displancement for output vector
 *
 * Wrapper for MPI_Alltoallv. Supports both MPI_Datatype and serializable datatypes using
 * the Boost.Serialization library.
 */
template <typename T>
inline void all_to_allv(const boost::mpi::communicator &comm, const std::vector<T> &in_values,
        const std::vector<int> &in_count, const std::vector<int> &in_disp,
        std::vector<T> &out_values, const std::vector<int> &out_count,
        const std::vector<int> &out_disp)
{
    // Compute total size of output vector
    int out_size = std::accumulate(out_count.begin(), out_count.end(), 0);

    out_values.resize(out_size);

    if constexpr(boost::mpi::is_mpi_datatype<T>())
        detail::all_to_allv_mpi_type_impl(comm, &in_values[0], in_count.data(), in_disp.data(),
            &out_values[0], out_count.data(), out_disp.data());
    else
        detail::all_to_allv_general_impl(comm, &in_values[0], in_count.data(), in_disp.data(),
                &out_values[0], out_count.data(), out_disp.data());
}


}; // namespace mpi
}; // namespace psrs


#endif
