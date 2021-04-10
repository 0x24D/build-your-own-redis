cmake_minimum_required(VERSION 3.20)

project(build-your-own-redis)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(EXECUTABLE_OUTPUT_PATH "bin")

file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

add_executable(redis-server src/main.cpp)

