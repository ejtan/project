#include <numeric>

#include <fmt/format.h>

#include "count_filesize.h"

namespace fs = std::filesystem;


void count_filesize(const cmd_options &opts)
{
    for (const auto &dir : opts) {
        std::uintmax_t dir_size;
        fs::path root = dir;

        if (auto max_depth = opts.get_max_depth()) {
            dir_size = count_max_depth(root, opts.use_symlinks(), max_depth.value());
        } else {
            auto dir_iter = (!opts.use_symlinks()) ? fs::recursive_directory_iterator(root) :
                fs::recursive_directory_iterator(root,
                        fs::directory_options::follow_directory_symlink);
            dir_size = count_all_files(dir_iter);
        }

        if (auto prefix = opts.get_prefix()) {
            double new_dir_size = opts.convert_size(static_cast<double>(dir_size));
            fmt::print("{:<{}} {:>} {}\n", root.string(), root.string().length() + 5,
                    new_dir_size, prefix.value());
        } else {
            fmt::print("{:<{}} {:>n} bytes\n", root.string(), root.string().length() + 5, dir_size);
        } // Choose print message based on if prefix is provided
    }
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
