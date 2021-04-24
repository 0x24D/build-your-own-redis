#include <optional>

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
    template<DataTypes T>
    static std::optional<std::vector<std::string>> parseRequest(const RecvBuffer& recv, const size_t bytesReceived) {
        return {};
    }
};

template<>
std::optional<std::vector<std::string>> RESPParser::parseRequest<DataTypes::BulkString>(const RecvBuffer& recv, const size_t bytesReceived) {
    std::string length {};
    auto it = std::find(recv.begin(), recv.end(), '\r');
    std::copy(recv.begin() + 1, it, std::back_inserter(length));
    it += 2;
    const std::string str {it, it + std::stoi(length)};
    return std::vector {str};
}
template<>
std::optional<std::vector<std::string>> RESPParser::parseRequest<DataTypes::Array>(const RecvBuffer& recv, const size_t bytesReceived) {
    std::vector<std::string> ret {};
    std::string numElements {};
    const auto numElementsEnd = std::find(recv.begin(), recv.end(), '\r');
    std::copy(recv.begin() + 1, numElementsEnd, std::back_inserter(numElements));
    auto stringStart = numElementsEnd + 2;
    for (auto i = 0; i < std::stoi(numElements); ++i) {
        const DataTypes t {*stringStart};
        if (t == DataTypes::BulkString) {
            RecvBuffer bulkString {};
            std::copy(stringStart, recv.end(), bulkString.begin());
            const auto bytesToSend = std::distance(stringStart, recv.end()) + 1;
            const auto request = parseRequest<DataTypes::BulkString>(bulkString, bytesToSend).value();
            std::copy(request.begin(), request.end(), std::back_inserter(ret));
            const auto returnSize = ret[i].size();
            stringStart += (1 + std::to_string(returnSize).size() + 2 + returnSize + 2); // DataType, size of string length, \r\n, length of string, \r\n 
        }
    }
    return ret;
}