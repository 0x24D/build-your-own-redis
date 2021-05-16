#include "../../src/redisserver.h"
#include "../utils/testclient.h"
#include "../utils/testhelper.h"

void testCommand() {
    TestClient client{};

    // Command details
    client.send("$7\r\ncommand\r\n");
    TestHelper::startsWith("command", client.recv(), std::string_view{"*3\r\n*7\r\n"});

    client.send("$7\r\nCOMMAND\r\n");
    TestHelper::startsWith("COMMAND", client.recv(), std::string_view{"*3\r\n*7\r\n"});

    // Number of commands
    client.send("*2\r\n$7\r\ncommand\r\n$5\r\ncount\r\n");
    TestHelper::equals("command count", client.recv(), std::string_view{":3\r\n"});

    client.send("*2\r\n$7\r\nCOMMAND\r\n$5\r\nCOUNT\r\n");
    TestHelper::equals("COMMAND COUNT", client.recv(), std::string_view{":3\r\n"});

    // Specific command details
    client.send("*3\r\n$7\r\ncommand\r\n$4\r\ninfo\r\n$4\r\nping\r\n");
    TestHelper::startsWith("command info ping", client.recv(), std::string_view{"*1\r\n*7\r\n"});

    client.send("*3\r\n$7\r\nCOMMAND\r\n$4\r\nINFO\r\n$4\r\nPING\r\n");
    TestHelper::startsWith("COMMAND INFO PING", client.recv(), std::string_view{"*1\r\n*7\r\n"});

    client.send("*4\r\n$7\r\ncommand\r\n$4\r\ninfo\r\n$4\r\necho\r\n$4\r\nping\r\n");
    TestHelper::startsWith(
        "command info echo ping", client.recv(), std::string_view{"*2\r\n*7\r\n"});

    client.send("*4\r\n$7\r\nCOMMAND\r\n$4\r\nINFO\r\n$4\r\nECHO\r\n$4\r\nPING\r\n");
    TestHelper::startsWith(
        "COMMAND INFO ECHO PING", client.recv(), std::string_view{"*2\r\n*7\r\n"});

    // No command
    client.send("*2\r\n$7\r\ncommand\r\n$4\r\ninfo\r\n");
    TestHelper::equals("command info", client.recv(), std::string_view{"*0\r\n"});

    client.send("*2\r\n$7\r\nCOMMAND\r\n$4\r\nINFO\r\n");
    TestHelper::equals("COMMAND INFO", client.recv(), std::string_view{"*0\r\n"});

    // Non-existing command
    client.send("*3\r\n$7\r\ncommand\r\n$4\r\ninfo\r\n$3\r\nfoo\r\n");
    TestHelper::equals("command info foo", client.recv(), std::string_view{"*1\r\n*0\r\n"});

    client.send("*3\r\n$7\r\nCOMMAND\r\n$4\r\nINFO\r\n$3\r\nFOO\r\n");
    TestHelper::equals("COMMAND INFO FOO", client.recv(), std::string_view{"*1\r\n*0\r\n"});
}

void testConcurrentClients() {
    TestClient client1{};
    TestClient client2{};
    TestClient client3{};

    client1.send("$4\r\nping\r\n");
    client2.send("*2\r\n$4\r\nping\r\n$5\r\nHello\r\n");
    client3.send("*2\r\n$4\r\nping\r\n$5\r\nWorld\r\n");

    // Get response in order sent
    TestHelper::equals("Client 1 -> ping", client1.recv(), std::string_view{"+PONG\r\n"});
    TestHelper::equals(
        "Client 2 -> ping Hello", client2.recv(), std::string_view{"$5\r\nHello\r\n"});
    TestHelper::equals(
        "Client 3 -> ping World", client3.recv(), std::string_view{"$5\r\nWorld\r\n"});

    client1.send("$4\r\nping\r\n");
    client2.send("*2\r\n$4\r\nping\r\n$5\r\nHello\r\n");
    client3.send("*2\r\n$4\r\nping\r\n$5\r\nWorld\r\n");

    // Get response out of order
    TestHelper::equals(
        "Client 2 -> ping Hello", client2.recv(), std::string_view{"$5\r\nHello\r\n"});
    TestHelper::equals("Client 1 -> ping", client1.recv(), std::string_view{"+PONG\r\n"});
    TestHelper::equals(
        "Client 3 -> ping World", client3.recv(), std::string_view{"$5\r\nWorld\r\n"});
}

