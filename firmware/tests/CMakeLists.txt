cmake_minimum_required(VERSION 3.22)

project(tests)

find_package(Catch2 2 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/solution)
add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain solutionLib)