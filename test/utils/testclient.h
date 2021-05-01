#ifndef TestClientH
#define TestClientH

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class TestClient {
public:
    TestClient(unsigned int port);
    void send(const std::string& cmd);
    std::string recv();
    void close();
private:
    boost::asio::io_context m_ctx;
    tcp::resolver m_resolver;
    tcp::socket m_socket;
};

#endif
