#ifndef MPI_UTILITY_H
#define MPI_UTILITY_H


namespace psrs {
namespace utility {


// message tags
constexpr int data_tag = 1;
constexpr int prompt_tag = 2;

// Functions

/* blk_low()
 * @INPUT: id = proc id
 * @INPUT: p = number of procs
 * @INPUT: n = number of elements
 *
 * Computes the low index of the block.
 */
int blk_low(int id, int p, int n) { return id * n / p; }


/* blk_low()
 * @INPUT: id = proc id
 * @INPUT: p = number of procs
 * @INPUT: n = number of elements
 *
 * Computes the high index of the block.
 */
int blk_high(int id, int p, int n) { return blk_low(id + 1, p, n) - 1; }


/* blk_low()
 * @INPUT: id = proc id
 * @INPUT: p = number of procs
 * @INPUT: n = number of elements
 *
 * Computes the size of the block.
 */
int blk_size(int id, int p, int n) { return blk_high(id, p, n) - blk_low(id, p, n) + 1; }


}; // namespace utility
}; // namespace psrs


#endif
