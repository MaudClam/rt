#include <iostream>
#include <vector>
#include "../srcs/logging/cell.hpp"
#include "../srcs/logging/timing.hpp"

using namespace logging;

void test_display_unit(os_t& os, sv_t input) {
    os << "\n[test] input= '" << input << "', length=" << input.size() << '\n';
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

    ScopedTimer timer(std::cout);

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
