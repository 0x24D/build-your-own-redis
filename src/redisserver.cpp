#include "redisserver.h"
#include "rediscommands.h"
#include "respparser.h"

RedisServer::RedisServer(unsigned int port) : m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)) {}

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
        boost::asio::write(socket, boost::asio::buffer(RedisCommands::getResponse(parsedRequest)), ec);
    }
}
