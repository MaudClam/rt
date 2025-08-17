#pragma once
#include <string_view>
#include <ostream>
#include <sstream>
#include "traits.hpp"


namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;
using oss_t = std::ostringstream;

struct A {}; // BufferTag 'A'
struct B {}; // BufferTag 'B'

template<typename BufferTag> [[nodiscard]]
oss_t& get_buffer(bool restore = true) {
    static thread_local oss_t oss;
    oss.str(""); // clears the string contents, preserves capacity
    oss.clear(); // clears error flags (failbit, badbit, eofbit)
    if (restore) {
        // clears all formatting flags (fixed, boolalpha, etc.)
        oss.flags(std::ios_base::fmtflags{});
        // resets precision to default (like std::cout)
        oss.precision(6);
    }
    return oss;
}

[[nodiscard]] constexpr
bool is_ascii(char c) noexcept {
    return static_cast<unsigned char>(c) < 0x80;
}

[[nodiscard]] inline
int ascii_prefix_length(sv_t sv) noexcept {
    for (int i = 0, sz = static_cast<int>(sv.size()); i < sz; ++i)
        if (!is_ascii(sv[i]))
            return i;
    return static_cast<int>(sv.size());
}

[[nodiscard]] inline
bool is_ascii_only(sv_t sv) noexcept {
    return ascii_prefix_length(sv) == static_cast<int>(sv.size());
}

[[nodiscard]] constexpr
bool is_ascii_control(char c) noexcept {
    return static_cast<unsigned char>(c) < 0x20 || c == 0x7F;
}

[[nodiscard]] constexpr
char normalize_char(char c) noexcept {
    return is_ascii_control(c) ? '?' : c;
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

inline void terminal_clear_fallback() noexcept {
#if defined(_WIN32)
    std::system("cls");
#else
    std::system("clear");
#endif
}

} // namespace logging
