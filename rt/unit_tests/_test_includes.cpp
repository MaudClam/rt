//#include "../srcs/logging/logger.hpp"
#include "../srcs/config.hpp"

// g++ -std=c++2a -O2 -Wall -Wextra -Werror ../srcs/globals.cpp _test_includes.cpp -o _test && ./_test
int main(int ac, char** av) {
    rt::config.init(ac, av);
    return 0;
}

