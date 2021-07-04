#include "../../src/rediscommands.h"
#include "../utils/testhelper.h"

void test_command() {
    // All command details
    TestHelper::starts_with(
        "command", RedisCommands::get_response({"command"}), std::string_view{"*3\r\n*7\r\n"});

    TestHelper::starts_with(
        "COMMAND", RedisCommands::get_response({"COMMAND"}), std::string_view{"*3\r\n*7\r\n"});

    // Number of commands
    TestHelper::equals("command count", RedisCommands::get_response({"command", "count"}),
        std::string_view{":3\r\n"});

    TestHelper::equals("COMMAND COUNT", RedisCommands::get_response({"COMMAND", "COUNT"}),
        std::string_view{":3\r\n"});

    // Specific command details
    TestHelper::starts_with("command info ping",
        RedisCommands::get_response({"command", "info", "ping"}), std::string_view{"*1\r\n*7\r\n"});

    TestHelper::starts_with("COMMAND INFO PING",
        RedisCommands::get_response({"COMMAND", "INFO", "PING"}), std::string_view{"*1\r\n*7\r\n"});

    TestHelper::starts_with("command info echo ping",
        RedisCommands::get_response({"command", "info", "echo", "ping"}),
        std::string_view{"*2\r\n*7\r\n"});

    TestHelper::starts_with("COMMAND INFO ECHO PING",
        RedisCommands::get_response({"COMMAND", "INFO", "ECHO", "PING"}),
        std::string_view{"*2\r\n*7\r\n"});

    // No command
    TestHelper::equals("command info", RedisCommands::get_response({"command", "info"}),
        std::string_view{"*0\r\n"});

    TestHelper::equals("COMMAND INFO", RedisCommands::get_response({"COMMAND", "INFO"}),
        std::string_view{"*0\r\n"});

    // Non-existent command
    TestHelper::equals("command info foo", RedisCommands::get_response({"command", "info", "foo"}),
        std::string_view{"*1\r\n*0\r\n"});

    TestHelper::equals("COMMAND INFO FOO", RedisCommands::get_response({"COMMAND", "INFO", "FOO"}),
        std::string_view{"*1\r\n*0\r\n"});

    // Non-existent subcommand
    TestHelper::equals("command foo", RedisCommands::get_response({"command", "foo"}),
        std::string_view{"-ERR Unknown subcommand or wrong number of arguments for 'foo'\r\n"});

    TestHelper::equals("COMMAND FOO", RedisCommands::get_response({"COMMAND", "FOO"}),
        std::string_view{"-ERR Unknown subcommand or wrong number of arguments for 'FOO'\r\n"});
}

void test_echo() {
    TestHelper::equals("echo", RedisCommands::get_response({"echo"}),
        std::string_view{"-ERR wrong number of arguments for 'echo' command\r\n"});

    TestHelper::equals("ECHO", RedisCommands::get_response({"ECHO"}),
        std::string_view{"-ERR wrong number of arguments for 'echo' command\r\n"});

    TestHelper::equals("echo Hello", RedisCommands::get_response({"echo", "Hello"}),
        std::string_view{"$5\r\nHello\r\n"});

    TestHelper::equals("echo Hello World", RedisCommands::get_response({"echo", "Hello", "World"}),
        std::string_view{"-ERR wrong number of arguments for 'echo' command\r\n"});

    TestHelper::equals("ECHO Hello World", RedisCommands::get_response({"ECHO", "Hello", "World"}),
        std::string_view{"-ERR wrong number of arguments for 'echo' command\r\n"});

    TestHelper::equals("echo Hello\\r\\nWorld",
        RedisCommands::get_response({"echo", "Hello\\r\\nWorld"}),
        std::string_view{"$14\r\nHello\\r\\nWorld\r\n"});

    TestHelper::equals("echo \"Hello World\"", RedisCommands::get_response({"echo", "Hello World"}),
        std::string_view{"$11\r\nHello World\r\n"});

    TestHelper::equals(R"(echo "Hello\r\nWorld")",
        RedisCommands::get_response({"echo", "Hello\r\nWorld"}),
        std::string_view{"$12\r\nHello\r\nWorld\r\n"});
}

void test_ping() {
    TestHelper::equals(
        "ping", RedisCommands::get_response({"ping"}), std::string_view{"+PONG\r\n"});

    TestHelper::equals(
        "PING", RedisCommands::get_response({"PING"}), std::string_view{"+PONG\r\n"});

    TestHelper::equals("ping Hello", RedisCommands::get_response({"ping", "Hello"}),
        std::string_view{"$5\r\nHello\r\n"});

    TestHelper::equals("ping Hello World", RedisCommands::get_response({"ping", "Hello", "World"}),
        std::string_view{"-ERR wrong number of arguments for 'ping' command\r\n"});

    TestHelper::equals("PING Hello World", RedisCommands::get_response({"PING", "Hello", "World"}),
        std::string_view{"-ERR wrong number of arguments for 'ping' command\r\n"});

    TestHelper::equals("ping Hello\\r\\nWorld",
        RedisCommands::get_response({"ping", "Hello\\r\\nWorld"}),
        std::string_view{"$14\r\nHello\\r\\nWorld\r\n"});

    TestHelper::equals("ping \"Hello World\"", RedisCommands::get_response({"ping", "Hello World"}),
        std::string_view{"$11\r\nHello World\r\n"});

    TestHelper::equals(R"(ping "Hello\r\nWorld")",
        RedisCommands::get_response({"ping", "Hello\r\nWorld"}),
        std::string_view{"$12\r\nHello\r\nWorld\r\n"});
}

void test_unknown() {
    TestHelper::equals("unknown", RedisCommands::get_response({"unknown"}),
        std::string_view{"-ERR unknown command 'unknown'\r\n"});

    TestHelper::equals("UNKNOWN", RedisCommands::get_response({"UNKNOWN"}),
        std::string_view{"-ERR unknown command 'UNKNOWN'\r\n"});
}

int main() {
    test_command();
    test_echo();
    test_ping();
    test_unknown();

    return TestHelper::result("unit/rediscommands");
}
