#include "../srcs/logging/terminal_width.hpp"
#include "../srcs/config.hpp"

// g++ -std=c++2a -O2 -Wall -Wextra -Werror ../srcs/globals.cpp _test_includes.cpp -o _test && ./_test
int main(int ac, char** av) {
    rt::config.init(ac, av);
    
    static_assert(traits::Ostreamable<int>);
    static_assert(traits::Ostreamable<const char*>);
    static_assert(traits::Ostreamable<std::string_view>);
    static_assert(traits::Ostreamable<decltype(std::boolalpha)>);
    static_assert(traits::Ostreamable<decltype(std::hex)>);
    static_assert(traits::Ostreamable<decltype(std::endl<char, std::char_traits<char>>)>);
    static_assert(traits::OsWriter<
        decltype([](std::ostream& os, int x){ os << x; }), int>);
    
    common::with_stdout("Hello World!\n_test_includes.cpp OK!\n");
    return 0;
}

