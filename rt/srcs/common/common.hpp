#pragma once
#if NDEBUG
 constexpr bool debug_mode = false;
#else
 constexpr bool debug_mode = true;
#endif
#include <string_view>
#include <ostream>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <charconv>
#include <array>
#include <cassert>
#include <mutex>
#include <thread>
#include <memory>
#include "traits.hpp"


namespace common {

using sv_t   = std::string_view;
using os_t   = std::ostream;
using is_t   = std::istream;
using oss_t  = std::ostringstream;
using lock_t = std::lock_guard<std::mutex>;

inline auto stdout_mutex = std::mutex();
inline auto stderr_mutex = std::mutex();

enum class ExitCode : uint8_t {
    Success        = EXIT_SUCCESS,
    UnknownError   = EXIT_FAILURE,
    OutputFailure  = 2,
    CfgFileFailure = 3,
    CmdlineFailure = 4,
    LoggingFailure = 5,
};

[[noreturn]] inline void fatal_exit(ExitCode code) noexcept {
    std::exit(static_cast<int>(code));
}

[[noreturn]] inline void graceful_exit() noexcept {
    std::exit(static_cast<int>(ExitCode::Success));
}

template<typename... X>
constexpr auto stream_inserter() {
    return [](os_t& os, const X&... xs) noexcept {
        if constexpr (sizeof...(X) > 0)
            (os << ... << xs);
    };
}

enum class IoStatus : uint8_t { Ok, Failed };

template<typename F, class... Args>
inline IoStatus stream_locker(os_t& os, std::mutex& mtx, F&& writer,
              Args&&... args) noexcept requires (traits::OsWriter<F, Args...>) {
    bool ok = os.good();
    try {
        lock_t lk(mtx);
        std::invoke(std::forward<F>(writer), os, std::forward<Args>(args)...);
        os << std::flush;
        ok = os.good();
    } catch (...) {
        ok = false;
    }
    return ok ? IoStatus::Ok : IoStatus::Failed;
}

template<typename... Args>
inline IoStatus with_stdout(const Args&... args) noexcept {
    return stream_locker(std::cout,
                         stdout_mutex, stream_inserter<Args...>(), args...);
}

template<typename... Args>
inline void with_stderr(const Args&... args) noexcept {
    const auto st = stream_locker(std::cerr, stderr_mutex,
                                           stream_inserter<Args...>(), args...);
    if (st == IoStatus::Failed)
        fatal_exit(ExitCode::OutputFailure);
}

struct Return {
    sv_t status    = "";
    sv_t prompt    = "";
    bool fatal_err = false;

    [[nodiscard]] bool ok() const noexcept { return status.empty(); }

    [[nodiscard]] bool is_fatal() const noexcept { return fatal_err; }

    explicit operator bool() const noexcept { return !ok(); }

    os_t& write(os_t& os) const {
        os << status;
        if (!prompt.empty())
            os << " '" << prompt << '\'';
        return os;
    }
};

inline os_t& operator<<(os_t& os, const Return& ret) noexcept {
    return ret.write(os);
}

[[nodiscard]] inline Return ok() noexcept { return {}; }

[[nodiscard]] inline Return error(sv_t status, sv_t prompt = {},
                                              bool fatal_err = false) noexcept {
    if (status.empty()) {
        if (debug_mode) status = "CODING ERROR (missing error status)";
        else status = "Unknown error";
    }
    return {status, prompt, fatal_err};
}

inline ExitCode print_error(sv_t context, const Return& st,
         ExitCode exit_code = ExitCode::UnknownError, sv_t help = {}) noexcept {
    if (ok()) return ExitCode::Success;
    if (context.empty()) with_stderr(st, '\n');
    else with_stderr(context, ' ', st, '\n');
    if (!help.empty()) with_stdout(help);
    return exit_code;
}

template<typename T>
class ScopedOverride {
public:
    ScopedOverride(T& target, T value) noexcept
        : ref_(target), saved_(target) {
        ref_ = std::move(value);
    }

    ScopedOverride(const ScopedOverride&) = delete;
    ScopedOverride& operator=(const ScopedOverride&) = delete;

