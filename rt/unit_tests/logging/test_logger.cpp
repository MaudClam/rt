#include "../../srcs/logging/logger.hpp"

#define debug_msg(...) \
    do { if constexpr (debug_mode) logging::debug().msg(__VA_ARGS__); } while(0)

namespace rt { thread_local Config config; }

// Command lines fo testing
// g++ -std=c++2a -O2 -DNDEBUG -Wall -Wextra -Werror test_logger.cpp -o log && ./log
// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_logger.cpp -o log && ./log
int main(int ac, char** av) {
//    using namespace logging;
    using namespace rt;
    if (auto r = config.parse_cmdline(ac, av); r) return r.write_error_if();

    std::cout << "debug_mode: " << std::boolalpha << debug_mode << '\n';
    debug_msg("Hello World! (debug_mode == true only)", '\n');
    
    
    
    return 0;
}
