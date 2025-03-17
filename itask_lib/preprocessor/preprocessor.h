#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "utils/types/types.h"

#include<string>

using namespace itask::utils::types;

namespace itask::preprocessor
{
    /**
     * @class Preprocessor
     * @brief Prepares file partitions and time intervals for processing.
     *
     * The Preprocessor class is responsible for reading an input file,
     * partitioning it into segments for further multithreaded processing, and
     * defining time intervals for data unique classification.
     */
    class Preprocessor
    {
    public:
        Preprocessor() = delete;
        Preprocessor(const Preprocessor&) = delete;
        Preprocessor(Preprocessor&&) = delete;
        Preprocessor& operator=(const Preprocessor&) = delete;
        Preprocessor& operator=(Preprocessor&&) = delete;

        ~Preprocessor() = default;

        /**
         * @brief Constructs a Preprocessor instance.
         *
         * This class is responsible for preparing partitions for further parsing.
         * It splits the input file into multiple partitions for the specified number of threads.
         * And defines time intervals range to map records by specific interval id.
         *
         * @param filePath Path to the input file.
         * @param threadCount Number of threads for processing.
         *        The file will be evenly partitioned, with each thread assigned its own parsing chunk.
         * @param intervalRangeNanoSec Interval range in nanoseconds.
         *        This defines the time range for which records will be grouped and processed together,
         *        Each record will belong to a specific interval based on its timestamp.
         *
         * @throws If the provided file path is empty, file is invalid, thread count or intervalRange is zero.
         */
        Preprocessor(std::string filePath, const uint16_t threadCount, const uint64_t intervalRangeNanoSec);

        /**
         * @brief Retrieves preprocessed data from a file.
         *
         * Reads a file and parses its contents to generate preprocessed data,
         * including file segments and time intervals.
         *
         * @return Preprocessed data containing file segments and time intervals.
         *
         * @throws If the file cannot be opened, read, failed to parse data.
         */
        PreprocessedData getPreprocessedData();

    private:
        std::string filePath_{};
        uint16_t threadCount_{0};
        uint64_t intervalRangeNs_{0};
        uint64_t fileSize_{0};

        /**
         * @brief Parses time intervals from the given file stream.
         *
         * This method reads and extracts first and last timestamp from the provided input file stream.
         * It processes the file content to generate a collection of time intervals,
         * total duration and requred interval range.
         *
         * @param file Input file stream to read time intervals from.
         * @return Parsed intervals collection with total duration and requred interval range.
         *
         * @throws If the file cannot be read or the parsing fails.
         */
        TimeIntervalSet getTimeIntervalSet_(std::ifstream& file) const;

        /**
         * @brief Parses file segments from the given file stream.
         *
         * This method reads the input file and determines segments for partitioning.
         *
         * @param file Input file stream used to read and extract file segments.
         * @return A vector of parsed file segments.
         *
         * @throws  If the file cannot be read or the parsing fails.
         */
        std::vector<FileSegment> getFileSegments_(std::ifstream& file) const;
    };
}

#endif //PREPROCESSOR_H
