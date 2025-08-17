#pragma once
#include "ansi_escape_codes.hpp"

namespace ansi {

template <typename T>
struct NamedEnum {
    T    value;
    sv_t name;
};

constexpr NamedEnum<Color> enumColors[] = {
    { Color::Default,       "Default" },
    { Color::Black,         "Black" },
    { Color::Red,           "Red" },
    { Color::Green,         "Green" },
    { Color::Yellow,        "Yellow" },
    { Color::Blue,          "Blue" },
    { Color::Magenta,       "Magenta" },
    { Color::Cyan,          "Cyan" },
    { Color::White,         "White" },
    { Color::BrightBlack,   "BrightBlack" },
    { Color::BrightRed,     "BrightRed" },
    { Color::BrightGreen,   "BrightGreen" },
    { Color::BrightYellow,  "BrightYellow" },
    { Color::BrightBlue,    "BrightBlue" },
    { Color::BrightMagenta, "BrightMagenta" },
    { Color::BrightCyan,    "BrightCyan" },
    { Color::BrightWhite,   "BrightWhite" }
};

constexpr NamedEnum<Background> enumBackgrounds[] = {
    { Background::Default,       "Default" },
    { Background::Black,         "Black" },
    { Background::Red,           "Red" },
    { Background::Green,         "Green" },
    { Background::Yellow,        "Yellow" },
    { Background::Blue,          "Blue" },
    { Background::Magenta,       "Magenta" },
    { Background::Cyan,          "Cyan" },
    { Background::White,         "White" },
    { Background::BrightBlack,   "BrightBlack" },
    { Background::BrightRed,     "BrightRed" },
    { Background::BrightGreen,   "BrightGreen" },
    { Background::BrightYellow,  "BrightYellow" },
    { Background::BrightBlue,    "BrightBlue" },
    { Background::BrightMagenta, "BrightMagenta" },
    { Background::BrightCyan,    "BrightCyan" },
    { Background::BrightWhite,   "BrightWhite" }
};

constexpr NamedEnum<Style> enumStyles[] = {
    { Style::Reset,         "Reset" },
    { Style::Bold,          "Bold" },
    { Style::Faint,         "Faint" },
    { Style::Italic,        "Italic" },
    { Style::Underline,     "Underline" },
    { Style::Blink,         "Blink" },
    { Style::BlinkRapid,    "BlinkRapid" },
    { Style::Inverse,       "Inverse" },
    { Style::Hidden,        "Hidden" },
    { Style::Strikethrough, "Strikethrough" },
};

template <typename T, size_t N>
[[nodiscard]]
inline T find_named_enum(const NamedEnum<T>(&table)[N], sv_t name) noexcept {
    for (const auto& entry : table)
        if (name == entry.name)
            return entry.value;
    return static_cast<T>(-1);
}

template <typename T, size_t N>
inline os_t& write_named_enum(os_t& os, T value,
                                        const NamedEnum<T> (&table)[N]) noexcept
{
    for (const auto& entry : table)
        if (entry.value == value)
            return os << entry.name;
    return os;
}

inline os_t& operator<<(os_t& os, Color c) noexcept {
    return write_named_enum(os, c, enumColors);
}

inline os_t& operator<<(os_t& os, Background b) noexcept {
    return write_named_enum(os, b, enumBackgrounds);
}

inline os_t& operator<<(os_t& os, Style s) noexcept {
    return write_named_enum(os, s, enumStyles);
}

inline os_t& write(os_t& os, const ansi::Format& fmt) noexcept {
    os << "{" << fmt.foreground << ", " << fmt.background << ", {";
    for (const auto& s : fmt.styles)
        os << (&s == fmt.styles.begin() ? "" : ", ") << s;
    os << "}, use_ansi=" << std::boolalpha << fmt.use_ansi << "}";
    return os;
}

inline os_t& operator<<(os_t& os, const ansi::Format& fmt) noexcept {
    return write(os, fmt);
}

} // namespace ansi

