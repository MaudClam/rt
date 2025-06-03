#pragma once
#include <string>
#include <sstream>
#include <ostream>
#include <initializer_list>

namespace ansi {

using str_t = std::string;
using os_t  = std::ostream;

inline constexpr str_t prefix       = "\033[";
inline constexpr char separator     = ';';
inline constexpr char suffix        = 'm';
inline constexpr str_t reset        = "\033[0m";
inline constexpr str_t clear_screen = "\033[H\033[2J";
inline constexpr str_t clear_line   = "\033[K";

enum class Color : uint8_t {
    Default = 39,
    Black = 30, Red, Green, Yellow, Blue, Magenta, Cyan, White,
    BrightBlack = 90, BrightRed, BrightGreen, BrightYellow,
    BrightBlue, BrightMagenta, BrightCyan, BrightWhite
};

enum class Background : uint8_t {
    Default = 49,
    Black = 40, Red, Green, Yellow, Blue, Magenta, Cyan, White,
    BrightBlack = 100, BrightRed, BrightGreen, BrightYellow,
    BrightBlue, BrightMagenta, BrightCyan, BrightWhite
};

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

struct Format {
    Color foreground = Color::Default;
    Background background = Background::Default;
    std::initializer_list<Style> styles = {};
    bool use_ansi = true;
};

inline os_t& ansi_code(os_t& os, const Format& fmt) {
    if (!fmt.use_ansi) return os;
    os << prefix << static_cast<int>(fmt.foreground);
    os << separator << static_cast<int>(fmt.background);
    for (Style s : fmt.styles)
        os << separator << static_cast<int>(s);
    return os << suffix;
}

inline os_t& sequence(os_t& os, const str_t& prefix_, int n, char suffix_) {
    return os << prefix_ << n << suffix_;
}

inline os_t& pad(os_t& os, int width, char pader = ' ') {
	for (int i = 0; i < width; ++i)
		os << pader;
	return os;
}

inline os_t& move_left(os_t& os, int n)  { return sequence(os, prefix, n, 'D'); }
inline os_t& move_right(os_t& os, int n) { return sequence(os, prefix, n, 'C'); }
inline os_t& move_up(os_t& os, int n)    { return sequence(os, prefix, n, 'A'); }
inline os_t& move_down(os_t& os, int n)  { return sequence(os, prefix, n, 'B'); }

inline os_t& clear_left(os_t& os, int n) {
	move_left(os, n);
    pad(os, n);
	move_left(os, n);
	return os;
}

} // namespace ansi
