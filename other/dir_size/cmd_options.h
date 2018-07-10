#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#include <filesystem>

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
        std::filesystem::path root_dir;
        bool use_symlink;

    public:
        cmd_options(int argc, char **const argv);
        bool is_help() const;
        friend std::ostream& operator<<(std::ostream &os, const cmd_options &opts);
};


#endif
