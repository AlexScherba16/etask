#include "preprocessor/preprocessor.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace itask::preprocessor;

TEST(PreprocessorTest, CreatePreprocessor_EmptyPath_ThrowsException) {
    ASSERT_THROW(Preprocessor ("",1), std::invalid_argument);
}

TEST(PreprocessorTest, CreatePreprocessor_CreateZeroThread_ThrowsException) {
    ASSERT_THROW(Preprocessor ("test",0), std::invalid_argument);
}
