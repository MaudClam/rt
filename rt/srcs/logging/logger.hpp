#pragma once
#include "../config.hpp"
#include "logger_sink.hpp"

#define debug_msg(...) \
    do { if constexpr (debug_mode) logging::make_global_logger().debug(__VA_ARGS__); } while(0)
#define error_msg(...) do { \
    logging::make_global_logger().msg(logging::Level::Error __VA_OPT__(, ) __VA_ARGS__); } while(0)
#define info_msg(...) do { \
    logging::make_global_logger().msg(logging::Level::Info __VA_OPT__(, ) __VA_ARGS__); } while(0)
#define test_msg(...) do { \
    logging::make_global_logger().msg(logging::Level::Test __VA_OPT__(, ) __VA_ARGS__); } while(0)
#define warn_msg(...) do { \
    logging::make_global_logger().msg(logging::Level::Warn __VA_OPT__(, ) __VA_ARGS__); } while(0)
#define time_msg(...) do { \
    logging::make_global_logger().msg(logging::Level::Time __VA_OPT__(, ) __VA_ARGS__); } while(0)
#define ok_msg(...) do { \
    logging::make_global_logger().msg(logging::Level::Ok __VA_OPT__(, ) __VA_ARGS__); } while(0)

namespace logging {

enum class Level : uint8_t {
    Error,
    Debug,
    Info,
    Test,
    Warn,
    Time,
    Ok,
    count_
};

[[nodiscard]]
inline constexpr sv_t sticker(Level level, bool emoji = false) noexcept {
    using enum Level;
    switch (level) {
        case Error: return emoji ? "❌" : "[ERROR]";
        case Debug: return emoji ? "🪲" : "[DEBUG]";
        case Info:  return emoji ? "ℹ️" : "[INFO]";
        case Test:  return emoji ? "⚙️" : "[TEST]";
        case Warn:  return emoji ? "⚠️" : "[WARN]";
        case Time:  return emoji ? "⏱️" : "[TIME]";
        case Ok:    return emoji ? "✅" : "[OK]";
        default:    break;
    }
    return emoji ? "❓" : "[UNKNOWN]";
}

[[nodiscard]] inline Format& sticker_format(Level level) noexcept {
    using enum Level;
    switch (level) {
        case Error:
            static thread_local Format error_fmt{
                .ansi_format = {
                    .foreground = ansi::Color::BrightRed,
                    .styles     = { ansi::Style::Bold }
                }};
            return error_fmt;
        case Debug:
            static thread_local Format debug_fmt{
                .ansi_format = {
                    .foreground = ansi::Color::Cyan,
                    .styles     = { ansi::Style::Bold }
                }};
            return debug_fmt;
        case Info:
            static thread_local Format info_fmt{
                .ansi_format = {
                    .foreground = ansi::Color::BrightBlue,
                    .styles     = { ansi::Style::Bold }
                }};
            return info_fmt;
        case Test:
            static thread_local Format test_fmt{
                .ansi_format = {
                    .styles     = { ansi::Style::Bold }
                }};
            return test_fmt;
        case Warn:
            static thread_local Format warn_fmt{
                .ansi_format = {
                    .foreground = ansi::Color::Yellow,
                    .styles     = { ansi::Style::Bold }
                }};
            return warn_fmt;
        case Time:
            static thread_local Format time_fmt{
                .ansi_format = {
                    .foreground = ansi::Color::Green,
                    .styles     = { ansi::Style::Bold }
                }};
            return time_fmt;
        case Ok:
            static thread_local Format ok_fmt{
                .ansi_format = {
                    .foreground = ansi::Color::Green,
                    .styles     = { ansi::Style::Bold }
                }};
            return ok_fmt;
        default:
            break;
    }
    static thread_local Format default_fmt{};
    return default_fmt;
}

struct LoggerBase {
    LoggerBase& set_output(io::Output mode, sv_t raw_path = {},
                                         bool fatal_on_failure = false) noexcept
    {
        sink_.init(mode, raw_path, fatal_on_failure);
        return *this;
    }

    template<traits::Ostreamable... Args>
    LoggerBase& print(const Args&... args) noexcept {
        sink_.print(args...);
        return *this;
    }

    template<traits::Ostreamable... Args>
    LoggerBase& print(Format& fmt, const Args&... args) noexcept {
        sink_.print(fmt, args...);
        return *this;
    }

protected:
    LoggerSink  sink_{io::Output::Stdout};
    rt::Config* cfg_ = &rt::config;
};

struct GlobalLogger : LoggerBase {
    GlobalLogger() {
        sink_.init(cfg_->log_out, cfg_->log_file.view(), true);
    }

    template<traits::Ostreamable... Args>
    void msg(Level level, const Args&... args) noexcept {
        msg(level, sticker_format(level), args...);
    }

    template<traits::Ostreamable... Args>
    void msg(Level level, const Format& args_fmt, const Args&... args) noexcept {
        auto& sticker_fmt = sticker_format(level);
        std::lock_guard lock(sink_.mtx());
        print(sticker_fmt, sticker(level, sticker_fmt.should_emoji()));
        print(args_fmt, args...);
    }

    template<traits::Ostreamable... Args>
    void debug(const Args&... args) noexcept {
        if constexpr (debug_mode) {
            debug(sticker_format(Level::Debug), args...);
        } else {
            (void)sizeof...(args); // suppress unused warning in release builds
        }
    }

    template<traits::Ostreamable... Args>
    void debug(Format& args_fmt, const Args&... args) noexcept {
        if constexpr (debug_mode) {
            auto& sticker_fmt = sticker_format(Level::Debug);
            std::lock_guard lock(sink_.mtx());
            print(sticker_fmt, sticker(Level::Debug, sticker_fmt.should_emoji()));
            print(args_fmt, args...);
        } else {
            (void)args_fmt;
            (void)sizeof...(args); // suppress unused warning in release builds
        }
    }
};

// Returns a global debug logger.
// Initialized lazily and safely with static local storage.
// The returned object cannot be reconfigured.
[[nodiscard]] inline GlobalLogger& make_global_logger() noexcept {
    static thread_local GlobalLogger instance;
    return instance;
}

} // namespace logging
