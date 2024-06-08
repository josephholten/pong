cmake_minimum_required(VERSION 3.24.0)
project(pong VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(raylib)
add_subdirectory(util)
include_directories(util)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games

set(WARNINGS -Wall -Wextra -Wpedantic -Wno-missing-field-initializers)

add_executable(pong pong.cpp)
target_link_libraries(pong PRIVATE util raylib)
target_compile_options(pong PRIVATE ${WARNINGS})

add_executable(breakout breakout.cpp)
target_link_libraries(breakout PRIVATE util raylib)
target_compile_options(breakout PRIVATE ${WARNINGS})