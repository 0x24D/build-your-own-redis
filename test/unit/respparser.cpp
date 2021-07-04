#include "../../src/respparser.h"
#include "../utils/testhelper.h"

void test_array() {
    TestHelper::equals("Array of 1 Bulk String",
        RESPParser::parse_request<DataTypes::Array>(RecvBuffer{"*1\r\n$5\r\nHello\r\n"}).value(),
        std::vector{std::string{"Hello"}});

    TestHelper::equals("Array of 2 Bulk Strings",
        RESPParser::parse_request<DataTypes::Array>(
            RecvBuffer{"*2\r\n$5\r\nHello\r\n$5\r\nWorld\r\n"})
            .value(),
        std::vector{std::string{"Hello"}, std::string{"World"}});

    TestHelper::equals("Array of 1 Bulk String with Space",
        RESPParser::parse_request<DataTypes::Array>(RecvBuffer{"*1\r\n$11\r\nHello World\r\n"})
            .value(),
        std::vector{std::string{"Hello World"}});

    TestHelper::equals("Array of 2 Bulk Strings with Space",
        RESPParser::parse_request<DataTypes::Array>(
            RecvBuffer{"*2\r\n$11\r\nHello World\r\n$11\r\nWorld Hello\r\n"})
            .value(),
        std::vector{std::string{"Hello World"}, std::string{"World Hello"}});

    TestHelper::equals("Array of 1 Bulk String with Newline",
        RESPParser::parse_request<DataTypes::Array>(RecvBuffer{"*1\r\n$12\r\nHello\r\nWorld\r\n"})
            .value(),
        std::vector{std::string{"Hello\r\nWorld"}});

    TestHelper::equals("Array of 2 Bulk Strings with Newline",
        RESPParser::parse_request<DataTypes::Array>(
            RecvBuffer{"*2\r\n$12\r\nHello\r\nWorld\r\n$12\r\nWorld\r\nHello\r\n"})
            .value(),
        std::vector{std::string{"Hello\r\nWorld"}, std::string{"World\r\nHello"}});
}

void test_bulk_string() {
    TestHelper::equals("Bulk String",
        RESPParser::parse_request<DataTypes::BulkString>(RecvBuffer{"$5\r\nHello\r\n"}).value(),
        std::vector{std::string{"Hello"}});

    TestHelper::equals("Bulk String with Space",
        RESPParser::parse_request<DataTypes::BulkString>(RecvBuffer{"$11\r\nHello World\r\n"})
            .value(),
        std::vector{std::string{"Hello World"}});

    TestHelper::equals("Bulk String with Newline (Quoted)",
        RESPParser::parse_request<DataTypes::BulkString>(RecvBuffer{"$12\r\nHello\r\nWorld\r\n"})
            .value(),
        std::vector{std::string{"Hello\r\nWorld"}});

    TestHelper::equals("Bulk String with Newline (Unquoted)",
        RESPParser::parse_request<DataTypes::BulkString>(RecvBuffer{"$14\r\nHello\\r\\nWorld\r\n"})
            .value(),
        std::vector{std::string{"Hello\\r\\nWorld"}});
}

int main() {
    test_array();
    test_bulk_string();

    return TestHelper::result("unit/respparser");
}
