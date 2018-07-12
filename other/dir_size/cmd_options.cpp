#include <iostream>
#include <clocale>

#include "cmd_options.h"

namespace po = boost::program_options;


cmd_options::cmd_options(int argc, char **const argv)
{
    // Allowed flags
    program.add_options()
        ("help,h", "Prints help message.")
        ("sym,s", po::bool_switch(&use_symlink), "Allow symbolic links.")
        (",n", "Use nice number output (uses system locale).")
        (",d", po::value<int>(), "Set max recursion depth allowed")
        ("prefix,p", po::value<std::string>(&prefix_str),
         "Convert bytes to the corresponding prefix (below).\n"
         "SI Units:            Binary:\n"
         "  kB - 10^3 bytes      KiB - 1024 bytes\n"
         "  MB - 10^6 bytes      MiB - 1024^2 bytes\n"
         "  GB - 10^8 bytes      GiB - 1024^3 bytes");

    // Hidden input not printed in help message.
    po::options_description hidden;
    hidden.add_options()
        ("dir", po::value<std::string>(), "Input directory");

    // Take the directory input as a positional parameter
    po::positional_options_description dir_arg;
    dir_arg.add("dir", -1);

    // Group hidden and program options
    po::options_description cmd_line_input;
    cmd_line_input.add(program).add(hidden);

    // Store arguments to variables_map arg_values
    po::store(po::command_line_parser(argc, argv).
            options(cmd_line_input).positional(dir_arg).run(), arg_values);
    po::notify(arg_values);

    // Set locale if provided
    if (arg_values.count("-n"))
        std::setlocale(LC_ALL, "");

    // Check for a valid --prefix input. Check againsts conversion_table
    if (arg_values.count("prefix") && conversion_table.find(prefix_str) == conversion_table.end())
            throw po::validation_error(po::validation_error::invalid_option_value);
}


/* is_help()
 *
 * @return: bool: true if help flag is provided, false otherwise.
 */
bool cmd_options::is_help() const
{
    return arg_values.count("help");
}


/* get_root_dir()
 *
 * @return: const string reference representing the path to root directory
 */
const std::string& cmd_options::get_root_dir() const
{
    return arg_values["dir"].as<std::string>();
}


/* get_max_depth()
 *
 * @return: optional<int> containing either the max recursion depth if
 *          exists or an empty optional if not
 */
std::optional<int> cmd_options::get_max_depth() const
{
    return (arg_values.count("-d")) ?
        std::make_optional<int>(arg_values["-d"].as<int>()) : std::nullopt;
}


/* get_prefix()
 *
 * @return: optional<string> with either the prefix passed in by command line or nullopt
 */
std::optional<std::string> cmd_options::get_prefix() const
{
    return (arg_values.count("prefix")) ?
        std::make_optional<std::string>(prefix_str) : std::nullopt;
}


/* use_symlinks()
 *
 * @return: returns use_symlink variable
 */
bool cmd_options::use_symlinks() const
{
    return use_symlink;
}


/* convert_size()
 *
 * @param: size = number to convert
 *
 * @param: double with size converted by a value determined by prefix_str.
 *
 * Note we will check if the prefix flag is used before doing the conversion.
 */
double cmd_options::convert_size(double size) const noexcept
{
    return size / conversion_table.find(prefix_str)->second;
}


/* overloaded <<
 *
 * @param: os = ostream
 * @param: cmd_options = options to print
 *
 * @return: ostream
 *
 * Prints out the options_description variable program. The output represents the avaliable
 * options avaliable when using the command.
 */
std::ostream& operator<<(std::ostream &os, const cmd_options &opts)
{
    os << "Options: \n" << opts.program;
    return os;
}
