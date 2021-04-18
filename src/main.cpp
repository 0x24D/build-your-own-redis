#include <algorithm>
#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

using boost::asio::ip::tcp;

using CharType = unsigned char;
using RecvBuffer = std::array<CharType, 128>;

enum struct DataTypes : CharType {
    SimpleString = '+',
    Error = '-',
    Integer = ':',
    BulkString = '$',
    Array = '*'
};

template<DataTypes T>
std::optional<std::vector<std::string>> parseRequest(const RecvBuffer& recv, const size_t bytesReceived) {
    return {};
}
 
//TODO: Correctly handle bulk strings.
/*
127.0.0.1:6379> ping 12456\r\n
12456\r\n
127.0.0.1:6379> ping 124567\r\n
1
*/

template<>
std::optional<std::vector<std::string>> parseRequest<DataTypes::BulkString>(const RecvBuffer& recv, const size_t bytesReceived) {
    const auto length = recv[1] - '0';
    const auto stringStart = std::find(recv.begin(), recv.end(), '\r') + 2;
    const std::string str {stringStart, stringStart + length};
    return std::vector {str};
}

template<>
std::optional<std::vector<std::string>> parseRequest<DataTypes::Array>(const RecvBuffer& recv, const size_t bytesReceived) {
    std::vector<std::string> ret {};
    const auto numElements = recv[1] - '0';
    auto findStart = recv.begin() + 4;
    for (auto i = 0; i < numElements; ++i) {
        const DataTypes t {*findStart};
        if (t == DataTypes::BulkString) {
            const auto stringEnd = std::find(
                                    std::find(findStart, recv.end(), '\r') + 1, 
                                        recv.end(), '\r');
            RecvBuffer bulkString {};
            std::copy(findStart, stringEnd + 1, bulkString.begin());
            const auto bytesToSend = std::distance(findStart, stringEnd + 1) + 1;
            findStart = stringEnd + 2;
            const auto request = parseRequest<DataTypes::BulkString>(bulkString, bytesToSend).value();
            std::copy(request.begin(), request.end(), std::back_inserter(ret));
        }
    }
    return ret;
}

class RedisCommand {
public:
    RedisCommand(std::string name, int arity, std::vector<std::string> flags, unsigned int firstKeyPosition, int lastKeyPosition, unsigned int stepCount, std::vector<std::string> tag) : m_name(name), m_arity(arity), m_flags(flags), m_firstKeyPosition(firstKeyPosition), m_lastKeyPosition(lastKeyPosition), m_stepCount(stepCount), m_tags(tag) {}
    const std::string toString() const {
        std::string str {"*7\r\n"};
        str += '$'; 
        str += std::to_string(m_name.size()); 
        str += "\r\n";
        str += m_name; 
        str += "\r\n";
        str += ':'; 
        str += std::to_string(m_arity); 
        str += "\r\n";
        str += '*'; 
        str += std::to_string(m_flags.size()); 
        str += "\r\n";
        for (const auto& flag : m_flags) {
            str += '+'; 
            str += flag;
            str += "\r\n";
        }
        str += ':'; 
        str += std::to_string(m_firstKeyPosition); 
        str += "\r\n";
        str += ':'; 
        str += std::to_string(m_lastKeyPosition);
        str += "\r\n";
        str += ':'; 
        str += std::to_string(m_stepCount);
        str += "\r\n";
        str += '*'; 
        str += std::to_string(m_tags.size());
        str += "\r\n";
        for (const auto& tag : m_tags) {
            str += '+'; 
            str += tag; 
            str += "\r\n";
        }
        return str;
    }
    const auto getName() const {
        return m_name;
    }
private:    
    std::string m_name;
    int m_arity;
    std::vector<std::string> m_flags;
    unsigned int m_firstKeyPosition;
    int m_lastKeyPosition;
    unsigned int m_stepCount;
    std::vector<std::string> m_tags;
};

std::string makeCommandResponse(std::array<RedisCommand, 2> commands) {
    std::string res {'*'}; 
    res += std::to_string(commands.size());
    res += "\r\n";
    for (const auto& command : commands) {
        res += command.toString();
    }
    return res;
}

int main() {
    const RedisCommand commandCmd {"command", -1, {"random", "loading", "stale"}, 0, 0, 0, {"@slow", "@connection"}};
    const RedisCommand pingCmd {"ping", -1, {"stale", "fast"}, 0, 0, 0, {"@fast", "@connection"}};
    const std::array commandsImplemented {commandCmd, pingCmd};
    const auto commandResponse = makeCommandResponse(commandsImplemented);   
    
    const auto commandCmdResponse = [&commandResponse](const std::vector<std::string>& parsedRequest) { return commandResponse; }; // TODO: Handle COMMAND with additional args.
    const auto pingCmdResponse = [](const std::vector<std::string>& parsedRequest) { 
        std::string str {};
        if (parsedRequest.size() == 1) {
            str = "+PONG\r\n";
        } else {
            if (parsedRequest[1].find("\r\n") == std::string::npos) {
                str = '+';
            } else {
                str = '$' + std::to_string(parsedRequest[1].size()) + "\r\n";
            }
            str += parsedRequest[1] + "\r\n";
        }
        return str;
    };

    const std::map<std::string, std::function<std::string(const std::vector<std::string>&)>> requestResponses {
        {commandCmd.getName(), commandCmdResponse},
        {pingCmd.getName(), pingCmdResponse}
    };
    
    boost::asio::io_context ctx;
    tcp::acceptor acceptor(ctx, tcp::endpoint(tcp::v4(), 6379));
    tcp::socket socket(ctx);
    acceptor.accept(socket);
    while (true) {
        RecvBuffer recv {};
        boost::system::error_code ec;
        size_t bytesReceived = socket.read_some(boost::asio::buffer(recv), ec);
        if (ec == boost::asio::error::eof) {
            break;
        } else if (ec) {
            throw boost::system::system_error(ec);
        }
        std::vector<std::string> parsedRequest {};
        const DataTypes t {recv[0]};
        switch (t) {
            case DataTypes::BulkString:
            { 
                const auto request = parseRequest<DataTypes::BulkString>(recv, bytesReceived).value();
                std::copy(request.begin(), request.end(), std::back_inserter(parsedRequest));
                break;
            }
            case DataTypes::Array:
            { 
                const auto request = parseRequest<DataTypes::Array>(recv, bytesReceived).value(); 
                std::copy(request.begin(), request.end(), std::back_inserter(parsedRequest));
                break;
            }
        }
        std::string request {};
        std::transform(parsedRequest[0].begin(), parsedRequest[0].end(), std::back_inserter(request), ::tolower);
        boost::asio::write(socket, boost::asio::buffer(requestResponses.at(request)(parsedRequest)), ec);
    }
    return 0;
}
