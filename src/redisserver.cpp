#include "redisserver.h"
#include "rediscommands.h"

RedisServer::RedisServer(unsigned int port) : m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)) {}

void RedisServer::listen() {
    while (true) {
        tcp::socket socket{m_ctx};
        m_acceptor.accept(socket);
        handleClient(socket);
    }
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
        const auto parsedRequest = parseRequest(recv, bytesReceived);
        boost::asio::write(socket, boost::asio::buffer(RedisCommands::getResponse(parsedRequest)), ec);
    }
}

std::vector<std::string> RedisServer::parseRequest(const RecvBuffer& recv, size_t bytesReceived) const {
    const DataTypes t {recv[0]};
    switch (t) {
        case DataTypes::BulkString:
            return RESPParser::parseRequest<DataTypes::BulkString>(recv, bytesReceived).value();
        case DataTypes::Array:
            return RESPParser::parseRequest<DataTypes::Array>(recv, bytesReceived).value();
        case DataTypes::SimpleString:
        case DataTypes::Error:
        case DataTypes::Integer:
            throw std::runtime_error(std::string{"Unhandled data type: '", recv[0],  '\''});
    }
    throw std::runtime_error(std::string{"Unknown data type: '", recv[0], '\''});
}
