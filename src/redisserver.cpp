#include "redisserver.h"
#include "redisclient.h"

RedisServer::RedisServer(unsigned int port) : m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)) {}

void RedisServer::listen() {
    auto client = std::make_shared<RedisClient>(m_ctx);
    m_acceptor.async_accept(client->get_socket(),
        std::bind(&RedisServer::handle_accept, this, client, std::placeholders::_1));
    m_ctx.run();
}

void RedisServer::handle_accept(
    const std::shared_ptr<RedisClient>& client, const boost::system::error_code& ec) {
    if (!ec)
        client->listen();
    listen();
}
