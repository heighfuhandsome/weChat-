cmake_minimum_required(VERSION 3.15)
set(CMAKE_BUILD_TYPE Debug)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DLOG_DEBUG)
endif()

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
project(xchat)
add_subdirectory(src/)

