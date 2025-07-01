#include <iostream>
#include <thread>
#include "../srcs/config.hpp"
#include "../srcs/logging/cell.hpp"

void time_delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Command lins fo testing
// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_ProgressBar.cpp -o test_pb && ./test_pb --test-param=1
// ./test_pb --test-param=0
// ./test_pb --test-param=0 --no-tty
// ./test_pb --test-param=0 --no-ansi
// ./test_pb --test-param=1 --test-string=' 🇱🇹Хеллоюшки🇺🇸Hello 🌍✨💥🇺🇸\xF0\x28\x8C\x28🇱🇹 World! 🚀🇺🇸👾🇱🇹 '

namespace rt { Config config; }

int main(int ac, char** av) {
    if (auto r = rt::config.parse_cmdline(ac, av); r) return r.write_error_if();
    using namespace logging;
    
    const int  cycles   = 33;
    const int  delay_ms = 150;
    
    ProgressBar bar;
    bar.cycles = cycles;
    bar.width = 10;
    bar.hide = rt::config.test_param != 0 ;

    std::cout << "ProgressBar::Type::Bar " << std::flush;
    bar.type = ProgressBar::Type::Bar;
    bar.bar.suffix = rt::config.test_string.view().empty() ?
                     "👈 " :
                     rt::config.test_string.view();
    bar.bar.prefix = "👉";
    bar.bar.mark = "💥";
    bar.bar.unmark = "🌍";
    for (int i = 0; i < cycles; ++i) {
        std::cout << bar;
        time_delay(delay_ms);
        if (i == cycles - 1) std::cout << bar << "OK\n";
    }

    std::cout << "ProgressBar::Type::Alternate " << std::flush;
    bar.type = ProgressBar::Type::Alternate;
    bar.alt.prefix = rt::config.test_string.view().empty() ?
                     bar.alt.suffix :
                     rt::config.test_string.view();
    for (int i = 0; i < cycles; ++i) {
        std::cout << bar;
        time_delay(delay_ms);
        if (i == cycles - 1) std::cout << bar << "OK\n";
    }

    std::cout << "ProgressBar::Type::Percent " << std::flush;
    bar.type = ProgressBar::Type::Percent;
    bar.percent.prefix = rt::config.test_string.view().empty() ?
                         bar.percent.prefix :
                         rt::config.test_string.view();
    for (int i = 0; i < cycles; ++i) {
        std::cout << bar;
        time_delay(delay_ms);
        if (i == cycles - 1) std::cout << bar << "OK\n";
    }

	return 0;
}

