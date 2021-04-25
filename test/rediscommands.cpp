#include "../src/rediscommands.h"
#include "testhelper.h"

int main() {
    // TODO: "command" response starts with "*numCommands*\r\n
    TestHelper::assert("ping", RedisCommands::getResponse({"ping"}), std::string{"+PONG\r\n"});
    TestHelper::assert("PING", RedisCommands::getResponse({"PING"}), std::string{"+PONG\r\n"});
    TestHelper::assert("ping Hello", RedisCommands::getResponse({"ping", "Hello"}), std::string{"$5\r\nHello\r\n"});
    // TODO: Error handling
    //TestHelper::assert("ping Hello World", RedisCommands::getResponse({"ping", "Hello", "World"}), std::string{"-ERR wrong number of arguments for 'ping' command\r\n"});
    TestHelper::assert("ping Hello\\r\\nWorld", RedisCommands::getResponse({"ping", "Hello\\r\\nWorld"}), std::string{"$14\r\nHello\\r\\nWorld\r\n"});
    TestHelper::assert("ping \"Hello World\"", RedisCommands::getResponse({"ping", "Hello World"}), std::string{"$11\r\nHello World\r\n"});
    TestHelper::assert("ping \"Hello\\r\\nWorld\"", RedisCommands::getResponse({"ping", "Hello\r\nWorld"}), std::string{"$12\r\nHello\r\nWorld\r\n"});
    
    TestHelper::result();
    return 0;
}
