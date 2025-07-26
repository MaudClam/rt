#include "../../srcs/logging/logger_sink.hpp"


namespace rt { thread_local Config config; }

// Command lines fo testing
// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_logger_sink.cpp -o sink && ./sink
int main(int ac, char** av) {
    using namespace logging;
    using namespace rt;
    if (auto r = config.parse_cmdline(ac, av); r) return r.write_error_if();

    // Test 1: Stdout
    LoggerSink std_out(Output::Stdout);
    std_out.print("LoggerSink: Stdout test1\n");
    std_out.print("LoggerSink: Stdout test2\n");

    // Test 2: Stderr
    LoggerSink std_err(Output::Stderr);
    std_err.print("LoggerSink: Stderr test1\n");
    std_err.print("LoggerSink: Stderr test2\n");

    // Test 3: Buffer
    LoggerSink buff(Output::Buffer);
    std::cout << buff.print("LoggerSink: Buffer test1\n").view_buffer();
    std::cout << buff.print("LoggerSink: Buffer test2\n").view_buffer();
    std::cout << buff.clear_buffer().print("LoggerSink: Buffer test3\n").view_buffer();

    // Test 4: File output
    sv_t filename = "sink.log";
    LoggerSink file(Output::File, filename);
    file.print("LoggerSink: File output test\n");
    std::cout << "Search for file '" << filename << "'\n";

    return 0;
}
