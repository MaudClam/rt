#pragma once
#include "../config.hpp"
#include "logger_sink.hpp"

namespace logging {

using Flags     = rt::LoggerStatusFlags;
using LogConfig = rt::Config;

struct LoggerBase {
    LoggerBase& set_output(rt::Output mode, sv_t raw_path = {},
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

protected:
    LoggerSink sink_{rt::Output::Stdout};
    LogConfig& cfg_ = log_config;
};

struct DebugLogger : LoggerBase {
    DebugLogger() {
        sink_.init(rt::Output::Stderr, {}, false);
    }
    
    template<traits::Ostreamable... Args>
    void msg(const Args&... args) noexcept {
        if constexpr (debug_mode) {
            print(args...);
        } else {
            (void)sizeof...(args); // suppress unused warning in release builds
        }
    }
};

// Returns a global debug logger.
// Initialized lazily and safely with static local storage.
// The returned object is fixed to stderr and cannot be reconfigured.
inline DebugLogger& debug() {
    static thread_local DebugLogger instance;
    return instance;
}

} // namespace logging
