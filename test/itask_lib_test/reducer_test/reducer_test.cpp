#include "reducer/reducer.h"
#include "utils/types/types.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace itask::reducer;

TEST(ReducerTest, CreateReducer_EmptyChannelsMap_ThrowsException)
{
    QuoteChannelsMap quotesChannelsMap{};
    ASSERT_THROW(Reducer(0, {}, quotesChannelsMap ), std::invalid_argument);
}

TEST(ReducerTest, CreateReducer_IdIsOutOfBound_ThrowsException)
{
    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});
    ASSERT_THROW(Reducer(1337, {}, quotesChannelsMap ), std::invalid_argument);
}

TEST(ReducerTest, CreateReducer_EndIsLowerThanStart_ThrowsException)
{
    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});
    ASSERT_THROW(Reducer(0, {1337, 0}, quotesChannelsMap ), std::invalid_argument);
}
