#include "cli/cli_parser.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace itask::cli_parser;
using namespace itask::utils::types;

TEST(CliParserTest, ParseMissingParameter_ThrowsException) {
    const char* argv[] = {"test"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    CliParser parser("", "");
    ASSERT_THROW(parser.parse(argc, const_cast<char**>(argv)), std::invalid_argument);
}

TEST(CliParserTest, ParseValidInputParameter) {
    const std::string expectedPath {"path/to/something_expected_here.json"};

    const char* argv[] = {"test", "--path", expectedPath.c_str()};
    int argc = sizeof(argv) / sizeof(argv[0]);

    CliParser parser("", "");
    CliArgs actualArgs {};

    ASSERT_NO_THROW(actualArgs = parser.parse(argc, const_cast<char**>(argv)));
    ASSERT_EQ(actualArgs.jsonFilePath, expectedPath);
}

TEST(CliParserTest, ParseInvalidInputParameter_ThrowsException) {
    const std::string expectedPath {"path/to/something_expected_here.json"};

    const char* argv[] = {"test", "--INVALID_PARAMETER", expectedPath.c_str()};
    int argc = sizeof(argv) / sizeof(argv[0]);

    CliParser parser("", "");

    // assert any, to prevent high framework code dependency
    ASSERT_ANY_THROW(parser.parse(argc, const_cast<char**>(argv)));
}
