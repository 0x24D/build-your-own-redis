#ifndef RESPParserH
#define RESPParserH

#include <algorithm>
#include <array>
#include <optional>
#include <string>
#include <vector>

using CharType = unsigned char;
using RecvBuffer = std::array<CharType, 128>;

enum struct DataTypes : CharType {
    SimpleString = '+',
    Error = '-',
    Integer = ':',
    BulkString = '$',
    Array = '*'
};

class RESPParser {
public:
    template <DataTypes T>
    static inline std::optional<std::vector<std::string>> parseRequest(const RecvBuffer&) {
        return {};
    }
};

template <>
inline std::optional<std::vector<std::string>> RESPParser::parseRequest<DataTypes::BulkString>(
    const RecvBuffer& recv) {
    auto it = std::ranges::find(recv, '\r');
    const std::string length{recv.begin() + 1, it};
    it += 2;
    const std::string str{it, it + std::stoi(length)};
    return std::vector{str};
}

template <>
inline std::optional<std::vector<std::string>> RESPParser::parseRequest<DataTypes::Array>(
    const RecvBuffer& recv) {
    std::vector<std::string> ret{};
    const auto numElementsEnd = std::ranges::find(recv, '\r');
    const std::string numElements{recv.begin() + 1, numElementsEnd};
    auto stringStart = numElementsEnd + 2;
    for (auto i = 0; i < std::stoi(numElements); ++i) {
        const DataTypes t{*stringStart};
        if (t == DataTypes::BulkString) {
            RecvBuffer bulkString{};
            std::copy(stringStart, recv.end(), bulkString.begin());
            const auto request = parseRequest<DataTypes::BulkString>(bulkString).value();
            ret.insert(ret.end(), request.begin(), request.end());
            const auto returnSize = ret[i].size();
            stringStart += (1 + std::to_string(returnSize).size() + 2 + returnSize +
                            2);  // DataType, size of string length, \r\n, length of string, \r\n
        }
    }
    return ret;
}

#endif
