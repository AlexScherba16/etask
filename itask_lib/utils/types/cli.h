#ifndef CLI_ARGS_H
#define CLI_ARGS_H

#include <string>

namespace itask::utils::types
{
    /**
    * @struct CliArgs
    * @brief Structure for storing command-line arguments.
    *
    * This structure is used to store the file path to a JSON file
    * provided as a command-line argument.
    */
    struct CliArgs
    {
        std::string jsonFilePath{};
    };
}

#endif //CLI_ARGS_H
