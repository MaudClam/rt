#pragma once
#include <string_view>
#include <cstdint>
#include <ostream>


namespace logging {

using sv_t    = std::string_view;
using os_t    = std::ostream;
using flags_t = uint8_t;

enum class LogWarns : flags_t {
    None                   = 0,
    LocaleActivationFailed = 1 << 0,
    Utf8NotInitialized     = 1 << 1,
    LoggingBufferFailed    = 1 << 2,
    LoggerWriteFailed      = 1 << 3,
    LoggerFileCloseFailed  = 1 << 4,
};

[[nodiscard]]
inline constexpr LogWarns operator|(LogWarns a, LogWarns b) noexcept {
    return static_cast<LogWarns>(
        static_cast<flags_t>(a) | static_cast<flags_t>(b));
}

inline constexpr LogWarns& operator|=(LogWarns& a, LogWarns b) noexcept {
    return a = a | b;
}

[[nodiscard]]
inline constexpr LogWarns operator&(LogWarns a, LogWarns b) noexcept {
    return static_cast<LogWarns>(
        static_cast<flags_t>(a) & static_cast<flags_t>(b));
}

[[nodiscard]]
inline constexpr bool has_log_warn(LogWarns value, LogWarns warn) noexcept {
    return (value & warn) != LogWarns::None;
}

inline void set_log_warn(LogWarns& value, LogWarns warn) noexcept {
    if (!has_log_warn(value, warn))
        value |= warn;
}

struct LogWarnDescriptor {
    const LogWarns value;
    const sv_t     message;
};

constexpr LogWarnDescriptor log_warn_descriptions[] = {
    {
        LogWarns::LocaleActivationFailed,
        "Failed to activate UTF-8 locale from environment. "
        "Unicode alignment may be incorrect."
    },
    {
        LogWarns::Utf8NotInitialized,
        "UTF-8 locale not initialized or unsupported. "
        "Unicode alignment may be incorrect."
    },
    {
        LogWarns::LoggingBufferFailed,
        "Failed to create logger buffer. "
        "Data alignment may be incorrect."
    },
    {
        LogWarns::LoggerWriteFailed,
        "LoggerSink write() failed. "
        "Output stream is null or unreachable."
    },
    {
        LogWarns::LoggerFileCloseFailed,
        "LoggerSink failed to close output file stream. "
        "Exception suppressed."
    },
};

inline os_t& write_log_warns(os_t& os, LogWarns log_warns) noexcept {
    for (const auto& entry : log_warn_descriptions)
        if ((log_warns & entry.value) != LogWarns::None)
            os << "[LOG_WARN] " << entry.message << '\n';
    return os;
}

inline os_t& operator<<(os_t& os, LogWarns log_warns) {
    return write_log_warns(os, log_warns);
}

} // namespace logging
