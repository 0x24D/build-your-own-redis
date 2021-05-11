#ifndef RedisServerH
#define RedisServerH

#include <boost/asio.hpp>
#include "respparser.h"

using boost::asio::ip::tcp;

class RedisServer {
public:
    [[nodiscard]] RedisServer(unsigned int port = 6379);
    void listen();
private:
    void handleClient(tcp::socket& socket);
    [[nodiscard]] auto parseRequest(const RecvBuffer& recv) const;
    boost::asio::io_context m_ctx;
    tcp::acceptor m_acceptor;
};

#endif
