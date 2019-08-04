project(microloop)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

add_library(microloop SHARED
  src/event_loop.cpp
  src/signals_monitor.cpp
  src/utils/error.cpp
  src/utils/thread_pool.cpp
  src/net/tcp_server.cpp)

target_include_directories(microloop PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/microloop)
target_link_libraries(microloop pthread)

option(BUILD_EXAMPLE "Build the usage example" OFF)
if (BUILD_EXAMPLE)
  add_executable(microloop-usage example/usage.cpp)
  target_link_libraries(microloop-usage microloop)
  target_include_directories(microloop PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/microloop)
endif()

