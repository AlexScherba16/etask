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
     * @struct Interval
     * @brief Represents a time interval.
     *
     * A time interval defines a range of timestamps that are used
     * to categorize records into specific time-based partitions.
     */
    struct Interval
    {
        uint64_t startTimestampNs;
        uint64_t endTimestampNs;
    };

    /**
     * @struct TimeIntervalSet
     * @brief Represents a collection of time intervals with total duration and interval range in nanoseconds.
     *
     * This structure stores multiple time intervals and calculates
     * the total duration of all intervals combined.
     */
    struct TimeIntervalSet
    {
        std::vector<Interval> timeIntervals;
        uint64_t totalDurationNs{0};
        uint64_t intervalRangeNs{0};
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

}

#endif //TYPES_H
