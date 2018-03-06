#include "mpi_utility.h"


/* block_low()
 *
 * @param: id = proc id
 * @param: p = number of procs
 * @param: n = number of elements
 *
 * @return: low index value of a proc's portion of data
 */
int block_low(int id, int p, int n)
{
    return id * n / p;
}


/* block_high()
 *
 * @param: id = proc id
 * @param: p = number of procs
 * @param: n = number of elements
 *
 * @return: high index of a proc's portion of data
 */
int block_high(int id, int p, int n)
{
    return block_low(id + 1, p, n) - 1;
}


/* block_size()
 *
 * @param: id = proc id
 * @param: p = number of procs
 * @param: n = number of elements
 *
 * @return: size of the proc's portion of data
 */
int block_size(int id, int p, int n)
{
    return block_high(id, p, n) - block_low(id, p, n) + 1;
}


/* make_mixed_xfer_array()
 *
 * @param: p = number of procs
 * @param: n = number of elements
 * @param: cnt = vector of counts
 * @param: disp = vector of displacement
 *
 * @return: cnt and disp
 *
 * Creates arrays used for transfering data in MPI collective communications. Required when each
 * proc's portion of data is not all the same size.
 */
void make_mixed_xfer_array(int p, int n, std::vector<int> &cnt, std::vector<int> &disp)
{
    cnt.resize(p);
    disp.resize(p);

    cnt[0]  = block_size(0, p, n);
    disp[0] = 0;

    for (int i = 1; i < p; i++) {
        disp[i] = disp[i - 1] + cnt[i - 1];
        cnt[i]  = block_size(i, p, n);
    } // Set arrays
}


/* make_uniform_xfer_array()
 *
 * @param: id = proc id
 * @param: p = number of procs
 * @param: n = number of elements
 * @param: cnt = vector of counts
 * @param: disp = vector of displacement
 *
 * @return: cnt and disp
 *
 * Creates arrays used for transfering data in MPI collective communications.
 */
void make_uniforn_xfer_array(int id, int p, int n, std::vector<int> &cnt, std::vector<int> &disp)
{
    cnt.resize(p);
    disp.resize(p);

    cnt[0]  = block_size(id, p, n);
    disp[0] = 0;

    for (int i = 0; i < p; i++) {
        disp[i] = disp[i - 1] + cnt[i - 1];
        cnt[i]  = block_size(id, p, n);
    } // Set arrays
}

