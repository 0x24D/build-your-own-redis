project(build-your-own-redis)

add_executable(test-rediscommands rediscommands.cpp ../src/rediscommands.cpp)
add_executable(test-respparser respparser.cpp)
add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic)
