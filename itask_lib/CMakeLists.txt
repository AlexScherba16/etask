project(itask_lib)

add_library(itask_lib STATIC
        utils/types/types.h
        cli/cli_parser.h
        cli/cli_parser.cpp
        utils/filesystem/filesystem.h
        preprocessor/preprocessor.h
        preprocessor/preprocessor.cpp
        utils/misc/misc.h
        mapper/mapper.cpp
        mapper/mapper.h
        reducer/reducer.cpp
        reducer/reducer.h
        statistics/staticstics.cpp
        statistics/staticstics.h
        statistics/metrics.cpp
        statistics/metrics.h
        aggregator/aggregator.cpp
        aggregator/aggregator.h
)

target_include_directories(itask_lib PRIVATE ${CMAKE_SOURCE_DIR}/itask_lib)
target_link_libraries(itask_lib
        cxxopts::cxxopts
        nlohmann_json::nlohmann_json
        concurrentqueue::concurrentqueue
        asio::asio)
