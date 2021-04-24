#include <iostream>
unsigned int testsRan = 0;
unsigned int testsFailed = 0;
class TestHelper {
public:
    static void assert(const std::string& label, const auto& response, const auto& expectedResponse) {
        if (response != expectedResponse) {
            std::cerr << "Test case '" << label << "' failed\n";
            ++testsFailed;
        }
        ++testsRan;
    }
    static void result() {
        if (testsFailed == 0) {
            std::cout << "All test cases passed\n";
        } else {
            std::cerr << '\n' << std::to_string(testsFailed) << " test cases failed\n";
        }
    }
};

