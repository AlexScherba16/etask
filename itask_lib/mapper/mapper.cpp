#include "mapper.h"
#include "utils/misc/misc.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using namespace itask::utils::types;
using namespace itask::utils::misc;
using namespace nlohmann;

namespace itask::mapper
{
    Mapper::Mapper(std::string filePath, FileSegment segment, const TimeIntervalSet& timeSet,
                   QuoteChannelsMap& quotesChannelsMap,
                   std::latch& latch) :
        filePath_(std::move(filePath)), segment_(segment), quotesChannelsMapRef_(quotesChannelsMap), latchRef_(latch),
        metadata_(timeSet.timeIntervalMetadata)
    {
        if (filePath_.empty())
        {
            throw std::invalid_argument("Empty path to mapping file");
        }

        auto fileSize{std::filesystem::file_size(filePath_)};
        if (fileSize == 0)
        {
            throw std::invalid_argument("File size must be positive");
        }

        if (segment_.endOffset < segment_.startOffset)
        {
            throw std::invalid_argument("Segment end offset is less than start offset");
        }

        if (quotesChannelsMapRef_.get().empty())
        {
            throw std::invalid_argument("Mapping channels are empty");
        }

        if (metadata_.intervalLengthNs == 0)
        {
            throw std::invalid_argument("Mapper interval length must be positive");
        }
    }

    Mapper::Mapper(Mapper&& other) noexcept :
        filePath_(std::move(other.filePath_)), segment_(std::move(other.segment_)),
        quotesChannelsMapRef_(other.quotesChannelsMapRef_), latchRef_(other.latchRef_),
        metadata_(other.metadata_)
    {
    }

    Mapper& Mapper::operator=(Mapper&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }
        filePath_ = std::move(other.filePath_);
        segment_ = std::move(other.segment_);
        quotesChannelsMapRef_ = other.quotesChannelsMapRef_;
        latchRef_ = other.latchRef_;
        metadata_ = std::move(other.metadata_);
        return *this;
    }

    void Mapper::operator()()
    {
        // decrement latch on exit scope
        Defer done{[this]() { latchRef_.get().count_down(); }};

        std::ifstream mappingFile{};
        mappingFile.open(filePath_);
        if (!mappingFile.is_open())
        {
            std::cerr << "Could not open mapping file : " + filePath_ << std::endl;
            return;
        }

        uint64_t channelIndex{0};
        uint64_t maxChannelsIndex{quotesChannelsMapRef_.get().size() - 1};

        std::string line;

        // go to start point of segment
        mappingFile.seekg(segment_.startOffset, std::ios::beg);
        while (std::getline(mappingFile, line) && mappingFile.tellg() <= static_cast<std::streampos>(segment_.
            endOffset))
        {
            try
            {
                auto json = nlohmann::json::parse(line);

                // minor processing, refer to Quote struct description
                Quote quote{
                    static_cast<uint64_t>(stoll(json["time"]["$numberLong"].get<std::string>())),
                    static_cast<double>(std::stoll(json["bid"]["$numberInt"].get<std::string>())) / 1'000'000.0,
                    static_cast<double>(std::stoll(json["ask"]["$numberInt"].get<std::string>())) / 1'000'000.0,
                    static_cast<double>(std::stoll(json["bidVolume"]["$numberInt"].get<std::string>())) / 1'000.0,
                    static_cast<double>(std::stoll(json["askVolume"]["$numberInt"].get<std::string>())) / 1'000.0,
                };

                // identify proper channel for data transfer
                channelIndex = (quote.timeNs - metadata_.globalStartTimestampNs) / metadata_.intervalLengthNs;
                if (channelIndex > maxChannelsIndex)
                {
                    std::cerr << "Invalid channel index : " << channelIndex << " timestamp : " << quote.timeNs <<
                        " interval range : " << metadata_.intervalLengthNs << std::endl;
                    continue;
                }

                // send Quote struct
                quotesChannelsMapRef_.get()[channelIndex].enqueue(std::move(quote));
            }
            catch (const std::exception& e)
            {
                std::cerr << "Handle exception during mapping : " << e.what() << std::endl;
            }
        }
    }
}
