#include "staticstics.h"

namespace itask::statistics
{
    Statistics::Statistics(TimeInterval timeInterval) :
        timeInterval_(std::move(timeInterval))
    {
        if (timeInterval_.startTimestampNs > timeInterval_.endTimestampNs)
        {
            std::stringstream ss;
            ss << "Statistics time interval is out of range, start : " << timeInterval_.startTimestampNs << ", end : "
                <<
                timeInterval_.endTimestampNs << std::endl;
            throw std::invalid_argument(ss.str());
        }
    }

    Statistics::Statistics(Statistics&& other) noexcept :
        timeInterval_(std::move(other.timeInterval_)), askMetrics_(std::move(other.askMetrics_)),
        bidMetrics_(std::move(other.bidMetrics_)), askVolume_(other.askVolume_), bidVolume_(other.bidVolume_)
    {
    }

    Statistics& Statistics::operator=(Statistics&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }
        timeInterval_ = std::move(other.timeInterval_);
        askMetrics_ = std::move(other.askMetrics_);
        bidMetrics_ = std::move(other.bidMetrics_);
        askVolume_ = other.askVolume_;
        bidVolume_ = other.bidVolume_;
        return *this;
    }

    void Statistics::addQuote(Quote quote)
    {
        askMetrics_.addNum(quote.ask);
        bidMetrics_.addNum(quote.bid);

        askVolume_ += quote.askVolume;
        bidVolume_ += quote.bidVolume;
    }

    IntervalStatistics Statistics::getStatistics() const
    {
        return {
            timeInterval_,
            askMetrics_.getMax(),
            askMetrics_.getMin(),
            askMetrics_.getAverage(),
            askMetrics_.getMedian(),
            askVolume_,
            bidMetrics_.getMax(),
            bidMetrics_.getMin(),
            bidMetrics_.getAverage(),
            bidMetrics_.getMedian(),
            bidVolume_
        };
    }
}
