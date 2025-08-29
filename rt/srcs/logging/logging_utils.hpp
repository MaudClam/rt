#pragma once
#include <string_view>
#include <ostream>
#include <limits>

namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;

inline constexpr int    kHidden       = 0;  // means "hidden value"
inline constexpr int    kUnset        = -1; // means "not specified"
inline constexpr int    kIntMax       = std::numeric_limits<int>::max();
inline constexpr size_t kIntMaxAsSize = static_cast<size_t>(kIntMax);

constexpr int to_int_clamped(size_t n) noexcept {
    return n > kIntMaxAsSize ? kIntMax : static_cast<int>(n);
}

[[nodiscard]] constexpr
bool is_ascii(char c) noexcept {
    return static_cast<unsigned char>(c) < 0x80;
}

[[nodiscard]] constexpr
bool is_ascii_control(char c) noexcept {
    return static_cast<unsigned char>(c) < 0x20 || c == 0x7F;
}

[[nodiscard]] constexpr
bool is_printable_ascii(char c) noexcept {
    return is_ascii(c) && !is_ascii_control(c);
}

[[nodiscard]] inline
int ascii_prefix_length(sv_t sv) noexcept {
    int i = 0, size = to_int_clamped(sv.size());
    for (; i < size; ++i)
        if (!is_ascii(sv[i]))
            return i;
    return size;
}

[[nodiscard]] inline
int printable_ascii_prefix_length(sv_t sv) noexcept {
    int i = 0, size = to_int_clamped(sv.size());
    for (; i < size; ++i)
        if (!is_printable_ascii(sv[i]))
            return i;
    return size;
}

[[nodiscard]] inline
bool is_ascii_only(sv_t sv) noexcept {
    return ascii_prefix_length(sv) == to_int_clamped(sv.size());
}

[[nodiscard]] inline
bool is_printable_ascii_only(sv_t sv) noexcept {
    return printable_ascii_prefix_length(sv) == to_int_clamped(sv.size());
}

[[nodiscard]] constexpr
char normalize_ascii_char(char c, char normchar = '?') noexcept {
    return is_ascii_control(c) ? normchar : c;
}

template<traits::Ostreamable Os>
os_t& write_repeats(os_t& os, int num, const Os& val) noexcept {
    for(int i = 0; i < num; ++i)
        os << val;
    return os;
}

[[nodiscard]] inline int bresenham_y(int x, int dx, int dy) noexcept {
    if (x < 0 || dx <= 0 || dy <= 0) return 0;
    bool steep = dx < dy;
    if (steep) std::swap(dx, dy);
    int derror2 = dy * 2;
    int error2 = 0;
    for (int x0 = 0, y0 = 0; x0 < dx; ++x0) {
        if (!steep && x0 == x) return y0;
        if ( steep && y0 == x) return x0;
        error2 += derror2;
        if (error2 > dx) {
            ++y0;
            error2 -= dx * 2;
        }
    }
    return dy;
}

} // namespace logging
