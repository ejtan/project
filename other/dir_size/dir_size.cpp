#include <iostream>
#include <exception>


int main(int argc, char *argv[])
{
    try {
        if (argc == 1) {
            std::cout << "Usage: " << argv[0] << "dir [Options]" << std::endl;
            return 0;
        } // Print usage message if no arguments
    } catch (std::exception &e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cout << "Unknown exception caught in main()." << std::endl;
        return EXIT_FAILURE;
    }
}
