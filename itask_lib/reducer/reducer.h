#ifndef REDUCER_H
#define REDUCER_H

#include "statistics/staticstics.h"

#include <latch>

namespace itask::reducer
{
    using namespace itask::utils::types;
    using namespace itask::statistics;

    /**
     * @class Reducer
     * @brief Collects data from mappers stream and computes aggregated statistics.
     *
     * The Reducer class is responsible for:
     * - Retrieving parsed Quote data produced by Mappers.
     * - Aggregating statistical metrics over a specific TimeInterval.
     * - Storing the computed statistics in the proper AggregatedStatistics position.
     *
     * This class is designed to operate as a callable (operator()), making it
     * suitable for execution in a separate thread.
     *
     * @note: only movable
     */
    class Reducer
    {
    public:
        Reducer() = delete;
        Reducer(const Reducer&) = delete;
        Reducer& operator=(const Reducer&) = delete;

        ~Reducer() = default;

        /**
         * @brief Constructs a Reducer instance for a given interval.
         *
         * @param id Unique identifier for the reducer.
         *        Refers to the channel index in QuoteChannelsMap from which data should be read
         *        and the position in AggregatedStatistics where the computed statistics should be stored.
         * @param timeInterval The time interval over which statistics will be computed.
         * @param qChanMap Reference to the collection of quote channels.
         * @param aggrStat Reference to the aggregated statistics storage.
         * @param latch A synchronization latch to signal completion.
         *
         * @note ❗❗❗IMPORTANT❗❗❗ It is SUPER CRUCIAL to ensure that the referenced objects
         * (QuoteChannelsMap, AggregatedStatistics, and std::latch) remain valid throughout the lifetime of
         * this Mapper instance. Dangling references will lead to undefined behavior.
         *
         * References are used instead of shared_ptr to avoid unnecessary pointer dereferencing overhead.
         */
        Reducer(uint64_t id, TimeInterval timeInterval,
                QuoteChannelsMap& qChanMap, AggregatedStatistics& aggrStat, std::latch& latch);
        /**
         * @brief Move constructor.
         *
         * Allows Reducer to be moved.
         */
        Reducer(Reducer&&) noexcept;

        /**
         * @brief Move assignment operator.
         *
         * Enables move assignment for Reducer.
         */
        Reducer& operator=(Reducer&&) noexcept;

        /**
         * @brief Executes the reducing process.
         *
         * This function preforms:
         * - Retrieves Quotes from the assigned QuoteChannel,
         * - Performs statistical metrics computation,
         * - Waits end of stream notification
         * - Stores the results in proper AggregatedStatistics position.
         */
        void operator()();

    private:
        uint64_t reducerId_{0};
        std::reference_wrapper<QuoteChannelsMap> quotesChannelsMapRef_;
        std::reference_wrapper<AggregatedStatistics> aggregatedStatisticsRef_;
        std::reference_wrapper<std::latch> latchRef_;
        Statistics statistics_;
    };
}

#endif //REDUCER_H
