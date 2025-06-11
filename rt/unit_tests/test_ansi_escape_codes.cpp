#include <iostream>
#include "../srcs/logging/ansi_enums_naming.hpp"

using namespace ansi;

void test_write_formats(sv_t text, const Format& fmt, std::ostream& out = std::cout) {
    ansi_style(out, fmt);
    out << text;
    reset_if(out, fmt);
    out << " [" << text << "]\n";}

// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_ansi_escape_codes.cpp -o colors && ./colors
int main() {

    std::cout << "\n=== Foreground Colors ===\n";
    for (const auto& c : enumColors) {
        Format fmt{ .foreground = c.value, .use_ansi = true };
        test_write_formats(c.name, fmt);
    }

    std::cout << "\n=== Background Colors ===\n";
    for (const auto& b : enumBackgrounds) {
        Format fmt{ .background = b.value, .use_ansi = true };
        test_write_formats(b.name, fmt);
    }

    std::cout << "\n=== Styles ===\n\n";
        for (const auto& s : enumStyles) {
            Format fmt{ .styles = {s.value}, .use_ansi = true };
            test_write_formats(s.name, fmt);
            std::cout << "\n";
        }

    std::cout << "\n=== Style Combinations ===\n\n";
    {
        Format fmt{ .foreground = Color::Red, .styles = { Style::Bold }, .use_ansi = true };
        test_write_formats("Red + bold (error)", fmt);
        std::cout << "\n";
   }

    {
        Format fmt{ .foreground = Color::BrightRed, .styles = { Style::Bold }, .use_ansi = true };
        test_write_formats("BrightRed + bold (error)", fmt);
        std::cout << "\n";
   }

    {
        Format fmt{ .foreground = Color::Yellow, .styles = { Style::Underline }, .use_ansi = true };
        test_write_formats("Yellow + underline (warning)", fmt);
        std::cout << "\n";
    }

    {
        Format fmt{ .foreground = Color::BrightYellow, .styles = { Style::Underline }, .use_ansi = true };
        test_write_formats("BrightYellow + underline (warning)", fmt);
        std::cout << "\n";
    }

    {
        Format fmt{ .foreground = Color::Blue, .styles = { Style::Italic }, .use_ansi = true };
        test_write_formats("Blue + Italic (Information)", fmt);
        std::cout << "\n";
   }

    {
        Format fmt{ .foreground = Color::BrightBlue, .styles = { Style::Italic }, .use_ansi = true };
        test_write_formats("BrightBlue + Italic (Information)", fmt);
        std::cout << "\n";
   }

    {
        Format fmt{ .foreground = Color::Green, .styles = { Style::Bold }, .use_ansi = true };
        test_write_formats("Green + bold (success)", fmt);
        std::cout << "\n";
    }

    {
        Format fmt{ .foreground = Color::BrightGreen, .styles = { Style::Bold }, .use_ansi = true };
        test_write_formats("BrightGreen + bold (success)", fmt);
        std::cout << "\n";
    }

    {
        Format fmt{ .foreground = Color::White, .background = Background::BrightBlack, .use_ansi = true };
        test_write_formats("Bright black background + white text (contrast mark)", fmt);
        std::cout << "\n";
    }

    {
        Format fmt{ .foreground = Color::Red, .background = Background::Yellow, .styles = { Style::Bold }, .use_ansi = true };
        test_write_formats("Yellow background + red text + bold", fmt);
        std::cout << "\n";
    }

    {
        Format fmt{ .styles = { Style::Bold, Style::Underline, Style::Italic, Style::Strikethrough }, .use_ansi = true };
        test_write_formats("All styles (as a test)", fmt);
        std::cout << "\n";
    }

    {
        Format fmt{ .styles = { Style::Hidden }, .use_ansi = true };
        test_write_formats("Hidden text", fmt);
        std::cout << "\n";
    }

    {
        Format fmt{ .styles = { Style::Inverse, Style::Bold }, .use_ansi = true };
        test_write_formats("Invert + bold", fmt);
        std::cout << "\n";
    }

    return 0;
}
