project(microloop)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)

add_library(microloop SHARED
        include/event_loop.h
        include/event_source.h
        include/kernel_exception.h
        include/linux_thread.h
        include/event_sources/timeout.h
        src/event_loop.cpp
        src/event_sources/timeout.cpp
        src/pthread_wrapper.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(microloop pthread)

add_executable(microloop-usage
        example/usage.cpp)
target_link_libraries(microloop-usage microloop)

add_executable(pthreads-example example/pthreads.cpp src/pthread_wrapper.cpp)
target_link_libraries(pthreads-example pthread)
