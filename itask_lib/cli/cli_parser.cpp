#include "cli_parser.h"

#include <cxxopts.hpp>

namespace itask::cli_parser
{
    using namespace itask::utils::types;

    CliParser::CliParser(std::string appName, std::string appDescription) :
        appName_(std::move(appName)), appDescription_(std::move(appDescription))
    {
    }

    CliArgs itask::cli_parser::CliParser::parse(int argc, char** argv)
    {
        cxxopts::Options options(appName_, appDescription_);
        options.add_options()
            ("p,path", "JSON file path", cxxopts::value<std::string>());

        auto result = options.parse(argc, argv);
        if (!result.count("path"))
        {
            throw std::invalid_argument("JSON file path is required. Use --path or -p to specify it");
        }

        std::string jsonFilePath{result["path"].as<std::string>()};
        return {std::move(jsonFilePath)};
    }
}
