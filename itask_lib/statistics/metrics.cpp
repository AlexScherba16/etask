#include "metrics.h"

// min/max heap median value calculations based on
// https://github.com/vitkarpov/coding-interviews-blog-archive/blob/main/posts/find-median-from-data-stream.md

namespace itask::statistics
{
    StatMetrics::StatMetrics(StatMetrics&& other) noexcept :
        maxHeap_(std::move(other.maxHeap_)),
        minHeap_(std::move(other.minHeap_)),
        globalMinVal_(other.globalMinVal_), globalMaxVal_(other.globalMaxVal_),
        valCounter_(other.valCounter_), valSum_(other.valSum_)
    {
    }

    StatMetrics& StatMetrics::operator=(StatMetrics&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }
        maxHeap_ = std::move(other.maxHeap_);
        minHeap_ = std::move(other.minHeap_);
        globalMinVal_ = other.globalMinVal_;
        globalMaxVal_ = other.globalMaxVal_;
        valCounter_ = other.valCounter_;
        valSum_ = other.valSum_;
        return *this;
    }

    void StatMetrics::addNum(double num)
    {
        globalMinVal_ = std::min(globalMinVal_, num);
        globalMaxVal_ = std::max(globalMaxVal_, num);

        valCounter_++;
        valSum_ += num;

        maxHeap_.push(num);
        minHeap_.push(maxHeap_.top());
        maxHeap_.pop();
        if (minHeap_.size() > maxHeap_.size())
        {
            maxHeap_.push(minHeap_.top());
            minHeap_.pop();
        }
    }

    double StatMetrics::getMin() const
    {
        return globalMinVal_;
    }

    double StatMetrics::getMax() const
    {
        return globalMaxVal_;
    }

    double StatMetrics::getMedian() const
    {
        if (minHeap_.size() == maxHeap_.size())
        {
            return (minHeap_.top() + maxHeap_.top()) / 2.0;
        }
        return maxHeap_.top();
    }

    double StatMetrics::getAverage() const
    {
        if (valCounter_ == 0)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return valSum_ / valCounter_;
    }

    void StatMetrics::clear()
    {
        maxHeap_ = std::priority_queue<double>();
        minHeap_ = std::priority_queue<double, std::vector<double>, std::greater<double>>();
        globalMinVal_ = std::numeric_limits<double>::max();
        globalMaxVal_ = std::numeric_limits<double>::lowest();
        valCounter_ = 0;
        valSum_ = 0;
    }
}
