#include "../../src/rediscommands.h"
#include "../utils/testhelper.h"

int main() {
    // TODO: "command" response starts with "*numCommands*7\r\n
    TestHelper::equals("ping", RedisCommands::getResponse({"ping"}), std::string{"+PONG\r\n"});
    TestHelper::equals("PING", RedisCommands::getResponse({"PING"}), std::string{"+PONG\r\n"});
    TestHelper::equals("ping Hello", RedisCommands::getResponse({"ping", "Hello"}), std::string{"$5\r\nHello\r\n"});
    // TODO: Error handling
    //TestHelper::equals("ping Hello World", RedisCommands::getResponse({"ping", "Hello", "World"}), std::string{"-ERR wrong number of arguments for 'ping' command\r\n"});
    TestHelper::equals("ping Hello\\r\\nWorld", RedisCommands::getResponse({"ping", "Hello\\r\\nWorld"}), std::string{"$14\r\nHello\\r\\nWorld\r\n"});
    TestHelper::equals("ping \"Hello World\"", RedisCommands::getResponse({"ping", "Hello World"}), std::string{"$11\r\nHello World\r\n"});
    TestHelper::equals("ping \"Hello\\r\\nWorld\"", RedisCommands::getResponse({"ping", "Hello\r\nWorld"}), std::string{"$12\r\nHello\r\nWorld\r\n"});
    
    TestHelper::result();
    return 0;
}
