#ifndef COUNT_FILESIZE_H
#define COUNT_FILESIZE_H

#include <filesystem>

#include "cmd_options.h"


/* count_filesize()
 *
 * @INPUT: opts = command options
 *
 * Recursively iterate through a directory and totals the size of valid files.
 */
void count_filesize(const cmd_options &opts);

/* count_all_files()
 *
 * @INPUT: dir_it = recursive directory iterator
 *
 * @RETURN: std::uintmax_t = total size of contents of the root directory
 *
 * Counts all the files within the directory (no set recursion depth).
 */
std::uintmax_t count_all_files(const std::filesystem::recursive_directory_iterator &dir_it);

/* count_max_depth()
 *
 * @INPUT: dir = directory
 * @INPUT: use_symlink = allow symlink
 * @INPUT: max_depth = max recursion depth allowed.
 *
 * @RETURN: std::uintmax_t = total size of contents of the directory
 *
 * Computes the size of a directory up to a recursion depth. Requires a call to
 * recursive_directory_iterator.depth() so this must be done with a for loop.
 */
std::uintmax_t count_max_depth(const std::filesystem::path &dir, bool use_symlink, int max_depth);


#endif
