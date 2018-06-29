#include <numeric>

#include <fmt/format.h>

#include "count_filesize.h"

namespace fs = std::filesystem;


void count_filesize(const std::filesystem::path &root, bool use_symlink, int max_depth)
{
    std::uintmax_t dir_size;

    if (max_depth >= 0) {
        dir_size = count_max_depth(root, use_symlink, max_depth);
    } else {
        auto dir_iter = (!use_symlink) ? fs::recursive_directory_iterator(root) :
            fs::recursive_directory_iterator(root, fs::directory_options::follow_directory_symlink);
        dir_size = count_all_files(dir_iter);
    }

    fmt::print("{:<{}} {:>n} bytes\n", root.string(), root.string().length() + 5, dir_size);
}


std::uintmax_t count_all_files(const std::filesystem::recursive_directory_iterator &dir_it)
{
    return std::accumulate(fs::begin(dir_it), fs::end(dir_it), std::uintmax_t(0),
            [](const std::uintmax_t tot, const fs::directory_entry &f) {
                return f.is_regular_file() ? f.file_size() + tot : tot;
    });
}


std::uintmax_t count_max_depth(const std::filesystem::path &dir, bool use_symlink, int max_depth)
{
    std::uintmax_t total = 0;
    auto it = (!use_symlink) ? fs::recursive_directory_iterator(dir) :
        fs::recursive_directory_iterator(dir, fs::directory_options::follow_directory_symlink);

    for(; it != fs::recursive_directory_iterator(); ++it )
        if (it.depth() <= max_depth && it->is_regular_file())
            total += it->file_size();

    return total;
}
