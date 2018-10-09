/* Written by: Eric Tan
 *
 * Implamentation of the parallel sort by regular sampling algorithm.
 */
#ifndef SORT_H
#define SORT_H

#include <vector>
#include <functional>
#include <stdexcept>
#include <mpi.h>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>

#include "mpi_vector.h"
#include "all_to_allv.h"


namespace psrs {
namespace detail {


template <typename T, class Compare>
void sort_mpi_type_impl(const boost::mpi::communicator &comm,
        psrs::mpi_vector<T> &data, Compare cmp)
{
    // Sort each proc's data
    std::sort(data.begin(), data.end(), cmp);

    // Get samples
    std::vector<T> local_samples;
    int p = comm.size();
    int n = data.total_size();

    for (int i = 0; i <= (p - 1) * (n / (p * p)); i += n / (p * p))
        local_samples.push_back(data[i]);

    // Gather samples into proc 0
    std::vector<T> samples;
    boost::mpi::gather(comm, local_samples.data(), local_samples.size(), samples, 0);

    // Sort samples and determine pivots on proc 0. The broadecast pivots to all procs
    std::vector<T> pivots;
    if (!comm.rank()) {
        std::sort(samples.begin(), samples.end(), cmp);
        for (int i = p + (p / 2) - 1; i <= (p - 1) * p + (p / 2); i += p)
            pivots.push_back(samples[i]);
    }

    boost::mpi::broadcast(comm, pivots, 0);

    // Generate send displacement and count vectors for all to all
    std::vector<int> send_disp, send_cnt;

    send_disp.push_back(0);
    for (const auto &piv : pivots) {
        auto it = std::lower_bound(data.begin(), data.end(), piv, cmp);
        send_disp.push_back(static_cast<int>(std::distance(data.begin(), it)));
    }

    send_cnt.resize(send_disp.size());
    std::adjacent_difference(send_disp.begin() + 1, send_disp.end() - 1, send_cnt.begin());
    send_cnt.back() = data.size() - send_disp.back();

    // Generate recv displacement and count vecor
    std::vector<int> recv_disp, recv_cnt;
    boost::mpi::all_to_all(comm, send_cnt, recv_cnt);

    recv_disp.resize(recv_cnt.size());
    std::partial_sum(recv_cnt.begin(), recv_cnt.end() - 1, recv_disp.begin() + 1);

    // Create temp vector to contain data.
    std::vector<T> tmp;

    // Perform an all to all data swap.
    psrs::mpi::all_to_allv(comm, data.get_vector(), send_cnt, send_disp, tmp, recv_cnt, recv_disp);

    // Perform final sort of data
    std::sort(tmp.begin(), tmp.end(), cmp);

    // move tmp vector with data's internal vector.
    data.move_vector(std::move(tmp));
}


}; // Namespace detail


/* sort()
 * @INPUT: comm = communicator
 * @INPUT: data = data to sort
 *
 * Sort with default comparator operator<.
 */
template <typename T>
void sort(psrs::mpi_vector<T> &data)
{
    // Special case: single proc
    if (data.get_comm().size() == 1) {
        std::sort(data.begin(), data.end());
        return;
    }

    detail::sort_mpi_type_impl(data.get_comm(), data, std::less<T>());
}


/* sort()
 * @INPUT: comm = communicator
 * @INPUT: data = data to sort
 * @INPUT: cmp = comparator for sorting. Default to std::less<T>
 *
 * Sort with callable comparator cmp.
 */
template <typename T, class Compare>
void sort(psrs::mpi_vector<T> &data, Compare cmp)
{
    // Special case: single proc
    if (data.get_comm().size() == 1) {
        std::sort(data.begin(), data.end(), cmp);
        return;
    }

    detail::sort_mpi_type_impl(data.get_comm(), data, cmp);
}


}; // Namespace psrs

#endif
