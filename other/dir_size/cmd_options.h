#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#include <optional>
#include <unordered_map>
#include <vector>

#include <boost/program_options.hpp>


/* Class: cmd_options
 *
 * Class for processing and getting command line options.
 */
class cmd_options
{
    private:
        boost::program_options::variables_map arg_values;
        boost::program_options::options_description program;
        bool use_symlink;
        std::string prefix_str;
        std::vector<std::string> dirs;
        // Small lookup table for --prefix flag
        const std::unordered_map<std::string, double> conversion_table = {
            {"kB", 1000.0}, {"MB", 1000000.0}, {"GB", 1000000000.0},
            {"KiB", 1024.0}, {"MiB", 1048576.0}, {"GiB", 1073741824.0} };

    public:
        // Typedef
        typedef std::vector<std::string>::iterator iterator;

        // Public member functions
        cmd_options(int argc, char **const argv);
        bool is_help() const;
        const std::string& get_root_dir() const;
        std::optional<int> get_max_depth() const;
        std::optional<std::string> get_prefix() const;
        bool use_symlinks() const;
        double convert_size(double size) const noexcept;
        iterator begin() noexcept;
        iterator end() noexcept;
        friend std::ostream& operator<<(std::ostream &os, const cmd_options &opts);
};


#endif
