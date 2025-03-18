#include "reducer/reducer.h"
#include "utils/types/types.h"

#include <gtest/gtest.h>
#include <thread>

using namespace testing;
using namespace itask::reducer;

TEST(ReducerTest, CreateReducer_IntervalEndIsLowerThanStart_ThrowsException)
{
    QuoteChannelsMap quotesChannelsMap{};
    AggregatedStatistics aggrStat{};
    std::latch latch{0};
    ASSERT_THROW(Reducer(0, {1337, 0}, quotesChannelsMap, aggrStat, latch ), std::invalid_argument);
}

TEST(ReducerTest, CreateReducer_EmptyChannelsMap_ThrowsException)
{
    QuoteChannelsMap quotesChannelsMap{};
    AggregatedStatistics aggrStat{};
    std::latch latch{0};
    ASSERT_THROW(
        Reducer(0, {0, 1337}, quotesChannelsMap, aggrStat, latch ), std::invalid_argument);
}

TEST(ReducerTest, CreateReducer_EmptyAggregatedStatistics_ThrowsException)
{
    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});

    AggregatedStatistics aggrStat{};
    std::latch latch{0};
    ASSERT_THROW(Reducer(0, {0, 1337}, quotesChannelsMap, aggrStat, latch ), std::invalid_argument);
}

TEST(ReducerTest, CreateReducer_IdIsOutOfChannelsBound_ThrowsException)
{
    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});

    AggregatedStatistics aggrStat{IntervalStatistics{}};
    std::latch latch{1};
    ASSERT_THROW(Reducer(1337, {0, 1337}, quotesChannelsMap, aggrStat, latch ), std::invalid_argument);
}

TEST(ReducerTest, CreateReducer_IdIsOutOfAggregatedStatisticsBound_ThrowsException)
{
    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});
    quotesChannelsMap.emplace_back(QuoteChannel{});

    AggregatedStatistics aggrStat{IntervalStatistics{}};
    std::latch latch{0};
    ASSERT_THROW(Reducer(1, {0, 1337}, quotesChannelsMap, aggrStat, latch ), std::invalid_argument);
}

TEST(ReducerTest, CreateReducer_ValidParameters_NoThrowsException)
{
    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});

    AggregatedStatistics aggrStat{IntervalStatistics{}};
    std::latch latch{0};
    ASSERT_NO_THROW(Reducer(0, {0, 1337}, quotesChannelsMap, aggrStat, latch ));
}

TEST(ReducerTest, PerformReducing_StopReducingAfterNullopt)
{
    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});

    AggregatedStatistics aggrStat{IntervalStatistics{}};
    std::latch latch{1};
    Reducer r(0, {0, 1337}, quotesChannelsMap, aggrStat, latch);

    std::jthread producer([&]()
    {
        std::vector<std::optional<Quote>> producedQuotes{Quote{}, Quote{}, Quote{}, std::nullopt};
        for (auto& quote : producedQuotes)
        {
            quotesChannelsMap[0].enqueue(quote);
        }
    });
    std::jthread consumer(std::move(r));

    latch.wait();
    producer.join();
    consumer.join();
}
