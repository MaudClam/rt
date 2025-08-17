#pragma once
#include <cstdint>


namespace logging {

using flags_t = uint8_t;

enum class LogWarn : flags_t {
    None                   = 0,
    LocaleActivationFailed = 1 << 0,
    Utf8NotInitialized     = 1 << 1,
    LoggingBufferFailed    = 1 << 2,
    LoggerWriteFailed      = 1 << 3,
    LoggerFileCloseFailed  = 1 << 4,
};

struct LogWarnDescriptor {
    const LogWarn value;
    const char*   message;
};

constexpr LogWarnDescriptor log_warn_descriptions[] = {
    {
        LogWarn::LocaleActivationFailed,
        "Failed to activate UTF-8 locale from environment. "
        "Unicode alignment may be incorrect."
    },
    {
        LogWarn::Utf8NotInitialized,
        "UTF-8 locale not initialized or unsupported. "
        "Unicode alignment may be incorrect."
    },
    {
        LogWarn::LoggingBufferFailed,
        "Failed to create logger buffer. "
        "Data alignment may be incorrect."
    },
    {
        LogWarn::LoggerWriteFailed,
        "LoggerSink write() failed. "
        "Output stream is null or unreachable."
    },
    {
        LogWarn::LoggerFileCloseFailed,
        "LoggerSink failed to close output file stream. "
        "Exception suppressed."
    },
};

[[nodiscard]] inline
bool has_log_warn(flags_t value, LogWarn warn) noexcept {
    return (value & static_cast<logging::flags_t>(warn)) != 0;
}

inline void set_log_warn(flags_t& value, LogWarn warn) noexcept
{
    if (!has_log_warn(value, warn))
        value |= static_cast<logging::flags_t>(warn);
}

} // namespace logging
