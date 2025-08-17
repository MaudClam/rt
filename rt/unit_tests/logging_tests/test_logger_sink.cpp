#include "../../srcs/logging/logger_sink.hpp"


// Command lines fo testing
// g++ -std=c++2a -O2 -Wall -Wextra -Werror ../../srcs/globals.cpp test_logger_sink.cpp -o sink
// ./sink && ./sink && ./sink && ./sink && ./sink
int main(int ac, char** av) {
    using namespace logging;
    using namespace rt;
    config.init(ac, av);

    // Test 1: Stdout
    LoggerSink std_out(io::Output::Stdout);
    std_out.print("LoggerSink: Stdout test1\n");
    std_out.print("LoggerSink: Stdout test2\n");

    // Test 2: Stderr
    LoggerSink std_err(io::Output::Stderr);
    std_err.print("LoggerSink: Stderr test1\n");
    std_err.print("LoggerSink: Stderr test2\n");

    // Test 3: Buffer
    LoggerSink buff(io::Output::Buffer);
    std::cout << buff.print("LoggerSink: Buffer test1\n").view_buffer();
    std::cout << buff.print("LoggerSink: Buffer test2\n").view_buffer();
    std::cout << buff.clear_buffer().print("LoggerSink: Buffer test3\n").view_buffer();

    // Test 4: File output
    sv_t filename = "logs/sink.log";
    LoggerSink file(io::Output::File |
                    io::Output::CreateDirs |
                    io::Output::Indexing |
                    io::Output::TimeIndex,
                    filename);
    file.print("LoggerSink: File output test\n");
    std::cout << "Search for file '" << filename << "' indexed '_Y-m-d_H.M.S(i)'\n";

    return 0;
}
