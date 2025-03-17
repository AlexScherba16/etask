#include "mapper/mapper.h"
#include "utils/misc/misc.h"
#include "utils/types/types.h"
#include "utils/filesystem/filesystem.h"
#include "preprocessor/preprocessor.h"

#include <gtest/gtest.h>

#include <thread>
#include <stop_token>

using namespace testing;
using namespace itask::mapper;
using namespace itask::utils::misc;
using namespace itask::preprocessor;
using namespace itask::utils::types;
using namespace itask::util::filesystem;

const std::vector<std::string> GLOBAL_VALID_JSON_DATA{
    R"({"time":{"$numberLong":"1"},"bid":{"$numberInt":"1000000"},"ask":{"$numberInt":"1000000"},"bidVolume":{"$numberInt":"1000"},"askVolume":{"$numberInt":"1000"}})",
    R"({"time":{"$numberLong":"2"},"bid":{"$numberInt":"2000000"},"ask":{"$numberInt":"2000000"},"bidVolume":{"$numberInt":"2000"},"askVolume":{"$numberInt":"2000"}})",
    R"({"time":{"$numberLong":"3"},"bid":{"$numberInt":"3000000"},"ask":{"$numberInt":"3000000"},"bidVolume":{"$numberInt":"3000"},"askVolume":{"$numberInt":"3000"}})",
    R"({"time":{"$numberLong":"4"},"bid":{"$numberInt":"4000000"},"ask":{"$numberInt":"4000000"},"bidVolume":{"$numberInt":"4000"},"askVolume":{"$numberInt":"4000"}})",
    R"({"time":{"$numberLong":"5"},"bid":{"$numberInt":"5000000"},"ask":{"$numberInt":"5000000"},"bidVolume":{"$numberInt":"5000"},"askVolume":{"$numberInt":"5000"}})",
    R"({"time":{"$numberLong":"6"},"bid":{"$numberInt":"6000000"},"ask":{"$numberInt":"6000000"},"bidVolume":{"$numberInt":"6000"},"askVolume":{"$numberInt":"6000"}})",
};

const std::vector<Quote> GLOBAL_EXPECTED_QUOTES{
    {1, 1, 1, 1, 1},
    {2, 2, 2, 2, 2},
    {3, 3, 3, 3, 3},
    {4, 4, 4, 4, 4},
    {5, 5, 5, 5, 5},
    {6, 6, 6, 6, 6},
};


TEST(MapperTest, CreateMapper_EmptyPath_ThrowsException)
{
    TimeIntervalSet timeSet{};
    QuoteChannelsMap quotesChannelsMap{};
    std::latch latch{0};
    ASSERT_THROW(Mapper("", std::move(FileSegment{}),timeSet, quotesChannelsMap, latch), std::invalid_argument);
}

TEST(MapperTest, CreateMapper_InvalidFilePath_ThrowsException)
{
    TimeIntervalSet timeSet{};
    QuoteChannelsMap quotesChannelsMap{};
    std::latch latch{0};
    ASSERT_THROW(Mapper("something", std::move(FileSegment{}), timeSet, quotesChannelsMap, latch),
                 std::filesystem::filesystem_error);
}

TEST(MapperTest, CreateMapper_EmptyFile_ThrowsException)
{
    TmpEmptyFile tmp{};
    TimeIntervalSet timeSet{};
    QuoteChannelsMap quotesChannelsMap{};
    std::latch latch{0};
    ASSERT_THROW(Mapper(tmp.path(), std::move(FileSegment{}), timeSet, quotesChannelsMap,latch), std::invalid_argument);
}

TEST(MapperTest, CreateMapper_EndIsLowerThanStart_ThrowsException)
{
    TmpJsonFile tmp{{R"({"dont_care_data":{"something":"here"}})"}};
    TimeIntervalSet timeSet{};
    QuoteChannelsMap quotesChannelsMap{};
    std::latch latch{0};
    ASSERT_THROW(Mapper(tmp.path(), {100, 10}, timeSet, quotesChannelsMap, latch), std::invalid_argument);
}

TEST(MapperTest, CreateMapper_EmptyChannels_ThrowsException)
{
    TmpJsonFile tmp{{R"({"dont_care_data":{"something":"here"}})"}};
    TimeIntervalSet timeSet{};
    QuoteChannelsMap quotesChannelsMap{};
    std::latch latch{0};
    ASSERT_THROW(Mapper(tmp.path(), {100, 10}, timeSet, quotesChannelsMap, latch), std::invalid_argument);
}

