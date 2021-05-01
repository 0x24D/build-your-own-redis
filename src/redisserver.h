#ifndef RedisServerH
#define RedisServerH

#include "respparser.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class RedisServer {
public:
    RedisServer(unsigned int port = 6379);
    void listen();
private:
    void handleClient(tcp::socket& socket);
    std::vector<std::string> parseRequest(const RecvBuffer& recv, size_t bytesReceived) const;
    boost::asio::io_context m_ctx;
    tcp::acceptor m_acceptor;
};

#endif
