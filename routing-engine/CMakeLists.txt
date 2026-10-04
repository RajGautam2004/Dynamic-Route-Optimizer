cmake_minimum_required(VERSION 3.14)
project(DynamicRouteOptimizer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(include)

# Find threads
find_package(Threads REQUIRED)

# Source files
set(SOURCES
    src/Graph.cpp
    src/DijkstraRouter.cpp
    src/AStarRouter.cpp
    src/MultiSourceRouter.cpp
    src/AlternativeRouter.cpp
    src/RouteManager.cpp
    src/HttpServer.cpp
)

# Main Executable
add_executable(routing_engine ${SOURCES} src/main.cpp)
target_link_libraries(routing_engine PRIVATE Threads::Threads)

# Test Executable
add_executable(routing_tests ${SOURCES} tests/test_main.cpp)
target_link_libraries(routing_tests PRIVATE Threads::Threads)

# Benchmark Executable
add_executable(routing_benchmarks ${SOURCES} benchmarks/benchmark_main.cpp)
target_link_libraries(routing_benchmarks PRIVATE Threads::Threads)

if (WIN32)
    target_link_libraries(routing_engine PRIVATE ws2_32)
    target_link_libraries(routing_tests PRIVATE ws2_32)
    target_link_libraries(routing_benchmarks PRIVATE ws2_32)
endif()
