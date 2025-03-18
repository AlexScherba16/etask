#include "aggregator.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <chrono>

std::string nsToHMS(uint64_t ns)
{
    time_t seconds = ns / 1'000'000'000;

    std::tm timeinfo{};
    localtime_r(&seconds, &timeinfo);

    // Форматируем в строку "HH:MM"
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << timeinfo.tm_hour << ":"
        << std::setw(2) << std::setfill('0') << timeinfo.tm_min;

    return oss.str();
}

namespace itask:: aggregator
{
    using namespace nlohmann;
    using namespace std::chrono;

    void Aggregator::printJson(const AggregatedStatistics& stat)
    {
        for (const auto& stats : stat)
        {
            std::string interval{intervalTo_H_M_S_Format(stats.timeInterval)};
            auto j = ordered_json{
                {"interval", interval},
                {"maxVal", {{"ask", stats.askMax}, {"bid", stats.bidMax}}},
                {"minVal", {{"ask", stats.askMin}, {"bid", stats.bidMin}}},
                {"average", {{"ask", stats.askAverage}, {"bid", stats.bidAverage}}},
                {"median", {{"ask", stats.askMedian}, {"bid", stats.bidMedian}}},
                {"volume", {{"ask", stats.askVolume}, {"bid", stats.bidVolume}}}
            };
            std::cout << j.dump() << std::endl;
        }
    }

    std::string Aggregator::intervalTo_H_M_S_Format(const TimeInterval& interval)
    {
        time_t startSeconds = interval.startTimestampNs / 1'000'000'000;
        time_t endSeconds = interval.endTimestampNs / 1'000'000'000;

        std::tm startInfo{};
        std::tm endInfo{};

        localtime_r(&startSeconds, &startInfo);
        localtime_r(&endSeconds, &endInfo);

        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << startInfo.tm_hour << ":" << std::setw(2) << std::setfill('0') <<
            startInfo.tm_min << ":" << std::setw(2) << std::setfill('0') << startInfo.tm_sec
            << " - "
            << std::setw(2) << std::setfill('0') << endInfo.tm_hour << ":" << std::setw(2) << std::setfill('0') <<
            endInfo.tm_min << ":" << std::setw(2) << std::setfill('0') << endInfo.tm_sec;

        auto result{std::move(ss.str())};

        return ss.str();
    }
}
