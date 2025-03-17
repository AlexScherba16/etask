#include <iostream>

#include "cli/cli_parser.h"
#include "preprocessor/preprocessor.h"
#include "mapper/mapper.h"

constexpr uint64_t THIRTY_MIN_IN_NANO_SECONDS{1'800'000'000'000};
constexpr uint64_t MAPPER_CHANNEL_CAPACITY{2048};

int main(int argc, char** argv)
{
    using namespace itask::mapper;
    using namespace itask::cli_parser;
    using namespace itask::preprocessor;

    try
    {
        auto threadCount{static_cast<uint16_t>(std::thread::hardware_concurrency())};
        threadCount = threadCount == 0 ? 4 : threadCount;

        const auto args{CliParser{"itask", "Ingenium coding task"}.parse(argc, argv)};
        const auto preprocData{
            Preprocessor{args.jsonFilePath, threadCount, THIRTY_MIN_IN_NANO_SECONDS}.getPreprocessedData()};

        // prepare channels map for data transfer Mappers -> Reducers,
        // each Reducer is responsible for proper interval
        QuoteChannelsMap quotesChannelsMap;
        {
            const auto timeMetadata{preprocData.timeIntervalSet.timeIntervalMetadata};
            quotesChannelsMap.reserve(timeMetadata.intervalsValue);
            for (uint32_t i = 0; i < timeMetadata.intervalsValue; ++i)
            {
                quotesChannelsMap.emplace_back(QuoteChannel{MAPPER_CHANNEL_CAPACITY});
            }
        }

        // channels map is ready, create Mappers
        std::vector<std::thread> producerThreads;
        producerThreads.reserve(threadCount);

        std::latch mappersDoneLatch{threadCount};
        for (auto& segment : preprocData.fileSegments)
        {
            Mapper m(args.jsonFilePath, std::move(segment), preprocData.timeIntervalSet, quotesChannelsMap,
                     mappersDoneLatch);
            producerThreads.emplace_back(std::move(m));
        }

        mappersDoneLatch.wait();

        for (auto& producerThread : producerThreads)
        {
            producerThread.join();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
