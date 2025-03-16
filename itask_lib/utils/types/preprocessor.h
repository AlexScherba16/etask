#ifndef PREPROCESSED_DATA_H
#define PREPROCESSED_DATA_H

#include <vector>

namespace itask::utils::types
{
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
        size_t startOffset;
        size_t endOffset;
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
}

#endif //PREPROCESSED_DATA_H
