#include <iostream>
#include <exception>
#include <filesystem>
#include <numeric>

#include <boost/program_options.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 *-----------------------------------------------------------------------------------------------*/
void count_filesize(const fs::path &root);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    try {
        if (argc == 1) {
            std::cout << "Usage: " << argv[0] << "dir [Options]" << std::endl;
            return 0;
        } // Print usage message if no arguments

        fs::path root_dir;

        // Allowed flags / program options
        po::options_description program("Allowed Options");
        program.add_options()
            ("help,h", "Print help message.");

        // Hidden argument not printed in help message.
        po::options_description hidden;
        hidden.add_options()
            ("dir", po::value<fs::path>(&root_dir), "Input directory.");

        // Grab directory as a positional parameter
        po::positional_options_description dir_arg;
        dir_arg.add("dir", -1);

        // Group hidden and program options
        po::options_description cmd_input;
        cmd_input.add(program).add(hidden);

        po::variables_map args;
        po::store(po::command_line_parser(argc, argv).options(cmd_input).positional(dir_arg).run(),
                args);
        po::notify(args);

        if (args.count("help")) {
            std::cout << program << std::endl;
            return 0;
        } // Output help message and exit program

        count_filesize(root_dir);
    } catch (std::exception &e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cout << "Unknown exception caught in main()." << std::endl;
        return EXIT_FAILURE;
    }
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/

/* count_filesize()
 *
 * @INPUT: root = path to directory to find the size of
 *
 * Recursively iterate through a directory and totals the size of valid files.
 */
void count_filesize(const fs::path &root)
{
    auto dir_iter = fs::recursive_directory_iterator(root);

    // Accumulate file sizes. uintmax_t is what file_size returns
    std::uintmax_t dir_size = std::accumulate(fs::begin(dir_iter), fs::end(dir_iter),
            std::uintmax_t(0), [](const std::uintmax_t tot, const fs::directory_entry &f) {
            return fs::is_regular_file(f) ? f.file_size() + tot : tot;
    });

    std::cout << "Total size of files in " << root << ": " << dir_size << " bytes.\n";
}
