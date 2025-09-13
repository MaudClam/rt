#pragma once
#include <cstddef>
#include <utility>
#include "types.hpp"


namespace common {

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

[[nodiscard]] inline size_t safe_strlen(const char* str) noexcept {
    size_t len = 0;
    while (str && str[len]) ++len;
    return len;
}

[[nodiscard]] inline sv_t to_sv(int n) noexcept {
    static thread_local char buf[16];
    auto [ptr, ec] = std::to_chars(std::begin(buf), std::end(buf), n);
    return (ec == std::errc{}) ? sv_t{buf, static_cast<size_t>(ptr - buf)} : sv_t{};
}

[[nodiscard]] inline sv_t to_sv(std::uint64_t n) noexcept {
    static thread_local char buf[20];
    auto [ptr, ec] = std::to_chars(std::begin(buf), std::end(buf), n);
    return (ec == std::errc{}) ? sv_t{buf, static_cast<size_t>(ptr - buf)} : sv_t{};
}

[[nodiscard]] inline sv_t as_sv(bool b) noexcept {
    return b ? "true" : "false";
}

[[nodiscard]] inline constexpr int parse_int(sv_t sv, int fback = 0) noexcept {
    int value = fback;
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
    return (ec == std::errc{}) ? value : fback;
}

[[nodiscard]] inline constexpr uint32_t hash_31(sv_t sv) noexcept {
    uint32_t h = 0;
    for (size_t i = 0; i < sv.size(); ++i)
        h = h * 31 + static_cast<unsigned char>(sv[i]);
    return h;
}

[[nodiscard]] inline constexpr
sv_t safe_substr(sv_t sv, size_t pos, size_t count = sv_t::npos) noexcept {
    return sv.substr(std::min(pos, sv.size()), count);
}

template<class T>
[[nodiscard]] inline T val(const std::atomic<T>& a) noexcept {
    return a.load(std::memory_order_relaxed);
}

template<typename T>
class ScopedOverride {
public:
    ScopedOverride(T& target, T value) noexcept : ref_(target), saved_(target) {
        ref_ = std::move(value);
    }
    ScopedOverride(const ScopedOverride&) = delete;
    ScopedOverride& operator=(const ScopedOverride&) = delete;
    ~ScopedOverride() noexcept { ref_ = std::move(saved_); }
private:
    T& ref_;
    T  saved_;
};

}//namespace common
