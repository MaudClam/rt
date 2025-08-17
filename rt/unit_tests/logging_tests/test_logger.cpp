#include <thread>
#include <vector>
#include <atomic>
#include "../../srcs/logging/logger.hpp"

#define debug_msg(...) \
    do { if constexpr (debug_mode) logging::make_global_logger().debug(__VA_ARGS__); } while(0)

using namespace logging;

void test_lables(os_t& os = std::cout) {
    auto test_fmt = sticker_format(Level::Test);
    test_fmt.apply(os,
                   sticker(Level::Test, test_fmt.should_emoji()),
                   ' ', __func__, "():"
                   ) << std::endl;

    for (int i = 0, n = static_cast<int>(Level::count_); i < n; ++i) {
        auto lbl = static_cast<Level>(i);
        auto fmt = sticker_format(lbl);
        fmt.use_emoji = true;
        fmt.apply(os, sticker(lbl, fmt.should_emoji()));
        fmt.use_emoji = false;
        fmt.apply(os, sticker(lbl, fmt.should_emoji())) << std::endl;
    }
}

void test_debug_logger(os_t& os = std::cout) {
    auto test_fmt = sticker_format(Level::Test);
    test_fmt.apply(os,
                   sticker(Level::Test, test_fmt.should_emoji()),
                   ' ', __func__, "():"
                   ) << std::endl;
    auto fmt = Format{ .manip.boolalpha = true, };
    fmt.apply(os, "debug_mode: ", debug_mode) << std::endl;
    debug_msg("Hello World! (debug_mode == true only)", '\n');


}

//void test_debug_logger_multithreaded(os_t& os = std::cout) {
//    auto test_fmt = sticker_format(Level::Test);
//    test_fmt.apply(os,
//                   sticker(Level::Test, test_fmt.should_emoji()),
//                   ' ', __func__, "():"
//                   ) << std::endl;
//
//    constexpr int num_threads = 3;
//    constexpr int num_messages = 10;
//
//    std::vector<std::thread> threads;
//    std::atomic<int> success_count{0};
//
//    for (int i = 0; i < num_threads; ++i) {
//        threads.emplace_back([i, &success_count]() {
//            for (int j = 0; j < num_messages; ++j) {
//                debug_msg("Thread #", i, " message #", j, '\n');
//            }
//            if constexpr (debug_mode) {
//                set_log_warn(rt::config.log_warns, LogWarns::LoggingBufferFailed);
//                success_count++;
//            }
//        });
//    }
//
//    for (auto& t : threads)
//        t.join();
//
//    Format f;
//    f.apply(os, "Threads completed: ", success_count.load(), "/", num_threads) << std::endl;
//
//    // Выведем финальные флаги
//    const auto flags = rt::config.log_warns;
//    f.apply(os, "Global logger flags: ", static_cast<int>(flags)) << std::endl;
//}

// Command lines fo testing
// g++ -std=c++2a -O2 -DNDEBUG -Wall -Wextra -Werror ../../srcs/globals.cpp test_logger.cpp -o log && ./log
// g++ -std=c++2a -O2 -Wall -Wextra -Werror ../../srcs/globals.cpp test_logger.cpp -o log && ./log
int main(int ac, char** av) {
    using namespace rt;
    config.init(ac, av);

    std::cout << std::endl;
    test_lables();

    std::cout << std::endl;
    test_debug_logger();

//    std::cout << std::endl;
//    test_debug_logger_multithreaded();

    std::cout << std::endl;
    return 0;
}
