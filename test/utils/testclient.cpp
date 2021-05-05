#include "testclient.h"

TestClient::TestClient(unsigned int port) : m_resolver{m_ctx}, m_socket{m_ctx} {
    const auto endpoints =
        m_resolver.resolve("localhost", std::to_string(port), tcp::resolver::query::canonical_name);
    boost::asio::connect(m_socket, endpoints);
}

TestClient::~TestClient() {
    m_socket.close();
}

void TestClient::send(const std::string& cmd) {
    boost::system::error_code ec;
    boost::asio::write(m_socket, boost::asio::buffer(cmd), ec);
}

std::string TestClient::recv() {
    std::array<unsigned char, 1024> recv;
    boost::system::error_code ec;
    size_t bytesReceived;
    do {
        bytesReceived = m_socket.read_some(boost::asio::buffer(recv), ec);
    } while (bytesReceived == 0);
    std::string ret;
    std::copy(recv.begin(), recv.begin() + bytesReceived, std::back_inserter(ret));
    return ret;
}
