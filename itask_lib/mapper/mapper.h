#ifndef MAPPER_H
#define MAPPER_H

#include "utils/types/types.h"

#include <fstream>
#include <latch>

using namespace itask::utils::types;

namespace itask::mapper
{
    /**
     * @class Mapper
     * @brief Parses file segment and Quotes structures to channel.
     *
     * The `Mapper` class is responsible for:
     * - Parsing a specific segment of a file.
     * - Performing minimal preprocessing (refer to 'Quote' structure for details in 'itask/itask_lib/utils/types/types.h').
     * - Sending the parsed `Quote` objects to the appropriate channel.
     *
     * This class is designed to operate as a callable (`operator()`), making it
     * suitable for execution in a separate thread.
     *
     * @note: only movable
     */
    class Mapper
    {
    public:
        Mapper(const Mapper&) = delete;
        Mapper& operator=(const Mapper&) = delete;

        /**
         * @brief Constructs a Mapper instance.
         *
         * @param filePath Path to the file containing quote data in JSON format.
         * @param segment The specific file segment assigned for parsing.
         * @param timeSet The set of time intervals used for mapping quotes.
         * @param quotesChannelsMap Reference to the collection of quote channels.
         * @param latch A synchronization latch to signal completion.
         *
         * @throws If the provided file path or channels are empty,
         * file or segment is invalid and interval range is zero.
         *
         * @note ❗❗❗IMPORTANT❗❗❗ It is SUPER CRUCIAL to ensure that the referenced objects
         * (`QuoteChannelsMap` and `std::latch`) remain valid throughout the lifetime of
         * this `Mapper` instance. Dangling references will lead to undefined behavior.
         *
         * References are used instead of `shared_ptr` to avoid unnecessary pointer dereferencing overhead.
         *
         */
        Mapper(std::string filePath, FileSegment segment, const TimeIntervalSet& timeSet,
               QuoteChannelsMap& quotesChannelsMap,
               std::latch& latch);
        /**
         * @brief Move constructor.
         *
         * Allows Mapper to be moved.
         */
        Mapper(Mapper&&) noexcept;

        /**
         * @brief Move assignment operator.
         *
         * Enables move assignment for Mapper.
         */
        Mapper& operator=(Mapper&&) noexcept;

        /**
         * @brief Executes the mapping process.
         *
         * This function performs:
         * - File parsing within the assigned segment.
         * - Minor preprocessing of the extracted quotes.
         * - Routing of processed `Quote` objects into the appropriate channels.
         */
        void operator()();

    private:
        std::string filePath_{};
        FileSegment segment_;
        std::reference_wrapper<QuoteChannelsMap> quotesChannelsMapRef_;
        std::reference_wrapper<std::latch> latchRef_;
        TimeIntervalMetadata metadata_;
    };
}

#endif //MAPPER_H
