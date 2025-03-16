#include "preprocessor/preprocessor.h"
#include "utils/filesystem/filesystem.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace itask::preprocessor;
using namespace etask::util::filesystem;

TEST(PreprocessorTest, CreatePreprocessor_EmptyPath_ThrowsException)
{
    ASSERT_THROW(Preprocessor ("",0,0), std::invalid_argument);
}

TEST(PreprocessorTest, CreatePreprocessor_CreateZeroThread_ThrowsException)
{
    ASSERT_THROW(Preprocessor ("something",0,0), std::invalid_argument);
}

TEST(PreprocessorTest, CreatePreprocessor_CreateZeroIntervalRange_ThrowsException)
{
    ASSERT_THROW(Preprocessor ("something",1337,0), std::invalid_argument);
}

TEST(PreprocessorTest, PerformPreprocessing_InvalidFilePath_ThrowsException)
{
    Preprocessor p("something", 1337, 1337);
    ASSERT_THROW(p.getPreprocessedData(), std::runtime_error);
}

TEST(PreprocessorTest, PerformPreprocessing_InvalidJsonFile_ThrowsException)
{
    std::vector<std::vector<std::string>> invalidJsonContent{
        {"null"},
        {"{}"},
        {R"({"_id":{"$oid":"5b6ac3d663bfd384de2361c1"}})"},
    };

    for (const auto& jsonData : invalidJsonContent)
    {
        TmpJsonFile tmp{jsonData};
        Preprocessor p(tmp.path(), 1337, 1337);
        ASSERT_ANY_THROW(p.getPreprocessedData()); // framework-agnostic exception
    }
}

TEST(PreprocessorTest, PerformPreprocessing_ValidJsonFile_GenarateValidPreprocessingData)
{
    std::vector<std::string> validJsonContent{
        R"({"time":{"$numberLong":"10"},"bid":{"$numberInt":"1000000"},"ask":{"$numberInt":"1000000"},"bidVolume":{"$numberInt":"1000000"},"askVolume":{"$numberInt":"1000000"}})",
        R"({"time":{"$numberLong":"20"},"bid":{"$numberInt":"2000000"},"ask":{"$numberInt":"2000000"},"bidVolume":{"$numberInt":"2000000"},"askVolume":{"$numberInt":"2000000"}})",
        R"({"time":{"$numberLong":"30"},"bid":{"$numberInt":"3000000"},"ask":{"$numberInt":"3000000"},"bidVolume":{"$numberInt":"3000000"},"askVolume":{"$numberInt":"3000000"}})",
        R"({"time":{"$numberLong":"40"},"bid":{"$numberInt":"4000000"},"ask":{"$numberInt":"4000000"},"bidVolume":{"$numberInt":"4000000"},"askVolume":{"$numberInt":"4000000"}})",
        R"({"time":{"$numberLong":"50"},"bid":{"$numberInt":"5000000"},"ask":{"$numberInt":"5000000"},"bidVolume":{"$numberInt":"5000000"},"askVolume":{"$numberInt":"5000000"}})",
        R"({"time":{"$numberLong":"60"},"bid":{"$numberInt":"6000000"},"ask":{"$numberInt":"6000000"},"bidVolume":{"$numberInt":"6000000"},"askVolume":{"$numberInt":"6000000"}})",
    };

    TmpJsonFile tmp{validJsonContent};
    Preprocessor p(tmp.path(), 1337, 15);
    ASSERT_NO_THROW(p.getPreprocessedData());
}
