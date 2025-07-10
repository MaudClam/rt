#include <iostream>
#include <thread>
#include "../srcs/logging/cell.hpp"

using namespace logging;

void test_display_unit(os_t& os, sv_t input) {
    os << "[test] input= '" << input << "', length=" << input.size() << '\n';
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
    os << "[test] output='" << buff.view() << "' width=" << width << '\n';
}

namespace rt { Config config; }

//g++ -std=c++2a -O2 -Wall -Wextra -Werror test_DisplayUnit.cpp -o unit && ./unit
int main(int ac, char** av) {
    if (auto r = rt::config.parse_cmdline(ac, av); r) return r.write_error_if();
    
//    sv_t sv = "📦🏽";// Bad case, wrong width.
//    sv_t sv = "✅🏽";// Bad case, wrong width.
//    sv_t sv = "🏽👨‍👩‍👧‍👦 🏽💇🏻‍♀️";// width=9
//    sv_t sv = "123🎓🦷🦷ZWNJ♀ abc";// width=18
    sv_t sv = "\xF0\x28\x8C\x28";// Incorrect codepoints
    test_display_unit(std::cout, sv);
    return 0;
}
