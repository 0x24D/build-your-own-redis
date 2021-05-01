#include "../../src/redisserver.h"
#include "../utils/testhelper.h"
#include "../utils/testclient.h"

void testCommand() {
    TestClient client{};
    
    client.send("$7\r\ncommand\r\n");
    TestHelper::startsWith(std::string{"command"}, client.recv(), std::string{"*2\r\n*7\r\n"});
    
    client.send("$7\r\nCOMMAND\r\n");
    TestHelper::startsWith(std::string{"COMMAND"}, client.recv(), std::string{"*2\r\n*7\r\n"});
}

void testConcurrentClients() {
    TestClient client1{};
    TestClient client2{};

    client1.send("$4\r\nping\r\n");
    client1.recv();

    client2.send("$4\r\nping\r\n");
    client2.recv();
}

void testPing() {
    TestClient client{};
    
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
}

int main() {
    RedisServer server{};
    std::thread serverThread(&RedisServer::listen, &server);
    
    testCommand();
    // TODO: Implement concurrent clients
    //testConcurrentClients();
    testPing();

    serverThread.detach();
    
    return TestHelper::result("integration/redisserver");
}
