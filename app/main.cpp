#include <iostream>

#include "cli/cli_parser.h"
#include "preprocessor/preprocessor.h"

constexpr uint64_t THIRTY_MIN_IN_NANO_SECONDS{1'800'000'000'000};

int main(int argc, char** argv)
{
    using namespace itask::cli_parser;
    using namespace itask::preprocessor;

    try
    {
        const auto args{CliParser{"itask", "Ingenium coding task"}.parse(argc, argv)};
        const auto preprocData{Preprocessor{args.jsonFilePath, 4, THIRTY_MIN_IN_NANO_SECONDS}.getPreprocessedData()};
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
