#include "redisclient.h"
#include "rediscommands.h"

RedisClient::RedisClient(boost::asio::io_context& ctx) : m_ctx(ctx) {}

void RedisClient::listen() {
    m_socket.async_read_some(boost::asio::buffer(m_buffer),
        std::bind(&RedisClient::handle_read, this, std::placeholders::_1));
}

tcp::socket& RedisClient::get_socket() {
    return m_socket;
}

auto RedisClient::parse_request(const RecvBuffer& recv) {
    const DataTypes t{recv[0]};
    switch (t) {
        case DataTypes::BulkString:
            return RESPParser::parse_request<DataTypes::BulkString>(recv).value();
        case DataTypes::Array:
            return RESPParser::parse_request<DataTypes::Array>(recv).value();
        case DataTypes::SimpleString:
        case DataTypes::Error:
        case DataTypes::Integer:
            throw std::runtime_error(
                "Unhandled data type: '" + std::string{static_cast<char>(recv[0]), '\''});
    }
    throw std::runtime_error(
        "Unknown data type: '" + std::string{static_cast<char>(recv[0]), '\''});
}

void RedisClient::handle_read(const boost::system::error_code& ec) {
    if (ec == boost::asio::error::eof) {
        return;
    } else if (ec) {
        throw boost::system::system_error(ec);
    }
    const auto parsed_request = parse_request(m_buffer);
    boost::system::error_code write_ec;
    boost::asio::write(
        m_socket, boost::asio::buffer(RedisCommands::get_response(parsed_request)), write_ec);

    m_socket.async_read_some(boost::asio::buffer(m_buffer),
        std::bind(&RedisClient::handle_read, this, std::placeholders::_1));
}
