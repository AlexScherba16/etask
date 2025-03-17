#ifndef TYPES_H
#define TYPES_H

#include <queue>
#include <vector>

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
        uint64_t startTimestamp;
        uint64_t endTimestamp;
    };

    /**
     *@struct PreprocessedData
     * @brief Stores preprocessed data for file segmentation and time-based intervals.
     *
     * This structure holds the result of preprocessing, including file partitions
     * and predefined time intervals for further processing.
     */
    struct PreprocessedData
    {
        std::vector<FileSegment> fileSegments;
        std::vector<TimeInterval> timeIntervals;
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

        bool operator==(const Quote& o) const
        {
            return timeNs == o.timeNs &&
                bid == o.bid &&
                ask == o.ask &&
                bidVolume == o.bidVolume &&
                askVolume == o.askVolume;
        }
    };

}

#endif //TYPES_H
