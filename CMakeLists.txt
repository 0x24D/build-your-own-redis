cmake_minimum_required(VERSION 3.20)

project(build-your-own-redis)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(EXECUTABLE_OUTPUT_PATH "bin")
file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS system) 

if(Boost_FOUND)
    include_directories("${Boost_INCLUDE_DIRS}") 
    link_directories("${Boost_LIBRARY_DIRS}")
endif(Boost_FOUND)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(redis-server src/main.cpp)
target_link_libraries(redis-server "${Boost_SYSTEM_LIBRARY}" "${CMAKE_THREAD_LIBS_INIT}")

