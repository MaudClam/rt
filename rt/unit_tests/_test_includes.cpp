//#include "../srcs/logging/logging_utils.hpp"
//
//// g++ -std=c++2a -O2 -Wall -Wextra -Werror _test_includes.cpp -o _test_includes && ./_test_includes
//int main() {
//    return 0;
//}

#include <sstream>
#include <string_view>
#include <iostream>

int main() {
    std::ostringstream oss;
    oss << "hello";
    std::string_view sv = oss.view(); // ✔ работает только в C++20+
    std::cout << sv << '\n';
}
