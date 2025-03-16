project(itask_lib)

add_library(itask_lib STATIC
        utils/types/cli.h
        cli/cli_parser.h
        cli/cli_parser.cpp
        utils/filesystem/filesystem.h
        utils/types/preprocessor.h
        preprocessor/preprocessor.h
        preprocessor/preprocessor.cpp
        utils/misc/misc.h

)

target_include_directories(itask_lib PRIVATE ${CMAKE_SOURCE_DIR}/itask_lib)
target_link_libraries(itask_lib cxxopts::cxxopts nlohmann_json::nlohmann_json)
