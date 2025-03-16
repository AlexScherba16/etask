cmake_minimum_required(VERSION 3.30)
project(itask LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cxxopts REQUIRED)
find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)

add_subdirectory(itask_lib)
add_subdirectory(test)

add_executable(itask app/main.cpp)

target_include_directories(itask PRIVATE ${CMAKE_SOURCE_DIR}/itask_lib)
target_link_libraries(itask PRIVATE itask_lib)
