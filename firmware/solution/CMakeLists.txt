cmake_minimum_required(VERSION 3.22)

project(solution)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Manually specify dbcppp's include and library directories
include_directories(/usr/local/include/dbcppp)
link_directories(/usr/local/lib)

# Locate the library file
find_library(DBCPPP_LIB NAMES dbcppp PATHS /usr/local/lib)

if (DBCPPP_LIB)
    message(STATUS "Found dbcppp: ${DBCPPP_LIB}")
else()
    message(FATAL_ERROR "dbcppp library not found!")
endif()

# Create the executable and link dbcppp
add_executable(answer main.cpp)
target_link_libraries(answer PRIVATE ${DBCPPP_LIB})


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
endif()


