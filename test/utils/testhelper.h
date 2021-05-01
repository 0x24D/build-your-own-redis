#include <iostream>
unsigned int testsRan = 0;
unsigned int testsFailed = 0;
class TestHelper {
public:
    static void equals(const std::string& label, const auto& response, const auto& expectedResponse) {
        if (response != expectedResponse) {
            const auto printLambda = [](const auto& v) {
                std::cerr << v << " ";
            };
            std::cerr << "Test case '" << label << "' failed\n";
            std::for_each(response.begin(), response.end(), printLambda);
            std::cerr << "== ";
            std::for_each(expectedResponse.begin(), expectedResponse.end(), printLambda);
            std::cerr << '\n';
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

