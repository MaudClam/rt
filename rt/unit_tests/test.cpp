#include "../srcs/logging/logger_sink.hpp"


namespace rt { Config config; }

// Command lines fo testing
// g++ -std=c++2a -O2 -Wall -Wextra -Werror test.cpp -o test && ./test
int main(int ac, char** av) {
    using namespace logging;
    using namespace rt;
    if (auto r = config.parse_cmdline(ac, av); r) return r.write_error_if();

    LoggerSink sink(Output::Stdout);
    sink.write("LoggerSink is alive!", '\n');
    
    // Test 1: Stdout
    LoggerSink sink_stdout(Output::Stdout);
    sink_stdout.write("LoggerSink: Stdout test\n");

    // Test 2: Stderr
    LoggerSink sink_stderr(Output::Stderr);
    sink_stderr.write("LoggerSink: Stderr test\n");

    // Test 3: Buffer
    LoggerSink sink_buffer(Output::Buffer);
    sink_buffer.write("LoggerSink: Buffer test\n");

    // Optionally: посмотреть содержимое буфера
    // std::cerr << "\n[Buffered output]:\n" << get_buffer<LoggerSink>().str();

    // Test 4: File output (обязательно проверь, что путь существует)
    LoggerSink sink_file(Output::File, config.log_file.view());
    sink_file.write("LoggerSink: File output test3\n");

    return 0;
}
