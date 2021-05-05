#ifndef TestClientH
#define TestClientH

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class TestClient {
public:
    TestClient(unsigned int port = 6379);
    ~TestClient();
    TestClient(TestClient &&) = delete;
    TestClient(const TestClient &) = delete;
    TestClient &operator=(const TestClient &) = delete;
    TestClient &operator=(TestClient &&) = delete;
    void send(const std::string &cmd);
    std::string recv();
private:
    boost::asio::io_context m_ctx;
    tcp::resolver m_resolver;
    tcp::socket m_socket;
};

#endif
