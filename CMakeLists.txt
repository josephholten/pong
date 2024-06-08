cmake_minimum_required(VERSION 3.24.0)
project(pong VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(raylib)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games

add_executable(pong pong.cpp)
target_include_directories(pong PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(pong PRIVATE raylib)