#ifndef RedisClientH
#define RedisClientH

#include <boost/asio.hpp>
#include "respparser.h"

using boost::asio::ip::tcp;

class RedisClient {
public:
    [[nodiscard]] explicit RedisClient(boost::asio::io_context& ctx);
    void listen();
    [[nodiscard]] tcp::socket& get_socket();
private:
    void handle_read(const boost::system::error_code& ec);
    [[nodiscard]] static auto parse_request(const RecvBuffer& recv);
    boost::asio::io_context& m_ctx;
    RecvBuffer m_buffer{};
    tcp::socket m_socket{m_ctx};
};

#endif
