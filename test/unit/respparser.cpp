#include "../../src/respparser.h"
#include "../utils/testhelper.h"

void testArray() {
    TestHelper::equals("Array of 1 Bulk String",
        RESPParser::parseRequest<DataTypes::Array>(RecvBuffer{"*1\r\n$5\r\nHello\r\n"}, -1).value(),
        std::vector{std::string{"Hello"}});

    TestHelper::equals("Array of 2 Bulk Strings",
        RESPParser::parseRequest<DataTypes::Array>(
            RecvBuffer{"*2\r\n$5\r\nHello\r\n$5\r\nWorld\r\n"}, -1)
            .value(),
        std::vector{std::string{"Hello"}, std::string{"World"}});

    TestHelper::equals("Array of 1 Bulk String with Space",
        RESPParser::parseRequest<DataTypes::Array>(RecvBuffer{"*1\r\n$11\r\nHello World\r\n"}, -1)
            .value(),
        std::vector{std::string{"Hello World"}});

    TestHelper::equals("Array of 2 Bulk Strings with Space",
        RESPParser::parseRequest<DataTypes::Array>(
            RecvBuffer{"*2\r\n$11\r\nHello World\r\n$11\r\nWorld Hello\r\n"}, -1)
            .value(),
        std::vector{std::string{"Hello World"}, std::string{"World Hello"}});

    TestHelper::equals("Array of 1 Bulk String with Newline",
        RESPParser::parseRequest<DataTypes::Array>(
            RecvBuffer{"*1\r\n$12\r\nHello\r\nWorld\r\n"}, -1)
            .value(),
        std::vector{std::string{"Hello\r\nWorld"}});

    TestHelper::equals("Array of 2 Bulk Strings with Newline",
        RESPParser::parseRequest<DataTypes::Array>(
            RecvBuffer{"*2\r\n$12\r\nHello\r\nWorld\r\n$12\r\nWorld\r\nHello\r\n"}, -1)
            .value(),
        std::vector{std::string{"Hello\r\nWorld"}, std::string{"World\r\nHello"}});
}

void testBulkString() {
    TestHelper::equals("Bulk String",
        RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$5\r\nHello\r\n"}, -1).value(),
        std::vector{std::string{"Hello"}});

    TestHelper::equals("Bulk String with Space",
        RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$11\r\nHello World\r\n"}, -1)
            .value(),
        std::vector{std::string{"Hello World"}});

    TestHelper::equals("Bulk String with Newline (Quoted)",
        RESPParser::parseRequest<DataTypes::BulkString>(RecvBuffer{"$12\r\nHello\r\nWorld\r\n"}, -1)
            .value(),
        std::vector{std::string{"Hello\r\nWorld"}});

    TestHelper::equals("Bulk String with Newline (Unquoted)",
        RESPParser::parseRequest<DataTypes::BulkString>(
            RecvBuffer{"$14\r\nHello\\r\\nWorld\r\n"}, -1)
            .value(),
        std::vector{std::string{"Hello\\r\\nWorld"}});
}

int main() {
    testArray();
    testBulkString();

    return TestHelper::result("unit/respparser");
}
