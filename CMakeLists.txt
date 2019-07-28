project(microloop)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Debug)

include(CheckIncludeFiles)
check_include_files("sys/epoll.h" HAVE_EPOLL)
check_include_files("sys/event.h;sys/types.h" HAVE_KQUEUE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sys_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/sys_config.h)

add_library(microloop SHARED
  src/event_loop.cpp
  src/signals_monitor.cpp
  src/utils/error.cpp
  src/utils/thread_pool.cpp
  src/net/tcp_server.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(microloop pthread)

add_executable(microloop-usage
        example/usage.cpp)
target_link_libraries(microloop-usage microloop)
