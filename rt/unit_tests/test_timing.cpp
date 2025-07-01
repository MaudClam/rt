#include <iostream>
#include "../srcs/config.hpp"
#include "../srcs/logging/cell.hpp"
#include "../srcs/logging/timing.hpp"

// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_timing.cpp -o test_timing && ./test_timing

namespace rt { Config config; }

int main(int ac, char** av) {
    if (auto r = rt::config.parse_cmdline(ac, av); r) return r.write_error_if();
    using namespace logging;
    
    int N = 33;
    TimeCounter	tc(100, "ms");
    
    ProgressBar bar;
    bar.cycles = N;
    bar.width = 12;
    bar.hide = rt::config.test_param != 0 ;
//    std::string test_str = "Hello 🌍✨💥\xF0\x28\x8C\x28 World 🚀👾!";
    std::string test_str(reinterpret_cast<const char*>(u8"\xF0\x28\x8C\x28"));
//    bar.bar.prefix = test_str;
    bar.bar.mark = "💥";
    bar.bar.unmark = "🌍";
//    bar.bar.suffix = test_str;
//    bar.alt.prefix = test_str;
    bar.alt.mark = test_str;
//    bar.alt.suffix = test_str;
//    bar.percent.prefix = test_str;
    bar.percent.suffix = test_str;

    std::cout << "ProgressBar::Type::Bar " << std::flush;
    bar.type = ProgressBar::Type::Bar;
    bar.bar.prefix = rt::config.test_string.view().empty() ? bar.bar.prefix
                                                           : rt::config.test_string.view();
    for (int i = 0; i < N; ++i) {
        std::cout << bar;
        tc.run();
        if (i == N - 1) std::cout << bar << "OK\n";
    }

    std::cout << "ProgressBar::Type::Alternate " << std::flush;
    bar.type = ProgressBar::Type::Alternate;
    bar.alt.prefix = rt::config.test_string.view().empty() ? bar.alt.prefix
                                                           : rt::config.test_string.view();
    for (int i = 0; i < N; ++i) {
        std::cout << bar;
        tc.run();
        if (i == N - 1) std::cout << bar << "OK\n";
    }

    std::cout << "ProgressBar::Type::Percent " << std::flush;
    bar.type = ProgressBar::Type::Percent;
    bar.percent.prefix = rt::config.test_string.view().empty() ? bar.percent.prefix
                                                               : rt::config.test_string.view();
    for (int i = 0; i < N; ++i) {
        std::cout << bar;
        tc.run();
        if (i == N - 1) std::cout << bar << "OK\n";
    }

	return 0;
}

