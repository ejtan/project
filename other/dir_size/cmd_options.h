#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#include <boost/program_options.hpp>


/* Class: cmd_options
 *
 * Class for processing and getting command line options.
 */
class cmd_options
{
    private:
        boost::program_options::variables_map arg_values;

    public:
        cmd_options(int argc, char **const argv);
};


#endif
