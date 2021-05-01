project(build-your-own-redis)

#add_executable(test-unit-rediscommands unit/rediscommands.cpp ../src/rediscommands.cpp)
#add_executable(test-unit-respparser unit/respparser.cpp)
add_subdirectory(integration)
add_subdirectory(unit)