    ~ScopedOverride() noexcept { ref_ = std::move(saved_); }

private:
    T& ref_;
    T  saved_;
};

[[nodiscard]] inline constexpr
uint32_t hash_31(sv_t sv) noexcept {
    uint32_t h = 0;
    for (size_t i = 0; i < sv.size(); ++i)
        h = h * 31 + static_cast<unsigned char>(sv[i]);
    return h;
}

[[nodiscard]] inline constexpr
int parse_int(sv_t sv, int fallback = 0) noexcept {
    int value = fallback;
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
    return (ec == std::errc{}) ? value : fallback;
}

[[nodiscard]] inline constexpr
sv_t safe_substr(sv_t sv, size_t pos, size_t count = sv_t::npos) noexcept {
    return sv.substr(std::min(pos, sv.size()), count);
}

[[nodiscard]] inline constexpr bool is_space(char c) noexcept {
    return c == ' '  || c == '\t' || c == '\n' ||
           c == '\r' || c == '\v' || c == '\f';
}

[[nodiscard]] inline constexpr sv_t trim(sv_t sv) noexcept {
    size_t start = 0;
    size_t end = sv.size();
    while (start < end && is_space(sv[start])) ++start;
    while (end > start && is_space(sv[end - 1])) --end;
    return sv.substr(start, end - start);
}

[[nodiscard]] inline sv_t to_sv(int n) noexcept {
    static thread_local char buf[16];
    auto [ptr, ec] = std::to_chars(std::begin(buf), std::end(buf), n);
    return (ec == std::errc{}) ? sv_t{buf, static_cast<size_t>(ptr - buf)} : sv_t{};
}

template <size_t Сapacity = 512>
struct StrBuffer {
    static constexpr size_t capacity = Сapacity;
    using Buffer = std::array<char, capacity>;

    StrBuffer() = default;

    template <size_t N>
    constexpr StrBuffer(const char (&str)[N]) noexcept {
        static_assert(N > 1, "String literal cannot be empty");
        [[maybe_unused]] const Return r = set(sv_t{str, N - 1}, "StrBuffer literal");
        assert(r.ok() && "Invalid default literal for StrBuffer");
    }

    constexpr StrBuffer(sv_t sv) noexcept {
        assert(sv.size() > 0 && "String literal cannot be empty");
        [[maybe_unused]] const Return r = set(sv, "StrBuffer string_view");
        assert(r.ok() && "Invalid default literal for StrBuffer");
    }
    
    int get_capasity() { return static_cast<int>(capacity); }

    [[nodiscard]] sv_t view() const noexcept { return view_; }
    
    [[nodiscard]] constexpr Return set(sv_t sv, sv_t prompt) noexcept {
        if (sv.empty())
            return error("Empty string", prompt);
        if (sv.size() >= capacity)
            return error("String too long", prompt);
        if (sv.find('\0') != sv_t::npos)
            return error("String contains null character", prompt);
        std::memcpy(buffer_.data(), sv.data(), sv.size());
        buffer_[sv.size()] = '\0';
        view_ = { buffer_.data(), sv.size() };
        return ok();
    }

    [[nodiscard]] Return read_line(is_t& in, sv_t prompt) noexcept {
        view_ = {};
        char* data = buffer_.data();
        in.getline(data, capacity);
        std::streamsize count = in.gcount();
        if (count <= 0) {
            if (in.eof()) return error("Unexpected end of stream", prompt);
            return error("Failed to read from stream", prompt);
        }
        if (in.bad())
            return error("Fatal I/O error while reading", prompt);
        if (in.fail() && !in.eof())
            return error("Input line too long", prompt);
        size_t len = 0;
        while (len < static_cast<size_t>(count) && !is_space(data[len]) &&
               data[len] != '\0') ++len;
        view_ = sv_t{data, len};
        return ok();
    }
    
    [[nodiscard]]
    Return append_int(int value, sv_t prompt = "append_int") noexcept {
        size_t current_size = view_.size();
        char* begin = buffer_.data() + current_size;
        char* end   = buffer_.data() + capacity;
        auto [ptr, ec] = std::to_chars(begin, end, value);
        if (ec != std::errc{})
            return error("Integer conversion failed or buffer too small", prompt);
        size_t added = ptr - begin;
        if (current_size + added >= capacity)
            return error("Resulting string too long", prompt);
        *ptr = '\0';
        view_ = sv_t{buffer_.data(), current_size + added};
        return ok();
    }
    
private:
    Buffer buffer_{};
    sv_t   view_{};
};

template<typename BufferTag> [[nodiscard]]
oss_t& tl_buffer(bool restore = true) {
    static thread_local oss_t oss;
    oss.str("");
    oss.clear();
    if (restore) {
        oss.flags(std::ios_base::fmtflags{});
        oss.precision(6);
    }
    return oss;
}

} // namespace common
