#include "../../src/rediscommands.h"
#include "../utils/testhelper.h"

void testCommand() {
    TestHelper::startsWith(
        "command", RedisCommands::getResponse({"command"}), std::string_view{"*2\r\n*7\r\n"});

    TestHelper::startsWith(
        "COMMAND", RedisCommands::getResponse({"COMMAND"}), std::string_view{"*2\r\n*7\r\n"});
}

void testPing() {
    TestHelper::equals("ping", RedisCommands::getResponse({"ping"}), std::string_view{"+PONG\r\n"});

    TestHelper::equals("PING", RedisCommands::getResponse({"PING"}), std::string_view{"+PONG\r\n"});

    TestHelper::equals("ping Hello", RedisCommands::getResponse({"ping", "Hello"}),
        std::string_view{"$5\r\nHello\r\n"});

    // TODO: Error handling
    // TestHelper::equals("ping Hello World", RedisCommands::getResponse({"ping", "Hello",
    // "World"}), std::string_view{"-ERR wrong number of arguments for 'ping' command\r\n"});

    TestHelper::equals("ping Hello\\r\\nWorld",
        RedisCommands::getResponse({"ping", "Hello\\r\\nWorld"}),
        std::string_view{"$14\r\nHello\\r\\nWorld\r\n"});

    TestHelper::equals("ping \"Hello World\"", RedisCommands::getResponse({"ping", "Hello World"}),
        std::string_view{"$11\r\nHello World\r\n"});

    TestHelper::equals("ping \"Hello\\r\\nWorld\"",
        RedisCommands::getResponse({"ping", "Hello\r\nWorld"}),
        std::string_view{"$12\r\nHello\r\nWorld\r\n"});
}

int main() {
    testCommand();
    testPing();

    return TestHelper::result("unit/rediscommands");
}
