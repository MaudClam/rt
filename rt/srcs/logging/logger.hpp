#pragma once
#include "../config.hpp"
#include "format.hpp"
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
    static thread_local Format default_fmt{
        .control.end_policy = Format::Control::EndPolicy::Pad,
    };
    using enum Level;
    switch (level) {
        case Error:
            static thread_local Format error_fmt{ default_fmt };
            error_fmt.ansi_format = {
                .foreground = ansi::Color::BrightRed,
                .styles     = { ansi::Style::Bold }
            };
            return error_fmt;
        case Debug:
            static thread_local Format debug_fmt{ default_fmt };
            debug_fmt.ansi_format = {
                .foreground = ansi::Color::Cyan,
                .styles     = { ansi::Style::Bold }
            };
            return debug_fmt;
        case Info:
            static thread_local Format info_fmt{ default_fmt };
            info_fmt.ansi_format = {
                .foreground = ansi::Color::BrightBlue,
                .styles     = { ansi::Style::Bold }
            };
            return info_fmt;
        case Test:
            static thread_local Format test_fmt{ default_fmt };
            test_fmt.ansi_format = {
                .styles     = { ansi::Style::Bold }
            };
            return test_fmt;
        case Warn:
            static thread_local Format warn_fmt{ default_fmt };
            warn_fmt.ansi_format = {
                .foreground = ansi::Color::Yellow,
                .styles     = { ansi::Style::Bold }
            };
            return warn_fmt;
        case Time:
            static thread_local Format time_fmt{ default_fmt };
            time_fmt.ansi_format = {
                .foreground = ansi::Color::Green,
                .styles     = { ansi::Style::Bold }
            };
            return time_fmt;
        case Ok:
            static thread_local Format ok_fmt{ default_fmt };
            ok_fmt.ansi_format = {
                .foreground = ansi::Color::Green,
                .styles     = { ansi::Style::Bold }
            };
            return ok_fmt;
        default:
            break;
    }
    return default_fmt;
}

struct LoggerBase {
    LoggerBase& set_output(io::Output mode, sv_t raw_path = {},
                                       bool fatal_on_failure = false) noexcept {
        sink_.init(mode, raw_path, fatal_on_failure);
        return *this;
    }

protected:
    LoggerSink sink_{io::Output::Stdout};

    os_t& out() const noexcept { return sink_.out(); }

    std::mutex& mtx() const noexcept { return sink_.mtx(); }

    io::Output mode() const noexcept { return sink_.get_mode(); }

    void on_write_failure(bool failed) {
        sink_.on_write_failure(failed);
    }
};

struct GlobalLogger : LoggerBase {
    GlobalLogger() noexcept { sink_.init_default(); }

    template<traits::Ostreamable... Args>
    void debug(const Args&... args) noexcept {
        if constexpr (debug_mode) msg(Level::Debug, args...);
        else (void)sizeof...(args);
    }

    template<traits::Ostreamable... Args>
    void msg(Level level, const Args&... args) noexcept {
        msg(level, sticker_format(level), args...);
    }

    template<traits::Ostreamable... Args>
    void msg(Level lvl, const Format& args_fmt, const Args&... args) noexcept {
        bool ok = out().good();
        if (ok) {
            try {
                auto& sticker_fmt     = sticker_format(lvl);
                const auto sticker_sv = sticker(lvl, sticker_fmt.should_emoji());
                lock_t lk(mtx());
                sticker_fmt.apply(mode(), out(), sticker_sv);
                if constexpr (sizeof...(Args) > 0)
                    args_fmt.apply(mode(), out(), args...);
                out() << std::flush;
                ok = out().good();
            } catch (...) {
                ok = false;
            }
        }
        on_write_failure(!ok);
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
