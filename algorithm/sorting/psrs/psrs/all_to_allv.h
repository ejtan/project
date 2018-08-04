/* Written by: Eric Tan
 *
 * Workaround for boost::mpi not implamenting MPI_Alltoallv.
 */
#ifndef MPI_ALLTOALLV
#define MPI_ALLTOALLV

#include <numeric>
#include <mpi.h>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/datatype.hpp>
#include <boost/mpi/exception.hpp>


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


}; // namespace detail


/* all_to_allv()
 * @INPUT: comm = MPI communicator
 * @INPUT: in_values = values to send
 * @INPUT: in_count, in_disp = count and displacement for values to send
 * @INPUT: out_values = output vector
 * @INPUT: out_values, out_count = count and displancement for output vector
 *
 * Wrapper for MPI_Alltoallv. Currently only supports MPI built in types
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
            &out_values[0], out_count.data(), out_count.data());
}


}; // namespace mpi
}; // namespace psrs


#endif
