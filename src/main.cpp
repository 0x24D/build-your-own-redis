#include "redisserver.h"

int main() {
    RedisServer server{};
    server.listen();

    return 0;
}
