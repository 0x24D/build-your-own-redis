project(build-your-own-redis)

add_executable(test-unit-rediscommands rediscommands.cpp ${CMAKE_BINARY_DIR}/src/rediscommands.cpp)
add_executable(test-unit-respparser respparser.cpp)
