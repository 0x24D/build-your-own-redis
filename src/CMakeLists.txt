project(build-your-own-redis)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS system) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(redis-server main.cpp 
                            redisclient.cpp
                            rediscommands.cpp 
                            redisserver.cpp)
target_link_libraries(redis-server ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
