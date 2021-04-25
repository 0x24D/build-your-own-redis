#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class RedisServer {
public:
    RedisServer(unsigned int port);
    void listen();
private:
    void handleClient(tcp::socket& socket);
    boost::asio::io_context m_ctx;
    tcp::acceptor m_acceptor;
};
