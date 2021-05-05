#ifndef RedisServerH
#define RedisServerH

#include <boost/asio.hpp>
#include "respparser.h"

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
