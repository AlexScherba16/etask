#include <iostream>

#include "cli/cli_parser.h"

int main(int argc, char** argv)
{
    using namespace itask::cli_parser;

    try
    {
        auto args{CliParser{"itask", "ingenium coding task"}.parse(argc, argv)};
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
