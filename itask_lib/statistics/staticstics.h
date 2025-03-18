#ifndef STATICSTICS_H
#define STATICSTICS_H

#include "metrics.h"
#include "utils/types/types.h"

namespace itask::statistics
{
    using namespace itask::utils::types;

    /**
     * @class Statistics
     * @brief Collects and computes statistical data for a specific time interval.
     *
     * The Statistics class is responsible for aggregating bids and asks price statistics
     * for specific TimeInterval.
     */
    class Statistics
    {
    public:
        Statistics() = delete;
        Statistics(const Statistics&) = delete;
        Statistics& operator=(const Statistics&) = delete;

        ~Statistics() = default;

        /**
         * @brief Constructs a Statistics instance for a specific time interval.
         *
         * @param interval The time interval for which statistics will be collected.
         *
         * @throws If end of the interval is lower then start
         */
        Statistics(TimeInterval interval);

        /**
         * @brief Move constructor.
         *
         * Allows Statistics to be moved.
         */
        Statistics(Statistics&&) noexcept;

        /**
         * @brief Move assignment operator.
         *
         * Enables move assignment for Statistics.
         */
        Statistics& operator=(Statistics&&) noexcept;

        /**
         * @brief Adds a new quote to the statistics computation.
         *
         * This method updates the current statistical values based on the given Quote.
         *
         * @param quote The quote data to be incorporated into the statistics.
         */
        void addQuote(Quote quote);

        /**
         * @brief Retrieves the computed statistics for the interval.
         *
         * Returns an IntervalStatistics object containing min, max, average,
         * median prices, and total volume for both bid and ask.
         *
         * @return Computed statistics for the stored time interval.
         */
        IntervalStatistics getStatistics() const;

    private:
        TimeInterval timeInterval_{};
        StatMetrics askMetrics_{};
        StatMetrics bidMetrics_{};
        double askVolume_{0};
        double bidVolume_{0};
    };
}

#endif //STATICSTICS_H
