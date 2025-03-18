#ifndef METRICS_H
#define METRICS_H

#include "utils/types/types.h"

namespace itask::statistics
{
    /**
     * @class StatMetrics
     * @brief Helper class for Statistics computation.
     * Computes and stores statistical metrics for a data stream.
     *
     * The StatMetrics class is responsible for dynamically computing statistical
     * values such as minimum, maximum, average, and median for an incoming stream of numbers.
     */
    class StatMetrics
    {
    public:
        StatMetrics() = default;
        ~StatMetrics() = default;

        StatMetrics(const StatMetrics&) = delete;
        StatMetrics& operator=(const StatMetrics&) = delete;

        /**
         * @brief Move constructor.
         *
         * Allows StatMetrics to be moved.
         */
        StatMetrics(StatMetrics&&) noexcept;

        /**
         * @brief Move assignment operator.
         *
         * Enables move assignment for StatMetrics.
         */
        StatMetrics& operator=(StatMetrics&&) noexcept;

        /**
         * @brief Adds a new number to the statistical computation.
         *
         * This method updates the internal metrics, including min, max, median, and average.
         *
         * @param num The number to be added to the dataset.
         */
        void addNum(double num);

        /**
         * @brief Retrieves the minimum value in the dataset.
         * @return The smallest recorded value.
         */
        double getMin() const;

        /**
         * @brief Retrieves the maximum value in the dataset.
         * @return The largest recorded value.
         */
        double getMax() const;

        /**
         * @brief Retrieves the median value in the dataset.
         * @return The median value.
         */
        double getMedian() const;

        /**
         * @brief Retrieves the median value in the dataset.
         * @return The median value.
         */
        double getAverage() const;

        /**
         * @brief Clears all collected and computed data
         */
        void clear();

    private:
        std::priority_queue<double> maxHeap_;
        std::priority_queue<double, std::vector<double>, std::greater<double>> minHeap_;

        double globalMinVal_{std::numeric_limits<double>::max()};
        double globalMaxVal_{std::numeric_limits<double>::lowest()};
        double valCounter_{0};
        double valSum_{0};
    };
}
#endif //METRICS_H
