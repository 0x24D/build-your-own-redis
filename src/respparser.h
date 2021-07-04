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
    [[nodiscard]] static inline auto parse_request(const RecvBuffer&) noexcept
        -> std::optional<std::vector<std::string>> {
        return {};
    }
};

template <>
[[nodiscard]] inline auto RESPParser::parse_request<DataTypes::BulkString>(
    const RecvBuffer& recv) noexcept -> std::optional<std::vector<std::string>> {
    auto it = std::ranges::find(recv, '\r');
    const std::string length{recv.cbegin() + 1, it};
    it += 2;
    const std::string str{it, it + std::stoi(length)};
    return std::vector{str};
}

template <>
[[nodiscard]] inline auto RESPParser::parse_request<DataTypes::Array>(
    const RecvBuffer& recv) noexcept -> std::optional<std::vector<std::string>> {
    std::vector<std::string> ret{};
    const auto num_elements_end = std::ranges::find(recv, '\r');
    const std::string num_elements{recv.begin() + 1, num_elements_end};
    auto string_start = num_elements_end + 2;
    for (auto i = 0; i < std::stoi(num_elements); ++i) {
        const DataTypes t{*string_start};
        if (t == DataTypes::BulkString) {
            RecvBuffer bulk_string{};
            std::copy(string_start, recv.end(), bulk_string.begin());
            const auto request = parse_request<DataTypes::BulkString>(bulk_string).value();
            ret.insert(ret.end(), request.cbegin(), request.cend());
            const auto return_size = ret[i].size();
            string_start += (1 + std::to_string(return_size).size() + 2 + return_size +
                             2);  // DataType, size of string length, \r\n, length of string, \r\n
        }
    }
    return ret;
}

#endif
