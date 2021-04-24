#include <iostream>

class TestHelper {
public:
    static void assert(const std::string& label, const auto& response, const auto& expectedResponse) {
        if (response == expectedResponse) {
            std::cout << label << ": " << "Pass\n";
        } else {
            std::cerr << label << ": " << "Fail\n";
        }
    }
};

