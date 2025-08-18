#pragma once
#include <string_view>
#include <cstdint>
#include <atomic>
#include <ostream>


namespace logging {

using sv_t    = std::string_view;
using os_t    = std::ostream;
using flags_t = uint32_t;

enum class Warn : flags_t {
    None                   = 0,
    LocaleActivationFailed = 1u << 0,
    Utf8NotInitialized     = 1u << 1,
    LoggingBufferFailed    = 1u << 2,
    LoggerWriteFailed      = 1u << 3,
    LoggerFileCloseFailed  = 1u << 4,
};

struct LogWarnDescriptor {
    const Warn value;
    const sv_t message;
};

constexpr LogWarnDescriptor log_warn_descriptions[] = {
    {
        Warn::LocaleActivationFailed,
        "Failed to activate UTF-8 locale from environment. "
        "Unicode alignment may be incorrect."
    },
    {
        Warn::Utf8NotInitialized,
        "UTF-8 locale not initialized or unsupported. "
        "Unicode alignment may be incorrect."
    },
    {
        Warn::LoggingBufferFailed,
        "Failed to create logger buffer. "
        "Data alignment may be incorrect."
    },
    {
        Warn::LoggerWriteFailed,
        "LoggerSink write() failed. "
        "Output stream is null or unreachable."
    },
    {
        Warn::LoggerFileCloseFailed,
        "LoggerSink failed to close output file stream. "
        "Exception suppressed."
    },
};

inline constexpr Warn operator|(Warn a, Warn b) noexcept {
    return static_cast<Warn>(static_cast<flags_t>(a) | static_cast<flags_t>(b));
}

inline constexpr bool any(Warn  w) noexcept { return w != Warn::None; }

class LogWarns {
public:
    LogWarns() noexcept : bits_(0) {}

    LogWarns& set(Warn w) noexcept {
        bits_.fetch_or(static_cast<flags_t>(w), std::memory_order_relaxed);
        return *this;
    }

    LogWarns& clear(Warn w) noexcept {
        bits_.fetch_and(~static_cast<flags_t>(w), std::memory_order_relaxed);
        return *this;
    }

    LogWarns& reset() noexcept {
        bits_.store(0, std::memory_order_relaxed);
        return *this;
    }

    LogWarns& merge(const LogWarns& other) noexcept {
        bits_.fetch_or(static_cast<flags_t>(other.snapshot()), std::memory_order_relaxed);
        return *this;
    }

    [[nodiscard]] bool test(Warn w) const noexcept {
        return (bits_.load(std::memory_order_relaxed) & static_cast<flags_t>(w)) != 0;
    }

    [[nodiscard]] Warn snapshot() const noexcept {
        return static_cast<Warn>(bits_.load(std::memory_order_relaxed));
    }

    [[nodiscard]] flags_t bits() const noexcept {
        return static_cast<flags_t>(snapshot());
    }

    os_t& write(os_t& os) const noexcept {
        for (const auto& entry : log_warn_descriptions)
            if (test(entry.value))
                os << "[LOG_WARN] " << entry.message << '\n';
        return os;
    }

private:
    std::atomic<flags_t> bits_;
};

inline os_t& operator<<(os_t& os, const LogWarns& log_warns) noexcept {
    return log_warns.write(os);
}

} // namespace logging
