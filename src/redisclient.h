#ifndef RedisClientH
#define RedisClientH

#include <boost/asio.hpp>
#include "respparser.h"

using boost::asio::ip::tcp;

class RedisClient {
public:
    [[nodiscard]] RedisClient(boost::asio::io_context& ctx);
    void listen();
    [[nodiscard]] tcp::socket& getSocket();
private:
    void handleRead(const boost::system::error_code& ec);
    [[nodiscard]] auto parseRequest(const RecvBuffer& recv) const;
    boost::asio::io_context& m_ctx;
    RecvBuffer m_buffer;
    tcp::socket m_socket{m_ctx};
};

#endif
