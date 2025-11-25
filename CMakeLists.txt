cmake_minimum_required(VERSION 3.30)

project("vvlib")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)
CPMAddPackage("gh:fmtlib/fmt#11.2.0")
CPMAddPackage("gh:doctest/doctest#v2.4.12")
CPMAddPackage("gh:martinus/nanobench#v4.3.11")

add_executable(result_tests tests/result.cpp)
target_include_directories(result_tests PRIVATE include)
target_link_libraries(result_tests fmt::fmt doctest::doctest nanobench)
set_target_properties(result_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/dbg"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/rel"
)

add_executable(option_tests tests/option.cpp)
target_include_directories(option_tests PRIVATE include)
target_link_libraries(option_tests fmt::fmt doctest::doctest nanobench)
set_target_properties(option_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/dbg"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/rel"
)
