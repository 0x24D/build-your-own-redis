#include "../../src/rediscommands.h"
#include "../utils/testhelper.h"

void testCommand() {
    // All command details
    TestHelper::startsWith(
        "command", RedisCommands::getResponse({"command"}), std::string_view{"*3\r\n*7\r\n"});

    TestHelper::startsWith(
        "COMMAND", RedisCommands::getResponse({"COMMAND"}), std::string_view{"*3\r\n*7\r\n"});

    // Number of commands
    TestHelper::equals("command count", RedisCommands::getResponse({"command", "count"}),
        std::string_view{":3\r\n"});

    TestHelper::equals("COMMAND COUNT", RedisCommands::getResponse({"COMMAND", "COUNT"}),
        std::string_view{":3\r\n"});

    // Specific command details
    TestHelper::startsWith("command info ping",
        RedisCommands::getResponse({"command", "info", "ping"}), std::string_view{"*1\r\n*7\r\n"});

    TestHelper::startsWith("COMMAND INFO PING",
        RedisCommands::getResponse({"COMMAND", "INFO", "PING"}), std::string_view{"*1\r\n*7\r\n"});

    TestHelper::startsWith("command info echo ping",
        RedisCommands::getResponse({"command", "info", "echo", "ping"}),
        std::string_view{"*2\r\n*7\r\n"});

    TestHelper::startsWith("COMMAND INFO ECHO PING",
        RedisCommands::getResponse({"COMMAND", "INFO", "ECHO", "PING"}),
        std::string_view{"*2\r\n*7\r\n"});

    // No command
    TestHelper::equals("command info", RedisCommands::getResponse({"command", "info"}),
        std::string_view{"*0\r\n"});

    TestHelper::equals("COMMAND INFO", RedisCommands::getResponse({"COMMAND", "INFO"}),
        std::string_view{"*0\r\n"});

    // Non-existing command
    TestHelper::equals("command info foo", RedisCommands::getResponse({"command", "info", "foo"}),
        std::string_view{"*1\r\n*0\r\n"});

    TestHelper::equals("COMMAND INFO FOO", RedisCommands::getResponse({"COMMAND", "INFO", "FOO"}),
        std::string_view{"*1\r\n*0\r\n"});
}

void testEcho() {
    // TODO: Error handling
    // TestHelper::equals("echo", RedisCommands::getResponse({"echo"}), std::string_view{"-ERR wrong
    // number of arguments for 'echo' command\r\n"});

    // TODO: Error handling
    // TestHelper::equals("ECHO", RedisCommands::getResponse({"ECHO"}), std::string_view{"-ERR wrong
    // number of arguments for 'echo' command\r\n"});

    TestHelper::equals("echo Hello", RedisCommands::getResponse({"echo", "Hello"}),
        std::string_view{"$5\r\nHello\r\n"});

    // TODO: Error handling
    // TestHelper::equals("echo Hello World", RedisCommands::getResponse({"echo", "Hello",
    // "World"}), std::string_view{"-ERR wrong number of arguments for 'echo' command\r\n"});

    TestHelper::equals("echo Hello\\r\\nWorld",
        RedisCommands::getResponse({"echo", "Hello\\r\\nWorld"}),
        std::string_view{"$14\r\nHello\\r\\nWorld\r\n"});

    TestHelper::equals("echo \"Hello World\"", RedisCommands::getResponse({"echo", "Hello World"}),
        std::string_view{"$11\r\nHello World\r\n"});

    TestHelper::equals(R"(echo "Hello\r\nWorld")",
        RedisCommands::getResponse({"echo", "Hello\r\nWorld"}),
        std::string_view{"$12\r\nHello\r\nWorld\r\n"});
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

    TestHelper::equals(R"(ping "Hello\r\nWorld")",
        RedisCommands::getResponse({"ping", "Hello\r\nWorld"}),
        std::string_view{"$12\r\nHello\r\nWorld\r\n"});
}

int main() {
    testCommand();
    testEcho();
    testPing();

    return TestHelper::result("unit/rediscommands");
}
