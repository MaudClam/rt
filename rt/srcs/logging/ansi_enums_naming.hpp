#pragma once
#include "ansi_escape_codes.hpp"
#include "../common/buffer.hpp"

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
[[nodiscard]]
inline sv_t as_sv(T value, const NamedEnum<T> (&table)[N]) noexcept {
    for (const auto& entry : table)
        if (entry.value == value)
            return entry.name;
    return {};
}

[[nodiscard]] inline sv_t as_sv(Color c) noexcept {return as_sv(c, enumColors);};
[[nodiscard]] inline sv_t as_sv(Background b) noexcept {return as_sv(b, enumBackgrounds);};
[[nodiscard]] inline sv_t as_sv(Style s) noexcept {return as_sv(s, enumStyles);};
[[nodiscard]] inline sv_t as_sv(const Format& fmt) noexcept {
    thread_local common::RawBuffer<160> buf;
    buf.append('{', as_sv(fmt.foreground), ", ", as_sv(fmt.background), ", {");
    for (const auto& s : fmt.styles)
        buf.append((&s == fmt.styles.begin() ? "" : ", "), as_sv(s));
    buf.append("}, use_ansi=", fmt.use_ansi, '}');
    buf.finalize_ellipsis_newline(false);
    return buf.view();
}

inline os_t& write(os_t& os, const Format& fmt) { return os << as_sv(fmt); }
inline os_t& operator<<(os_t& os, Color c) { return os << as_sv(c); }
inline os_t& operator<<(os_t& os, Background b) { return os << as_sv(b); }
inline os_t& operator<<(os_t& os, Style s) { return os << as_sv(s); }
inline os_t& operator<<(os_t& os, const Format& fmt) { return write(os, fmt); }

} // namespace ansi
