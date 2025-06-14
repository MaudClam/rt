#pragma once
#include <string_view>
#include <cwchar>       // for mbrtowc, mbstate_t, wchar_t
#include <cstdlib>      // for std::system
#include <algorithm>    // for std::max
#include <optional>
#include <cstdint>
#include "traits.hpp"
#include "ansi_escape_codes.hpp"
#include "../config.hpp"


namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;

using Flags             = rt::LoggerStatusFlags;
using LogConfig         = rt::Config;
inline auto& log_config = rt::config;

constexpr int hidden = 0; // means "hidden value"
constexpr int unset = -1; // means "not specified"

[[nodiscard]] constexpr
bool is_ascii(char c) noexcept {
    return static_cast<unsigned char>(c) < 0x80;
}

[[nodiscard]] inline
int ascii_prefix_length(sv_t sv) noexcept {
    for (int i = 0, n = static_cast<int>(sv.size()); i < n; ++i)
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
    return is_ascii_control(c) ? ' ' : c;
}

struct Codepoint {
    wchar_t value = L'?';
    int     width = 1;
    size_t  len   = 1;

    [[nodiscard]] constexpr
    bool is_ascii_control() const noexcept {
        return len == 1 &&
               ::logging::is_ascii_control(static_cast<char>(value));
    }
    
    [[nodiscard]] constexpr
    bool is_invalid() const noexcept {
        return len ==  0 ||
               len == static_cast<size_t>(-1) ||
               len == static_cast<size_t>(-2) ||
               width < 0;
    }
    
    Codepoint& replace_invalid_if() noexcept {
        if (is_invalid())
        value = L'?'; width = 1; len = 1;
        return *this;
    }
};

[[nodiscard]] inline std::optional<Codepoint>
next_codepoint(sv_t sv, size_t offset, mbstate_t& state) noexcept {
    Codepoint cp{};
    if (offset >= sv.size())
        return std::nullopt;
    const char* ptr = sv.data() + offset;
    size_t remaining = sv.size() - offset;
    cp.len = mbrtowc(&cp.value, ptr, remaining, &state);
    if (!cp.is_invalid())
        cp.width = wcwidth(cp.value);
    return cp.replace_invalid_if();
}

[[nodiscard]] inline
int utf8_terminal_width(sv_t sv) noexcept {
    mbstate_t state{};
    int width = 0;
    size_t i = 0;
    while (i < sv.size()) {
        auto cp = next_codepoint(sv, i, state);
        if (!cp) break;
        width += cp->width;
        i += cp->len;
    }
    return width;
}

[[nodiscard]] inline
int terminal_width(sv_t sv, LogConfig& conf = log_config) {
    if (conf.utf8_inited)
        return utf8_terminal_width(sv);
    conf.set_logger_flag_if(Flags::Utf8NotInitialized);
    return static_cast<int>(sv.size());
}

struct TruncateFormat {
    int&       term_width;
    int        max_width;
    int        dots;
    LogConfig& config;

    TruncateFormat(int& term_width,
                   int max_width = unset,
                   int dots = 3,
                   LogConfig& config = log_config) noexcept
    :
        term_width(term_width),
        max_width(max_width),
        dots(dots),
        config(config)
    {}

    [[nodiscard]] int limit(int content_width) noexcept {
        if (max_width < 0 || max_width >= content_width) {
            dots = 0;
            return std::numeric_limits<int>::max();
        }
        dots = std::clamp(dots, 0, max_width);
        return max_width - dots;
    }
};

[[nodiscard]] inline
TruncateFormat truncate(int& width, int max_width = unset, int dots = 3,
                        LogConfig& conf = log_config) noexcept {
    return TruncateFormat{ width, max_width, dots, conf };
}

inline
os_t& write_ascii_truncate_if(os_t& os, sv_t sv, TruncateFormat& fmt) noexcept {
    fmt.term_width = 0;
    if (fmt.max_width == hidden) return os;
    const int sv_size = static_cast<int>(sv.size());
    const int limit = fmt.limit(sv_size);
    for (int i = 0; i < sv_size && fmt.term_width < limit; ++i, ++fmt.term_width)
        os << normalize_char(sv[i]);
    for (int i = 0; i < fmt.dots; ++i, ++fmt.term_width)
        os << '.';
    return os;
}

inline os_t& write_truncate_if(os_t& os, sv_t sv, TruncateFormat& fmt)
noexcept {
    fmt.term_width = 0;
    if (fmt.max_width == hidden) return os;

    if (!fmt.config.utf8_inited) {
        if (fmt.max_width > fmt.dots &&
            ascii_prefix_length(sv) < fmt.limit(static_cast<int>(sv.size())))
        {
            fmt.max_width = unset;
            fmt.config.set_logger_flag_if(Flags::Utf8NotInitialized);
        }
        return write_ascii_truncate_if(os, sv, fmt);
    }
    const int limit = fmt.max_width < 0 ? fmt.limit(0)
                                        : fmt.limit(utf8_terminal_width(sv));
    mbstate_t state{};
    size_t i = 0;
    while (i < sv.size() && fmt.term_width < limit) {
        auto cp = next_codepoint(sv, i, state);
        if (!cp || cp->len < 1) break;
        if (cp->is_ascii_control()) {
            os << ' ';
            fmt.term_width += 1;
        } else {
            os.write(sv.data() + i, cp->len);
            fmt.term_width += cp->width;
        }
        i += cp->len;
    }
    for (int i = 0; i < fmt.dots; ++i, ++fmt.term_width)
        os << '.';
    return os;
}

inline void terminal_clear_fallback() noexcept {
#if defined(_WIN32)
    std::system("cls");
#else
    std::system("clear");
#endif
}

} // namespace logging
