#include <algorithm>
#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <iterator>
#include <string>

using boost::asio::ip::tcp;

using CharType = unsigned char;
using RecvBuffer = std::array<CharType, 128>;
using BytesCount = size_t;

enum struct DataTypes : CharType {
    SimpleString = '+',
    Error = '-',
    Integer = ':',
    BulkString = '$',
    Array = '*'
};

enum struct RESPCommand : int {
    DataType = 0
};

enum struct RESPBulkString : int {
    Length = 1
};

enum struct RESPArray : int {
    NumElements = 1
};

template<typename Enum>
auto getValue(const Enum& value) {
    return static_cast<typename std::underlying_type<Enum>::type>(value);
}

template<DataTypes T>
void parseRequest(const RecvBuffer& recv, const BytesCount bytesReceived) {
    std::cout << "Parsing for type is not implemented\n";
}

template<>
void parseRequest<DataTypes::BulkString>(const RecvBuffer& recv, const BytesCount bytesReceived) {
    const auto length = recv[getValue(RESPBulkString::Length)] - '0';
    const auto stringStart = std::find(recv.begin(), recv.end(), '\r') + 2;
    const std::string str {stringStart, stringStart + length};
    std::cout << str << '\n';
}

template<>
void parseRequest<DataTypes::Array>(const RecvBuffer& recv, const BytesCount bytesReceived) {
    const auto numElements = recv[getValue(RESPArray::NumElements)] - '0';
    auto findStart = recv.begin() + 4;
    for (auto i = 0; i < numElements; ++i) {
        const auto stringEnd = std::find(
                                std::find(findStart, recv.end(), '\r') + 1, 
                                    recv.end(), '\r');
        RecvBuffer bulkString {};
        std::copy(findStart, stringEnd + 1, bulkString.begin());
        const auto bytesToSend = std::distance(findStart, stringEnd + 1) + 1;
        findStart = stringEnd + 2;
        parseRequest<DataTypes::BulkString>(bulkString, bytesToSend);
    }
}

int main() {
    
    boost::asio::io_context ctx;
    tcp::acceptor acceptor(ctx, tcp::endpoint(tcp::v4(), 6379));
    std::cout << "Redis server running on port " << acceptor.local_endpoint().port() << '\n';
    
    while (true) {
        tcp::socket socket(ctx);
        acceptor.accept(socket);
        
        RecvBuffer recv {};
        boost::system::error_code ec;
        const auto bytesReceived = socket.read_some(boost::asio::buffer(recv), ec);
        DataTypes t { recv[getValue(RESPCommand::DataType)] };
        switch (t) {
            case DataTypes::BulkString: parseRequest<DataTypes::BulkString>(recv, bytesReceived); break;
            case DataTypes::Array: parseRequest<DataTypes::Array>(recv, bytesReceived); break;
        }
        
        // Response to COMMAND when only handling PING:
        /*
          1) 1) "ping"
             2) (integer) -1
             3) 1) "stale"
                2) "fast"
             4) (integer) 0
             5) (integer) 0
             6) (integer) 0
             7) 1) "@fast"
                2) "@connection"
        */
        //boost::asio::write(socket, boost::asio::buffer("+PONG\r\n"), ec);
    }
    return 0;
}
