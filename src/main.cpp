#include "redisserver.h"

int main() {
    RedisServer server{6379};
    server.listen();

    return 0;
}
