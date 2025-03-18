#include "preprocessor.h"
#include "utils/misc/misc.h"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <sstream>
#include <fstream>

namespace itask::preprocessor
{
    using namespace itask::utils::types;
    using namespace itask::utils::misc;
    using namespace nlohmann;

    Preprocessor::Preprocessor(std::string filePath, const uint16_t threadCount, const uint64_t intervalRangeNanoSec) :
        filePath_(std::move(filePath)), threadCount_(threadCount), intervalLengthNs_(intervalRangeNanoSec)
    {
        if (filePath_.empty())
        {
            throw std::invalid_argument("Empty path for preprocessing");
        }

        if (threadCount_ == 0)
        {
            throw std::invalid_argument("Thread count must be positive");
        }

        if (intervalLengthNs_ == 0)
        {
            throw std::invalid_argument("Interval range must be positive");
        }

        fileSize_ = std::filesystem::file_size(filePath_);
        if (fileSize_ == 0)
        {
            throw std::invalid_argument("File size must be positive");
        }
    }

    PreprocessedData Preprocessor::getPreprocessedData()
    {
        std::ifstream preprocFile(filePath_);
        if (!preprocFile.is_open())
        {
            throw std::runtime_error("Could not open preprocessing file: " + filePath_);
        }
        Defer deferClose(std::move([&preprocFile]() { preprocFile.close(); }));

        auto timeIntervals{getTimeIntervalSet_(preprocFile)};
        auto fileSegments{getFileSegments_(preprocFile)};
        return PreprocessedData{std::move(fileSegments), std::move(timeIntervals)};
    }

    TimeIntervalSet Preprocessor::getTimeIntervalSet_(std::ifstream& file) const
    {
        file.seekg(0, std::ios::beg);

        // first and last lines to get later first and last timestamp.
        std::string firstLine, lastLine;

        // process first line
        std::getline(file, firstLine);

        // process second line, first of all set read pointer to the before EOF position.
        file.seekg(-2, std::ios::end); // (-1 before EOF)
        char c;

        // move to file beginning char by char
        while (file.tellg() > 0)
        {
            // set previous position
            file.seekg(-2, std::ios::cur);
            file.get(c);

            // if '\n' was found, that means we are ready to read last line.
            if (c == '\n')
                break;
        }

        // store last line
        std::getline(file, lastLine);

        // precess timestamps
        uint64_t firstTimestamp{0};
        uint64_t lastTimestamp{0};
        {
            json j = json::parse(firstLine);
            firstTimestamp = static_cast<uint64_t>(stoll(j["time"]["$numberLong"].get<std::string>()));
            j = json::parse(lastLine);
            lastTimestamp = static_cast<uint64_t>(stoll(j["time"]["$numberLong"].get<std::string>()));
        }

        // timestamps was stored, ready to parse intervals.
        // calculate value of intervals
        uint64_t totalDuration{lastTimestamp - firstTimestamp};
        uint64_t intervalsValue{totalDuration / intervalLengthNs_};

        // check remaining fraction of interval.
        if (totalDuration % intervalLengthNs_)
        {
            // we add one more interval to ensure all data is covered.
            intervalsValue++;
        }

        // almost done, let's collect intervals collection
        std::vector<TimeInterval> intervals;
        intervals.reserve(intervalsValue);
        for (int i = 0; i < intervalsValue; ++i)
        {
            uint64_t startPoint{firstTimestamp + i * intervalLengthNs_};
            uint64_t endPoint{startPoint + intervalLengthNs_};
            TimeInterval tmp{startPoint, endPoint};
            intervals.emplace_back(std::move(tmp));
        }

        // collect metadata
        TimeIntervalMetadata metadata;
        metadata.intervalsValue = intervalsValue;
        metadata.globalStartTimestampNs = firstTimestamp;
        metadata.globalEndTimestampNs = lastTimestamp;
        metadata.intervalLengthNs = intervalLengthNs_;

        return {std::move(intervals), std::move(metadata)};
    }

    std::vector<FileSegment> Preprocessor::getFileSegments_(std::ifstream& file) const
    {
        size_t chunkSize = fileSize_ / threadCount_;
        if (chunkSize == 0)
        {
            std::stringstream ss;
            ss << "Chunk size must be positive, file size : " << fileSize_ << " threads : " << threadCount_
                << ", please reduce threads value";
            throw std::runtime_error(std::move(ss.str()));
        }

        std::vector<FileSegment> fileSegments;
        fileSegments.reserve(threadCount_);

        for (int i = 0; i < threadCount_; ++i)
        {
            size_t startOffset = i * chunkSize;
            size_t endOffset = (i == threadCount_ - 1) ? fileSize_ : (startOffset + chunkSize);

            // move startOffset forward until reach '\n'
            if (startOffset > 0)
            {
                file.seekg(startOffset, std::ios::beg);
                char c;
                while (file.get(c) && c != '\n' && file.tellg() < fileSize_);
                startOffset = file.tellg();
            }

            // move endOffset forward until reach '\n'
            if (endOffset < fileSize_)
            {
                file.seekg(endOffset, std::ios::beg);
                char c;
                while (file.get(c) && c != '\n' && file.tellg() < fileSize_);
                endOffset = file.tellg();
            }

            FileSegment tmp{startOffset, endOffset};
            fileSegments.emplace_back(std::move(tmp));
        }
        return fileSegments;
    }
}
