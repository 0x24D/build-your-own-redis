#include "../src/respparser.h"
#include "testhelper.h"

int main() {
    TestHelper::assert("Bulk String", RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$5\r\nHello\r\n"}, -1).value(), std::vector{std::string{"Hello"}});
    TestHelper::assert("Bulk String with Space", RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$11\r\nHello World\r\n"}, -1).value(), std::vector{std::string{"Hello World"}});
    TestHelper::assert("Bulk String with Newline (Quoted)", RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$12\r\nHello\r\nWorld\r\n"}, -1).value(), std::vector{std::string{"Hello\r\nWorld"}});
    TestHelper::assert("Bulk String with Newline (Unquoted)", RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$14\r\nHello\\r\\nWorld\r\n"}, -1).value(), std::vector{std::string{"Hello\\r\\nWorld"}});

    TestHelper::assert("Array of 1 Bulk String", RESPParser::parseRequest<DataTypes::Array>(RecvBuffer{"*1\r\n$5\r\nHello\r\n"}, -1).value(), std::vector{std::string{"Hello"}});
    TestHelper::assert("Array of 2 Bulk Strings", RESPParser::parseRequest<DataTypes::Array>(RecvBuffer{"*2\r\n$5\r\nHello\r\n$5\r\nWorld\r\n"}, -1).value(), std::vector{std::string{"Hello"}, std::string{"World"}});
    TestHelper::assert("Array of 1 Bulk String with Space", RESPParser::parseRequest<DataTypes::Array>(RecvBuffer{"*1\r\n$11\r\nHello World\r\n"}, -1).value(), std::vector{std::string{"Hello World"}});
    TestHelper::assert("Array of 2 Bulk Strings with Space", RESPParser::parseRequest<DataTypes::Array>(RecvBuffer{"*2\r\n$11\r\nHello World\r\n$11\r\nWorld Hello\r\n"}, -1).value(), std::vector{std::string{"Hello World"}, std::string{"World Hello"}});
    
    TestHelper::assert("Array of 1 Bulk String with Newline", RESPParser::parseRequest<DataTypes::Array>(RecvBuffer{"*1\r\n$12\r\nHello\r\nWorld\r\n"}, -1).value(), std::vector{std::string{"Hello\r\nWorld"}});
    TestHelper::assert("Array of 2 Bulk Strings with Newline", RESPParser::parseRequest<DataTypes::Array>(RecvBuffer{"*2\r\n$12\r\nHello\r\nWorld\r\n$12\r\nWorld\r\nHello\r\n"}, -1).value(), std::vector{std::string{"Hello\r\nWorld"}, std::string{"World\r\nHello"}});

    TestHelper::result();
    return 0;
}
