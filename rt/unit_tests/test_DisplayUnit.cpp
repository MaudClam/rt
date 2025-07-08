#include <iostream>
#include <thread>
#include "../srcs/logging/cell.hpp"

using namespace logging;

void test_display_unit(os_t& os, sv_t input) {
    os << "[test] input='" << input << "', length=" << input.size() << '\n';
    using namespace logging;
    oss_t buff;
    size_t offset = 0;
    int width = 0;
    DisplayUnit du;
    while (du.parse_debug(input, offset)) {
        du.write(buff, input);
        width += du.width;
        offset += du.length;
    }
    os << "[test] output='" << buff.view() << "', width=" << width << '\n';
}

namespace rt { Config config; }

//g++ -std=c++2a -O2 -Wall -Wextra -Werror test_DisplayUnit.cpp -o unit && ./unit
int main(int ac, char** av) {
    if (auto r = rt::config.parse_cmdline(ac, av); r) return r.write_error_if();
    
//    std::cout << "ABCDEFG";
    test_display_unit(std::cout, "123💇🏽‍♀️▶️ abc");
    test_display_unit(std::cout, "123🎓🦷🦷ZWNJ♀ abc");
    test_display_unit(std::cout, "\xF0\x28\x8C\x28");
    test_display_unit(std::cout, "🌍✨💥");
    return 0;
}
