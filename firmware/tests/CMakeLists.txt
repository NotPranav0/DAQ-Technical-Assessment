cmake_minimum_required(VERSION 3.22)

project(tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 2 REQUIRED)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)