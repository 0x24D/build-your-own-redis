#include "../src/respparser.h"
#include "testhelper.h"

int main() {
        TestHelper::assert("Bulk String", RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$5\r\nHello\r\n"}, -1), std::vector{std::string{"Hello"}});
        TestHelper::assert("Bulk String with Space", RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$11\r\nHello World\r\n"}, -1), std::vector{std::string{"Hello World"}});
        TestHelper::assert("Bulk String with Newline", RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$12\r\nHello\r\nWorld\r\n"}, -1), std::vector{std::string{"Hello\r\nWorld"}});
    return 0;
}