TEST(MapperTest, CreateMapper_EmptyTotalDuration_ThrowsException)
{
    TmpJsonFile tmp{{R"({"dont_care_data":{"something":"here"}})"}};
    TimeIntervalSet timeSet{};
    QuoteChannelsMap quotesChannelsMap{};
    std::latch latch{0};
    ASSERT_THROW(Mapper(tmp.path(), {100, 10}, timeSet, quotesChannelsMap, latch), std::invalid_argument);
}

TEST(MapperTest, PerformMapping_InvalidJsonFile_NoThrowException)
{
    std::vector<std::vector<std::string>> invalidJsonContent{
        {"null"},
        {"{}"},
        {R"({"_id":{"$oid":"5b6ac3d663bfd384de2361c1"}})"},
    };

    // Suppress cerr during testing, cuz I'm not using proper logger :)
    std::ostringstream buffer;
    std::streambuf* oldCerr = std::cerr.rdbuf(buffer.rdbuf());
    Defer restoreCerr(std::move([oldCerr]() { std::cerr.rdbuf(oldCerr); }));

    for (const auto& jsonData : invalidJsonContent)
    {
        TmpJsonFile tmp{jsonData};
        TimeIntervalSet timeSet{{}, {0, 0, 0, 1}};

        QuoteChannelsMap quotesChannelsMap{};
        quotesChannelsMap.emplace_back(QuoteChannel{});
        std::latch latch{0};
        Mapper m(tmp.path(), std::move(FileSegment{0, 100}), timeSet, quotesChannelsMap, latch);
        ASSERT_NO_THROW(m());
    }
}

TEST(MapperTest, PerformMapping_ValidJsonFile_SPSC_Stream_SingleInterval)
{
    TmpJsonFile tmp{GLOBAL_VALID_JSON_DATA};
    auto preprocData{Preprocessor{tmp.path(), 1, 10}.getPreprocessedData()};

    std::vector<Quote> actualQuotes;
    actualQuotes.reserve(GLOBAL_VALID_JSON_DATA.size());

    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});
    std::latch latch{1};
    Mapper m(tmp.path(), std::move(FileSegment{0, tmp.size()}), preprocData.timeIntervalSet, quotesChannelsMap, latch);

    std::jthread producer{std::move(m)};
    std::jthread consumer = std::jthread([&]()
    {
        while (true)
        {
            std::optional<Quote> tmp;
            if (quotesChannelsMap[0].try_dequeue(tmp))
            {
                if (!tmp.has_value())
                {
                    // end of streaming
                    return;
                }
                actualQuotes.emplace_back(std::move(tmp.value()));
            }
        }
    });

    latch.wait();
    quotesChannelsMap[0].enqueue(std::nullopt);
    consumer.join();
    ASSERT_EQ(GLOBAL_EXPECTED_QUOTES, actualQuotes);
}

TEST(MapperTest, PerformMapping_ValidJsonFile_SPMC_Stream_TwoIntervals)
{
    TmpJsonFile tmp{GLOBAL_VALID_JSON_DATA};
    auto preprocData{Preprocessor{tmp.path(), 1, 3}.getPreprocessedData()};

    std::vector<Quote> expectedQuotes_interval_0{
        {1, 1, 1, 1, 1},
        {2, 2, 2, 2, 2},
        {3, 3, 3, 3, 3},
    };
    std::vector<Quote> expectedQuotes_interval_1{
        {4, 4, 4, 4, 4},
        {5, 5, 5, 5, 5},
        {6, 6, 6, 6, 6},
    };

    std::vector<Quote> actualQuotes_interval_0;
    actualQuotes_interval_0.reserve(expectedQuotes_interval_0.size());

    std::vector<Quote> actualQuotes_interval_1;
    actualQuotes_interval_1.reserve(expectedQuotes_interval_1.size());

    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});
    quotesChannelsMap.emplace_back(QuoteChannel{});

    std::latch latch{1};
    Mapper m(tmp.path(), std::move(FileSegment{0, tmp.size()}), preprocData.timeIntervalSet, quotesChannelsMap, latch);

    auto consumerJob = [&quotesChannelsMap](int channelIndex, std::vector<Quote>& storage)
    {
        while (true)
        {
            std::optional<Quote> tmp;
            if (quotesChannelsMap[channelIndex].try_dequeue(tmp))
            {
                if (!tmp.has_value())
                {
                    // end of streaming
                    return;
                }
                storage.emplace_back(std::move(tmp.value()));
            }
        }
    };

    std::jthread consumer_0{consumerJob, 0, std::ref(actualQuotes_interval_0)};
    std::jthread consumer_1{consumerJob, 1, std::ref(actualQuotes_interval_1)};
    std::jthread producer{std::move(m)};

    latch.wait();
    quotesChannelsMap[0].enqueue(std::nullopt);
    quotesChannelsMap[1].enqueue(std::nullopt);

    consumer_0.join();
    consumer_1.join();

    ASSERT_EQ(expectedQuotes_interval_0, actualQuotes_interval_0);
    ASSERT_EQ(expectedQuotes_interval_1, actualQuotes_interval_1);
}

