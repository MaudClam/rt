#pragma once
#include <string_view>
#include <ostream>
#include <initializer_list>
#include <cstdint>          // uint8_t
#include "traits.hpp"

namespace ansi {

using sv_t = std::string_view;
using os_t = std::ostream;

inline constexpr char PARAM_SEP        = ';';
inline constexpr char SGR_SUFFIX       = 'm';
inline constexpr sv_t CSI_SEQ          = "\033[";
inline constexpr sv_t RESET_SEQ        = "\033[0m";
inline constexpr sv_t CLEAR_SCREEN_SEQ = "\033[H\033[2J";
inline constexpr sv_t CLEAR_LINE_SEQ   = "\033[K";

// ANSI foreground text colors
enum class Color : uint8_t {
    Default = 39,
    Black = 30, Red, Green, Yellow, Blue, Magenta, Cyan, White,
    BrightBlack = 90, BrightRed, BrightGreen, BrightYellow,
    BrightBlue, BrightMagenta, BrightCyan, BrightWhite
};

// ANSI background colors
enum class Background : uint8_t {
    Default = 49,
    Black = 40, Red, Green, Yellow, Blue, Magenta, Cyan, White,
    BrightBlack = 100, BrightRed, BrightGreen, BrightYellow,
    BrightBlue, BrightMagenta, BrightCyan, BrightWhite
};

// ANSI text styles
enum class Style : uint8_t {
    Bold = 1,
    Faint = 2,
    Italic = 3,
    Underline = 4,
    Inverse = 7,
    Hidden = 8,
    Strikethrough = 9,
    Reset = 0
};

// Combined ANSI format descriptor
struct Format {
    Color foreground = Color::Default;
    Background background = Background::Default;
    std::initializer_list<Style> styles = {};
    bool use_ansi = true;
};

// Outputs ANSI SGR sequence to stream
inline os_t& ansi_style(os_t& os, const Format& fmt) noexcept {
    if (!fmt.use_ansi) return os;
    os << CSI_SEQ << static_cast<int>(fmt.foreground);
    os << PARAM_SEP << static_cast<int>(fmt.background);
    for (Style s : fmt.styles)
        os << PARAM_SEP << static_cast<int>(s);
    return os << SGR_SUFFIX;
}

// ansi::reset_if — returns RESET_SEQ or empty if ANSI disabled
inline os_t& ansi_reset(os_t& os, bool use_ansi) noexcept {
    return os << (use_ansi ? RESET_SEQ : "");
}
inline os_t& ansi_reset(os_t& os, const Format& fmt) noexcept {
    return ansi_reset(os, fmt.use_ansi);
}

// Writes a short sequence of streamable values (for internal ANSI use only)
template<typename... Args>
inline os_t& sequence(os_t& os, const Args&... args)
    noexcept((noexcept(os << args) && ...)) {
    return (os << ... << args);
}

inline os_t& pad(os_t& os, int width, char pader = ' ') noexcept {
	for (int i = 0; i < width; ++i)
		os << pader;
	return os;
}

inline os_t& move_left(os_t& os, int n) noexcept  {
    n = std::max(n, 0);
    return sequence(os, CSI_SEQ, n, 'D');
}

inline os_t& move_right(os_t& os, int n) noexcept {
    n = std::max(n, 0);
    return sequence(os, CSI_SEQ, n, 'C');
}

inline os_t& move_up(os_t& os, int n) noexcept {
    n = std::max(n, 0);
    return sequence(os, CSI_SEQ, n, 'A');
}

inline os_t& move_down(os_t& os, int n) noexcept {
    n = std::max(n, 0);
    return sequence(os, CSI_SEQ, n, 'B');
}

inline os_t& clear_left(os_t& os, int n) noexcept {
    n = std::max(n, 0);
	move_left(os, n);
    pad(os, n);
	move_left(os, n);
	return os;
}

} // namespace ansi
