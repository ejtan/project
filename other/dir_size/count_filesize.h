#ifndef COUNT_FILESIZE_H
#define COUNT_FILESIZE_H

#include <filesystem>


/* count_filesize()
 *
 * @INPUT: root = path to directory to find the size of
 * @INPUT: use_symlink = allow symlink
 * @INPUT: max_depth = max recursion depth allowed.
 *
 * Recursively iterate through a directory and totals the size of valid files.
 */
void count_filesize(const std::filesystem::path &root, bool use_symlink, int max_depth);

/* count_all_files()
 *
 * @INPUT: dir_it = recursive directory iterator
 *
 * @RETURN: std::uintmax_t = total size of contents of the root directory
 *
 * Counts all the files within the directory (no set recursion depth).
 */
std::uintmax_t count_all_files(const std::filesystem::recursive_directory_iterator &dir_it);


#endif
