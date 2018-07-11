#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#include <optional>

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

    public:
        cmd_options(int argc, char **const argv);
        bool is_help() const;
        const std::string& get_root_dir() const;
        std::optional<int> get_max_depth() const;
        std::optional<std::string> get_prefix() const;
        bool use_symlinks() const;
        friend std::ostream& operator<<(std::ostream &os, const cmd_options &opts);
};


#endif
