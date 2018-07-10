#include "cmd_options.h"


cmd_options::cmd_options(int argc, char **const argv)
{
    // Allowed flags
    program.add_options()
        ("help,h", "Prints help message.")
        ("sym,s", boost::program_options::bool_switch(&use_symlink), "Allow symbolic links.")
        ("n", "Use nice number output (uses system locale).")
        ("d", "Set max recursion depth allowed");

    // Hidden input not printed in help message.
    boost::program_options::options_description hidden;
    hidden.add_options()
        ("dir", boost::program_options::value<std::filesystem::path>(&root_dir), "Input directory");

    // Take the directory input as a positional parameter
    boost::program_options::positional_options_description dir_arg;
    dir_arg.add("dir", -1);

    // Group hidden and program options
    boost::program_options::options_description cmd_line_input;
    cmd_line_input.add(program).add(hidden);

    // Store arguments to variables_map arg_values
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
            options(cmd_line_input).positional(dir_arg).run(), arg_values);
    boost::program_options::notify(arg_values);
}