void testEcho() {
    TestClient client{};

    // TODO: Error handling
    /*client.send("$4\r\necho\r\n");
    TestHelper::equals("echo", client.recv(), std::string_view{"-ERR wrong number of arguments for
    'echo' command\r\n"});*/

    // TODO: Error handling
    /*client.send("$4\r\nECHO\r\n");
    TestHelper::equals("ECHO", client.recv(), std::string_view{"-ERR wrong number of arguments for
    'echo' command\r\n"});*/

    client.send("*2\r\n$4\r\necho\r\n$5\r\nHello\r\n");
    TestHelper::equals("echo Hello", client.recv(), std::string_view{"$5\r\nHello\r\n"});

    // TODO: Error handling
    /*client.send("*3\r\n$4\r\necho\r\n$5Hello\r\n$5\r\nWorld\r\n");
    TestHelper::equals("echo Hello World", client.recv(), std::string_view{"-ERR wrong
    number of arguments for 'echo' command\r\n"});*/

    client.send("*2\r\n$4\r\nECHO\r\n$14\r\nHello\\r\\nWorld\r\n");
    TestHelper::equals(
        "echo Hello\\r\\nWorld", client.recv(), std::string_view{"$14\r\nHello\\r\\nWorld\r\n"});

    client.send("*2\r\n$4\r\nECHO\r\n$11\r\nHello World\r\n");
    TestHelper::equals(
        "echo \"Hello World\"", client.recv(), std::string_view{"$11\r\nHello World\r\n"});

    client.send("*2\r\n$4\r\nECHO\r\n$12\r\nHello\r\nWorld\r\n");
    TestHelper::equals(
        "echo \"Hello\\r\\nWorld\"", client.recv(), std::string_view{"$12\r\nHello\r\nWorld\r\n"});
}

void testPing() {
    TestClient client{};

    client.send("$4\r\nping\r\n");
    TestHelper::equals("ping", client.recv(), std::string_view{"+PONG\r\n"});

    client.send("$4\r\nPING\r\n");
    TestHelper::equals("PING", client.recv(), std::string_view{"+PONG\r\n"});

    client.send("*2\r\n$4\r\nping\r\n$5\r\nHello\r\n");
    TestHelper::equals("ping Hello", client.recv(), std::string_view{"$5\r\nHello\r\n"});

    // TODO: Error handling
    /*client.send("*3\r\n$4\r\nping\r\n$5Hello\r\n$5\r\nWorld\r\n");
    TestHelper::equals("ping Hello World", client.recv(), std::string_view{"-ERR wrong
    number of arguments for 'ping' command\r\n"});*/

    client.send("*2\r\n$4\r\nPING\r\n$14\r\nHello\\r\\nWorld\r\n");
    TestHelper::equals(
        "ping Hello\\r\\nWorld", client.recv(), std::string_view{"$14\r\nHello\\r\\nWorld\r\n"});

    client.send("*2\r\n$4\r\nPING\r\n$11\r\nHello World\r\n");
    TestHelper::equals(
        "ping \"Hello World\"", client.recv(), std::string_view{"$11\r\nHello World\r\n"});

    client.send("*2\r\n$4\r\nPING\r\n$12\r\nHello\r\nWorld\r\n");
    TestHelper::equals(
        "ping \"Hello\\r\\nWorld\"", client.recv(), std::string_view{"$12\r\nHello\r\nWorld\r\n"});
}

int main() {
    RedisServer server{};
    std::thread serverThread(&RedisServer::listen, &server);

    testCommand();
    testConcurrentClients();
    testEcho();
    testPing();

    serverThread.detach();

    return TestHelper::result("integration/redisserver");
}
