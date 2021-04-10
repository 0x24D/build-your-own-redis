#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

int main() {
    
    boost::asio::io_context ctx;
    tcp::acceptor acceptor(ctx, tcp::endpoint(tcp::v4(), 6379));
    
    while (true) {
        tcp::socket socket(ctx);
        acceptor.accept(socket);

        boost::system::error_code ec;
        boost::asio::write(socket, boost::asio::buffer("New client accepted\n"), ec);
    }

    return 0;
}
