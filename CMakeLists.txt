cmake_minimum_required(VERSION 3.10)
project(spdlog_example)

include_directories(include)

set(CMAKE_CXX_STANDARD 17)

add_executable(spdlog_example ./src/main.cpp ./src/benchMark.cpp)