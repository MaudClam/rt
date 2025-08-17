#include "../../srcs/config.hpp"
#include <iostream>
#include <thread>
#include "../../srcs/logging/format.hpp"

void time_delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Command lines fo testing
// g++ -std=c++2a -O2 -Wall -Wextra -Werror ../../srcs/globals.cpp test_progress_bar.cpp -o pbar
// ./pbar --test-param=1
// ./pbar --test-param=0
// ./pbar --test-param=0 --no-tty
// ./pbar --test-param=0 --no-ansi
// ./pbar --test-string=' 🇱🇹Хеллоюшки🇺🇸Hello 🌍✨💥🇺🇸\xF0\x28💇🏻‍♀️\x8C\x28🇱🇹 World! 🚀🇺🇸👾🇱🇹 ' --test-param=1

int main(int ac, char** av) {
    rt::config.init(ac, av);
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
                     "👈 " : rt::config.test_string.view();
    bar.bar.prefix = "👉";
    bar.bar.mark   = "💥";
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