TEST(MapperTest, PerformMapping_ValidJsonFile_MPSC_Stream_SinsleInterval)
{
    TmpJsonFile tmp{GLOBAL_VALID_JSON_DATA};
    auto preprocData{Preprocessor{tmp.path(), 2, 10}.getPreprocessedData()};

    std::vector<Quote> actualQuotes;
    actualQuotes.reserve(GLOBAL_EXPECTED_QUOTES.size());

    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});

    std::latch latch{static_cast<ptrdiff_t>(preprocData.fileSegments.size())};
    std::vector<std::thread> producerThreads;
    producerThreads.reserve(preprocData.fileSegments.size());

    for (const auto& segment : preprocData.fileSegments)
    {
        Mapper m(tmp.path(), std::move(segment), preprocData.timeIntervalSet, quotesChannelsMap, latch);
        producerThreads.emplace_back(std::move(m));
    }

    std::jthread consumer{[&]()
    {
        while (true)
        {
            std::optional<Quote> tmp;
            if (quotesChannelsMap[0].try_dequeue(tmp))
            {
                if (!tmp.has_value())
                {
                    // end of streaming
                    return;
                }
                actualQuotes.emplace_back(std::move(tmp.value()));
            }
        }
    }};

    latch.wait();
    quotesChannelsMap[0].enqueue(std::nullopt);
    consumer.join();
    for (auto& thread : producerThreads)
    {
        thread.join();
    }

    std::sort(actualQuotes.begin(), actualQuotes.end());
    ASSERT_EQ(GLOBAL_EXPECTED_QUOTES, actualQuotes);
}

TEST(MapperTest, PerformMapping_ValidJsonFile_MPMC_Stream_TwoIntervals)
{
    TmpJsonFile tmp{GLOBAL_VALID_JSON_DATA};
    auto preprocData{Preprocessor{tmp.path(), 2, 3}.getPreprocessedData()};

    std::vector<Quote> expectedQuotes_interval_0{
        {1, 1, 1, 1, 1},
        {2, 2, 2, 2, 2},
        {3, 3, 3, 3, 3},
    };
    std::vector<Quote> expectedQuotes_interval_1{
        {4, 4, 4, 4, 4},
        {5, 5, 5, 5, 5},
        {6, 6, 6, 6, 6},
    };

    std::vector<Quote> actualQuotes_interval_0;
    actualQuotes_interval_0.reserve(expectedQuotes_interval_0.size());

    std::vector<Quote> actualQuotes_interval_1;
    actualQuotes_interval_1.reserve(expectedQuotes_interval_1.size());

    QuoteChannelsMap quotesChannelsMap{};
    quotesChannelsMap.emplace_back(QuoteChannel{});
    quotesChannelsMap.emplace_back(QuoteChannel{});

    std::latch latch{static_cast<ptrdiff_t>(preprocData.fileSegments.size())};
    std::vector<std::thread> producerThreads;
    producerThreads.reserve(preprocData.fileSegments.size());

    for (const auto& segment : preprocData.fileSegments)
    {
        Mapper m(tmp.path(), std::move(segment), preprocData.timeIntervalSet, quotesChannelsMap, latch);
        producerThreads.emplace_back(std::move(m));
    }

    auto consumerJob = [&quotesChannelsMap](int channelIndex, std::vector<Quote>& storage)
    {
        while (true)
        {
            std::optional<Quote> tmp;
            if (quotesChannelsMap[channelIndex].try_dequeue(tmp))
            {
                if (!tmp.has_value())
                {
                    // end of streaming
                    return;
                }
                storage.emplace_back(std::move(tmp.value()));
            }
        }
    };
    std::jthread consumer_0{consumerJob, 0, std::ref(actualQuotes_interval_0)};
    std::jthread consumer_1{consumerJob, 1, std::ref(actualQuotes_interval_1)};

    latch.wait();
    quotesChannelsMap[0].enqueue(std::nullopt);
    quotesChannelsMap[1].enqueue(std::nullopt);

    consumer_0.join();
    consumer_1.join();
    for (auto& thread : producerThreads)
    {
        thread.join();
    }

    std::sort(actualQuotes_interval_0.begin(), actualQuotes_interval_0.end());
    std::sort(actualQuotes_interval_1.begin(), actualQuotes_interval_1.end());

    ASSERT_EQ(expectedQuotes_interval_0, actualQuotes_interval_0);
    ASSERT_EQ(expectedQuotes_interval_1, actualQuotes_interval_1);
}
