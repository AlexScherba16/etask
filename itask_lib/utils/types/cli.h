#ifndef TYPES_H
#define TYPES_H

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

#endif //TYPES_H
