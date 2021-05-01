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
    static void startsWith(const std::string& label, const auto& response, const auto& expectedStart) {
        if (!response.starts_with(expectedStart)) {
            const auto printLambda = [](const auto& v) {
                std::cerr << v << " ";
            };
            std::cerr << "Test case '" << label << "' failed\n";
            std::for_each(response.begin(), response.end(), printLambda);
            std::cerr << "startsWith ";
            std::for_each(expectedStart.begin(), expectedStart.end(), printLambda);
            std::cerr << '\n';
            ++testsFailed;
        }
        ++testsRan;
    }
    static int result(const std::string& label) {
        if (testsFailed == 0) {
            std::cout << label << ": all test cases passed\n";
            return 0;
        } else {
            std::cerr << label << ": " << std::to_string(testsFailed) << " test case";
            if (testsFailed != 1) {
                std::cerr << 's';
            }
            std::cerr <<" failed\n";
            return 1;
        }
    }
};

