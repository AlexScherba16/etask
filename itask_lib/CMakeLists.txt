project(itask_lib)

add_library(itask_lib STATIC
        cli/cli_parser.h
        cli/cli_parser.cpp
)

target_include_directories(itask_lib PRIVATE ${CMAKE_SOURCE_DIR}/itask_lib)
target_link_libraries(itask_lib cxxopts::cxxopts)
