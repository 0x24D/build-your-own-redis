#include <boost/asio.hpp>
#include <string>
#include <vector>
#include "rediscommand.h"

using boost::asio::ip::tcp;

class RedisServer {
public:
    RedisServer(unsigned int port);
    void listen();
private:
    void handleClient(tcp::socket& socket);
    std::string makeCommandResponse(std::array<RedisCommand, 2> commands);
    boost::asio::io_context m_ctx;
    tcp::acceptor m_acceptor;
    std::map<std::string, std::function<std::string(const std::vector<std::string>&)>> m_requestResponses;   
};
