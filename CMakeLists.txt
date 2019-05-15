project(microloop)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(microloop SHARED
    src/event_loop.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
