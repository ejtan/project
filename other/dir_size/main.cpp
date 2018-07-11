#include <iostream>
#include <exception>
#include <filesystem>
#include <clocale>
#include <string>

#include "count_filesize.h"
#include "cmd_options.h"


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    try {
        const std::string usage_msg = "Usage: " + std::string(argv[0]) + " dir [Options]";

        if (argc == 1) {
            std::cout << usage_msg << std::endl;
            return 0;
        } // Print usage message if no arguments

        cmd_options opts(argc, argv);

        if (opts.is_help()) {
            std::cout << usage_msg << "\n\n" << opts << std::endl;
            return 0;
        } // Outputs help message and exits

        count_filesize(opts);
    } catch (std::exception &e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cout << "Unknown exception caught in main()." << std::endl;
        return EXIT_FAILURE;
    }
}
