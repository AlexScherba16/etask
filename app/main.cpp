#include "cli/cli_parser.h"
#include "preprocessor/preprocessor.h"
#include "mapper/mapper.h"
#include "reducer/reducer.h"
#include "aggregator/aggregator.h"

#include <asio.hpp>

#include <chrono>
#include <iostream>

constexpr uint64_t THIRTY_MIN_IN_NANO_SECONDS{1'800'000'000'000};
constexpr uint64_t MAPPER_CHANNEL_CAPACITY{4096};

int main(int argc, char** argv)
{
    using namespace itask::mapper;
    using namespace itask::reducer;
    using namespace itask::cli_parser;
    using namespace itask::preprocessor;
    using namespace itask::aggregator;

    auto START = std::chrono::high_resolution_clock::now();

    try
    {
        auto threadCount{static_cast<uint16_t>(std::thread::hardware_concurrency())};
        threadCount = threadCount == 0 ? 4 : threadCount;

        // parse args, spinup preprocessing
        const auto args{CliParser{"itask", "Ingenium coding task"}.parse(argc, argv)};
        const auto preprocData{
            Preprocessor{args.jsonFilePath, threadCount, THIRTY_MIN_IN_NANO_SECONDS}.getPreprocessedData()};

        // prepare channels map for data transfer Mappers -> Reducers,
        // each Reducer is responsible for proper interval
        QuoteChannelsMap quotesChannelsMap;
        AggregatedStatistics aggregatedStatistics;
        {
            const auto timeMetadata{preprocData.timeIntervalSet.timeIntervalMetadata};

            quotesChannelsMap.reserve(timeMetadata.intervalsValue);
            aggregatedStatistics.reserve(timeMetadata.intervalsValue);
            for (uint32_t i = 0; i < timeMetadata.intervalsValue; ++i)
            {
                quotesChannelsMap.emplace_back(QuoteChannel{MAPPER_CHANNEL_CAPACITY});
                aggregatedStatistics.emplace_back(IntervalStatistics{});
            }
        }

        // prepare the thread pool and start Mappers and Reducers.
        asio::thread_pool threadPool(threadCount);

        const auto mappersValue{static_cast<uint32_t>(preprocData.fileSegments.size())};
        const auto reducersValue{static_cast<uint32_t>(preprocData.timeIntervalSet.timeIntervals.size())};
        std::latch mappersDoneLatch{static_cast<ptrdiff_t>(mappersValue)};
        std::latch reducersDoneLatch{static_cast<ptrdiff_t>(reducersValue)};

        // launching threads in sequence:
        // Mapper...NMapper - Reducer - Mapper...NMapper - Reducer, etc.

        // we launch more Mappers than Reducers to minimize starvation
        // and reduce active polling on the queue.
        //
        // On my machine, launching all available Mappers turned out to be
        // the most efficient approach.
        const uint32_t LAUNCH_MIN_MAPPERS{8};
        {

            // nothing special here, create Mappers, Reducers and post them in thread pool
            int i = 0, j = 0;
            while (i < mappersValue || j < reducersValue)
            {
                auto mappersLaunchSequence = std::min(LAUNCH_MIN_MAPPERS, mappersValue - i);
                for (int k = 0; k < mappersLaunchSequence; ++k, ++i)
                {
                    Mapper m(args.jsonFilePath, std::move(preprocData.fileSegments[i]), preprocData.timeIntervalSet,
                             quotesChannelsMap,
                             mappersDoneLatch);
                    asio::post(threadPool, std::move(m));
                }

                if (j < reducersValue)
                {
                    Reducer r(j, std::move(preprocData.timeIntervalSet.timeIntervals[j]), quotesChannelsMap,
                              aggregatedStatistics, reducersDoneLatch);
                    asio::post(threadPool, std::move(r));
                    ++j;
                }
            }
        }

        // wait until mappers complete their job
        mappersDoneLatch.wait();

        // the Mapping process is done, no more data will be streamed.
        // let's notify Reducers stop.
        for (int i = 0; i < mappersValue; ++i)
        {
            quotesChannelsMap[i].enqueue(std::nullopt);
        }

        // waiting for Reducers
        reducersDoneLatch.wait();

        // shutdown threadpool and print results
        threadPool.join();
        Aggregator::printJson(aggregatedStatistics);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    auto STOP = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(STOP - START);
    std::cout << duration.count() << "ms" << std::endl;

    return EXIT_SUCCESS;
}
