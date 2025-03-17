project(test)

enable_testing()

add_executable(test test_main.cpp
        itask_lib_test/cli_test/cli_test.cpp
        itask_lib_test/preprocessor_test/preprocessor_test.cpp
        itask_lib_test/mapper_test/mapper_test.cpp
)

target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/itask_lib)
target_compile_options(test PRIVATE -fexperimental-library)
target_link_libraries(test PRIVATE itask_lib GTest::gtest GTest::gtest_main)
