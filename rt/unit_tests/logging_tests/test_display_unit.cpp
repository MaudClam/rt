#include "../../srcs/config.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include "../../srcs/logging/terminal_width.hpp"

using namespace logging;
using oss_t = std::ostringstream;

void test_display_unit(os_t& os, sv_t input) {
    os << "\n[test] input= '" << input << "', length=" << input.size() << '\n';
    oss_t buff;
    size_t offset = 0;
    int width = 0;
    DisplayUnit du;
    while (du.parse_debug(input, offset, os)) {
        du.write(buff, input);
        width += du.width;
        offset += du.length;
    }
    os << "[test] output='" << buff.view() << "', width=" << width << '\n';
}

//g++ -std=c++2a -O2 -Wall -Wextra -Werror ../../srcs/globals.cpp test_display_unit.cpp -o unit && ./unit
int main(int ac, char** av) {
    rt::config.init(ac, av);

    std::vector<sv_t> sv;
    sv.push_back("123\tПапа");
    sv.push_back("📦🏽");                // Bad case, wrong width.
    sv.push_back("✅🏽");                // Bad case, wrong width.
    sv.push_back("🏽👨‍👩‍👧‍👦 🏽💇🏻‍♀️");         // width=9
    sv.push_back("123🎓🦷🦷ZWNJ♀ abc"); // width=18
    sv.push_back("\xF0\x28\x8C\x28");    // Incorrect codepoints
    sv.push_back("▶️");
    sv.push_back("🇷🇺");
    sv.push_back("🇷");
    sv.push_back("🇷💡🇺");
    sv.push_back("🇷🇺🇸");
    sv.push_back("🇩🇪 🇫🇷");
    sv.push_back("こんにちは");
    sv.push_back("🏳️‍🌈");
    sv.push_back("🧑🏿‍🦰");
    sv.push_back("👨‍❤️‍💋‍👨");
    sv.push_back("👩‍👧‍👦");
    sv.push_back("🏳️‍⚧️");
    for (sv_t s : sv)
        test_display_unit(std::cout, s);
    return 0;
}
