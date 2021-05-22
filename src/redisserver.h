#ifndef RedisServerH
#define RedisServerH

#include <boost/asio.hpp>

class RedisClient;

using boost::asio::ip::tcp;

class RedisServer {
public:
    [[nodiscard]] explicit RedisServer(unsigned int port = 6379);
    void listen();
private:
    void handleAccept(
        const std::shared_ptr<RedisClient>& client, const boost::system::error_code& ec);
    boost::asio::io_context m_ctx;
    tcp::acceptor m_acceptor;
};

#endif
