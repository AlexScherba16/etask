#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include "utils/types/types.h"

namespace itask:: aggregator
{
    using namespace itask::utils::types;

    /**
     * @class Aggregator
     * @brief Helper class for formatting and printing aggregated statistics.
     *
     * The Aggregator class provides utility functions to convert aggregated
     * statistical data into a readable JSON format and print it to the output.
     */
    class Aggregator
    {
    public:
        /**
         * @brief Prints aggregated statistics in JSON format.
         *
         * This function converts the given AggregatedStatistics data into a JSON-formatted
         * string and prints it to the standard output.
         *
         * @param stat The aggregated statistics to be printed.
         */
        static void printJson(const AggregatedStatistics& stat);

    private:
        /**
         * @brief Converts a time interval to an H:M:S formatted string.
         *
         * This function takes a TimeInterval and returns a human-readable string
         * representing the time range in Hours:Minutes:Seconds format.
         *
         * @param interval The time interval to format.
         * @return A string representing the interval in H:M:S format.
         */
        static std::string intervalTo_H_M_S_Format(const TimeInterval& interval);
    };
}
#endif //AGGREGATOR_H
