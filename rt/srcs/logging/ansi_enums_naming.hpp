#pragma once
#include "ansi_escape_codes.hpp"

namespace ansi {

template <typename T>
struct NamedEnum {
    T value;
    const char* name;
};

constexpr NamedEnum<Color> enumColors[] = {
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
    { Style::Bold,          "Bold" },
    { Style::Faint,         "Faint" },
    { Style::Italic,        "Italic" },
    { Style::Underline,     "Underline" },
    { Style::Inverse,       "Inverse" },
    { Style::Hidden,        "Hidden" },
    { Style::Strikethrough, "Strikethrough" }
};

} // namespace ansi

