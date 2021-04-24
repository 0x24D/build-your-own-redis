#include "redisserver.h"
#include "respparser.h"

#include <iostream>

RedisServer::RedisServer(unsigned int port) : m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)) {
    const RedisCommand commandCmd {"command", -1, {"random", "loading", "stale"}, 0, 0, 0, {"@slow", "@connection"}};
    const RedisCommand pingCmd {"ping", -1, {"stale", "fast"}, 0, 0, 0, {"@fast", "@connection"}};
    const std::array commandsImplemented {commandCmd, pingCmd};
    const auto commandResponse = makeCommandResponse(commandsImplemented);   
    const auto commandCmdResponse = [commandResponse](const std::vector<std::string>& parsedRequest) { return commandResponse; }; // TODO: Handle COMMAND with additional args.
    const auto pingCmdResponse = [](const std::vector<std::string>& parsedRequest) { 
        std::string str {};
        if (parsedRequest.size() == 1) {
            str = "+PONG\r\n";
        } else {
            str = '$';
            str += std::to_string(parsedRequest[1].size());
            str += "\r\n";
            str += parsedRequest[1];
            str += "\r\n";
        }
        return str;
    };
    m_requestResponses = {
        {commandCmd.getName(), commandCmdResponse},
        {pingCmd.getName(), pingCmdResponse}
    };
}

void RedisServer::listen() {
    tcp::socket socket{m_ctx};
    m_acceptor.accept(socket);
    handleClient(socket);
}

void RedisServer::handleClient(tcp::socket& socket) {
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
                const auto request = RESPParser::parseRequest<DataTypes::BulkString>(recv, bytesReceived).value();
                std::copy(request.begin(), request.end(), std::back_inserter(parsedRequest));
                break;
            }
            case DataTypes::Array:
            { 
                const auto request = RESPParser::parseRequest<DataTypes::Array>(recv, bytesReceived).value(); 
                std::copy(request.begin(), request.end(), std::back_inserter(parsedRequest));
                break;
            }
        }
        std::string request {};
        std::transform(parsedRequest[0].begin(), parsedRequest[0].end(), std::back_inserter(request), ::tolower);
        boost::asio::write(socket, boost::asio::buffer(m_requestResponses.at(request)(parsedRequest)), ec);
    }
}
 
std::string RedisServer::makeCommandResponse(std::array<RedisCommand, 2> commands) {
    std::string res {'*'}; 
    res += std::to_string(commands.size());
    res += "\r\n";
    for (const auto& command : commands) {
        res += command.toString();
    }
    return res;
}
