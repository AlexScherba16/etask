#include "reducer.h"
#include "utils/misc/misc.h"

#include <latch>

namespace itask::reducer
{
    using namespace itask::utils::misc;

    Reducer::Reducer(uint64_t id, TimeInterval timeInterval,
                     QuoteChannelsMap& qChanMap, AggregatedStatistics& aggrStat,
                     std::latch& latch) :
        reducerId_(id), quotesChannelsMapRef_(qChanMap),
        aggregatedStatisticsRef_(aggrStat), latchRef_(latch),
        statistics_(std::move(timeInterval))
    {
        if (quotesChannelsMapRef_.get().empty())
        {
            throw std::invalid_argument("Reducing channels are empty");
        }

        if (aggregatedStatisticsRef_.get().empty())
        {
            throw std::invalid_argument("Aggregated statistics is empty");
        }

        if (reducerId_ > quotesChannelsMapRef_.get().size() - 1)
        {
            throw std::invalid_argument("Reducer ID is out of channels range");
        }

        if (reducerId_ > aggregatedStatisticsRef_.get().size() - 1)
        {
            throw std::invalid_argument("Reducer ID is out of aggregated statistics range");
        }
    }

    Reducer::Reducer(Reducer&& other) noexcept :
        reducerId_(other.reducerId_), quotesChannelsMapRef_(other.quotesChannelsMapRef_),
        aggregatedStatisticsRef_(other.aggregatedStatisticsRef_), latchRef_(other.latchRef_),
        statistics_(std::move(other.statistics_))
    {
    }

    Reducer& Reducer::operator=(Reducer&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }
        reducerId_ = other.reducerId_;
        quotesChannelsMapRef_ = other.quotesChannelsMapRef_;
        aggregatedStatisticsRef_ = other.aggregatedStatisticsRef_;
        latchRef_ = other.latchRef_;
        statistics_ = std::move(other.statistics_);
        return *this;
    }

    void Reducer::operator()()
    {
        // decrement latch on exit scope
        Defer done{[this]() { latchRef_.get().count_down(); }};

        while (true)
        {
            std::optional<Quote> tmp;
            if (quotesChannelsMapRef_.get()[reducerId_].try_dequeue(tmp))
            {
                if (!tmp.has_value())
                {
                    // end of streaming condition
                    break;
                }
                statistics_.addQuote(std::move(tmp.value()));
            }
        }
        // collect computed statistics and store in aggregated collection
        auto reducedStatistics{statistics_.getStatistics()};
        aggregatedStatisticsRef_.get()[reducerId_] = std::move(reducedStatistics);
    }
}
