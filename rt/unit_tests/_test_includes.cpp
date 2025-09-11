//#include <iostream>

// common
//#include "../srcs/common/buffer.hpp"
//#include "../srcs/common/common.hpp"
//#include "../srcs/common/common_utils.hpp"
//#include "../srcs/common/error_prefix.hpp"
//#include "../srcs/common/raw_io.hpp"
//#include "../srcs/common/return.hpp"
//#include "../srcs/common/traits.hpp"
//#include "../srcs/common/types.hpp"

#include "../srcs/config.hpp"

//#include "../srcs/logging/format.hpp"
//#include "../srcs/common/common.hpp"
//#include "../srcs/common/return.hpp"

// g++ -std=c++2a -O2 -Wall -Wextra -Werror ../srcs/globals.cpp _test_includes.cpp -o _test && ./_test
int main(int ac, char** av) {
    (void)ac; (void)av;
    rt::config.init(ac, av);
    rt::config.flush_log_warns(true);
    
    static_assert(traits::Ostreamable<int>);
    static_assert(traits::Ostreamable<const char*>);
    static_assert(traits::Ostreamable<std::string_view>);
    static_assert(traits::Ostreamable<decltype(std::boolalpha)>);
    static_assert(traits::Ostreamable<decltype(std::hex)>);
    static_assert(traits::Ostreamable<decltype(std::endl<char, std::char_traits<char>>)>);
    static_assert(traits::OsWriter<
        decltype([](std::ostream& os, int x){ os << x; }), int>);
    
    (void)common::with_stdout("Hello World!\n_test_includes.cpp OK!\n");
    return 0;
}

