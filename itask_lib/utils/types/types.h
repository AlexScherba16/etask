#ifndef TYPES_H
#define TYPES_H

#include <queue>
#include <vector>

#include <concurrentqueue.h>

namespace itask::utils::types
{
    /**
    * @struct CliArgs
    * @brief Structure for storing command-line arguments.
    *
    * This structure is used to store the file path to a JSON file
    * provided as a command-line argument.
    */
    struct CliArgs
    {
        std::string jsonFilePath{};
    };

    /**
     * @struct FileSegment
     * @brief Represents a segment of a file.
     *
     * A file segment defines a specific byte range within a file,
     * Expected to use for partitioning the file into smaller chunks
     * for parallel processing.
     */
    struct FileSegment
    {
        size_t startOffset{0};
        size_t endOffset{0};
    };

    /**
     * @struct TimeInterval
     * @brief Represents a time interval.
     *
     * A time interval defines a range of timestamps that are used
     * to categorize records into specific time-based partitions.
     */
    struct TimeInterval
    {
        uint64_t startTimestampNs;
        uint64_t endTimestampNs;
    };

    /**
     * @struct TimeIntervalMetadata
     * @brief Stores metadata related to time intervals.
     *
     * This structure contains global parameters that define the interval settings,
     * including the total number of intervals, the first and last timestamps in the dataset,
     * and the defined length of each interval.
     */
    struct TimeIntervalMetadata
    {
        uint64_t intervalsValue{0}; // total number of intervals.
        uint64_t globalStartTimestampNs{0}; // timestamp of the first record in the file.
        uint64_t globalEndTimestampNs{0}; // timestamp of the last record in the file.
        uint64_t intervalLengthNs{0}; // defined length of each interval.
    };

    /**
     * @struct TimeIntervalSet
     * @brief Represents a set of time intervals with global time boundaries in nanoseconds.
     *
     * This structure defines a collection of time intervals, calculated based on
     * a specified interval length. It also stores the global start and end timestamps
     * of the dataset.
     */
    struct TimeIntervalSet
    {
        std::vector<TimeInterval> timeIntervals;
        TimeIntervalMetadata timeIntervalMetadata;
    };

    /**
     * @struct PreprocessedData
     * @brief Stores preprocessed data for file segmentation and time-based intervals.
     *
     * This structure holds the result of preprocessing, including file partitions
     * and predefined time intervals for further processing.
     */
    struct PreprocessedData
    {
        std::vector<FileSegment> fileSegments;
        TimeIntervalSet timeIntervalSet;
    };

    /**
     * @struct Quote
     * @brief Represents a currency pair quote
     *
     * The quote contains information about the timestamp, bid/ask prices, and bid/ask volumes
     * struct contains raw marked data
     *
     * According to task description for later processing struct fields must be transformed as mentioned below,
     * before it can be used:
     * - bid and ask are divided by 1,000,000
     * - bidVolume and askVolume are divided by 1,000
     */
    struct Quote
    {
        uint64_t timeNs{0};
        double bid{0};
        double ask{0};
        double bidVolume{0};
        double askVolume{0};

        /**
         * @brief
         *
         * Well, in honor of general code documentation.
         * However, I guess nobody will read this to clarify what actually operator== is doing, lol.
         */
        bool operator==(const Quote& o) const
        {
            return timeNs == o.timeNs &&
                bid == o.bid &&
                ask == o.ask &&
                bidVolume == o.bidVolume &&
                askVolume == o.askVolume;
        }

        /**
         * @brief
         *
         * Same as above.
         */
        bool operator<(const Quote& o) const
        {
            return timeNs < o.timeNs && bid < o.bid && ask < o.ask && bidVolume < o.bidVolume;
        }
    };

    /**
     * @brief A thread-safe queue for transmitting Quote data.
     *
     * This queue allows safe communication between producer and consumer threads
     * using `std::optional<Quote>`, where `std::nullopt` is using to send
     * end-of-stream signal, similar to Golang empty struct transition.
     */
    using QuoteChannel = moodycamel::ConcurrentQueue<std::optional<Quote>>;

    /**
     * @brief A collection of quote channels for parallel data processing.
     *
     * This structure represents multiple independent `QuoteChannel` instances,
     * using to send Quote messages across different consumers.
     * vector is chosen to avoid hash calculations like in classical mapping containers.
     */
    using QuoteChannelsMap = std::vector<QuoteChannel>;

    /**
     * @struct IntervalStatistics
     * @brief Stores statistical data for a specific time interval.
     *
     * This structure holds aggregated bid and ask statistics for a given time interval.
     * It includes maximum, minimum, average, median, and total volume values for both
     * bid and ask prices.
     */
    struct IntervalStatistics
    {
        TimeInterval timeInterval{};
        double askMax{0};
        double askMin{0};
        double askAverage{0};
        double askMedian{0};
        double askVolume{0};

        double bidMax{0};
        double bidMin{0};
        double bidAverage{0};
        double bidMedian{0};
        double bidVolume{0};
    };

    /**
     * @brief A collection of aggregated statistics across multiple intervals.
     *
     * This type represents a series of IntervalStatistics, allowing for
     * statistical analysis over time intervals.
     */
    using AggregatedStatistics = std::vector<IntervalStatistics>;

}

#endif //TYPES_H
