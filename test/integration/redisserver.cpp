#include "../../src/redisserver.h"
#include "../utils/testhelper.h"
#include "../utils/testclient.h"

int main() {
    RedisServer server{6379};
    std::thread serverThread(&RedisServer::listen, &server);
    TestClient client{6379};
    
    client.send("$7\r\ncommand\r\n");
    TestHelper::startsWith(std::string{"command"}, client.recv(), std::string{"*2\r\n*7\r\n"});
    
    client.send("$7\r\nCOMMAND\r\n");
    TestHelper::startsWith(std::string{"COMMAND"}, client.recv(), std::string{"*2\r\n*7\r\n"});

    client.send("$4\r\nping\r\n");
    TestHelper::equals(std::string{"ping"}, client.recv(), std::string{"+PONG\r\n"});
    
    client.send("$4\r\nPING\r\n");
    TestHelper::equals(std::string{"PING"}, client.recv(), std::string{"+PONG\r\n"});
    
    client.send("*2\r\n$4\r\nping\r\n$5\r\nHello\r\n");
    TestHelper::equals(std::string{"ping Hello"}, client.recv(), std::string{"$5\r\nHello\r\n"});
    
    // TODO: Error handling
    /*client.send("*3\r\n$4\r\nping\r\n$5Hello\r\n$5\r\nWorld\r\n");
    TestHelper::equals(std::string{"ping Hello World"}, client.recv(), std::string{"-ERR wrong number of arguments for 'ping' command\r\n"});*/
    
    client.send("*2\r\n$4\r\nPING\r\n$14\r\nHello\\r\\nWorld\r\n");
    TestHelper::equals(std::string{"ping Hello\\r\\nWorld"}, client.recv(), std::string{"$14\r\nHello\\r\\nWorld\r\n"});
    
    client.send("*2\r\n$4\r\nPING\r\n$11\r\nHello World\r\n");
    TestHelper::equals(std::string{"ping \"Hello World\""}, client.recv(), std::string{"$11\r\nHello World\r\n"});
    
    client.send("*2\r\n$4\r\nPING\r\n$12\r\nHello\r\nWorld\r\n");
    TestHelper::equals(std::string{"ping \"Hello\\r\\nWorld\""}, client.recv(), std::string{"$12\r\nHello\r\nWorld\r\n"});

    client.close();    
    serverThread.join();
    
    TestHelper::result();    
    return 0;
}
