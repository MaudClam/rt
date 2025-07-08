#include <iostream>
#include <thread>
#include "../srcs/logging/cell.hpp"
#include "../srcs/logging/ansi_enums_naming.hpp"

using namespace logging;
using namespace ansi;

void test_write_formats(sv_t text, const ansi::Format& fmt, std::ostream& out = std::cout) {
    write_style(out, fmt);
    out << text;
    write_reset(out, fmt);
    if (text == "Hidden text")
        out << " [" << text << "]";}

void time_delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

template <typename T, size_t N>
[[nodiscard]] inline int measure_max_width(const NamedEnum<T> (&en)[N], Cell<sv_t>& cell) {
    int max_width = 0;
    for (const auto& e : en) {
        cell.value = e.name;
        const int width = cell.measure_width();
        if (width <= 0)
            return unset;
        if (width > max_width)
            max_width = width;
    }
    return max_width;
}

template <typename T, size_t N>
os_t& test_ansi_style(os_t& os, sv_t prompt, const NamedEnum<T> (&en)[N],
                      int delay_ms = 1000) noexcept
{
    Cell<sv_t> title;
    title.value = prompt;
    title.format.ansi_style.styles = {Style::Bold};
    title.format.control.normalize = CellFormat::Control::Normalize::Required;
//    title.format.width = 25;

    Cell<sv_t> cell;

    os << title;
    if(delay_ms)
        time_delay(delay_ms);

    for (const auto& e : en) {
        cell.value = e.name;
        if constexpr (std::is_same_v<std::decay_t<T>, Color>) {
            cell.format.ansi_style.foreground = e.value;
            if (cell.format.ansi_style.foreground == Color::Black)
                cell.format.ansi_style.background = Background::White;
            else
                cell.format.ansi_style.background = Background::Default;
        }
        if constexpr (std::is_same_v<std::decay_t<T>, Background>) {
            cell.format.ansi_style.background = e.value;
            if (cell.format.ansi_style.background == Background::White)
                cell.format.ansi_style.foreground = Color::Black;
            else if (cell.format.ansi_style.background == Background::Black)
                cell.format.ansi_style.foreground = Color::Default;
            else
                cell.format.ansi_style.foreground = Color::Black;
        }
        if constexpr (std::is_same_v<std::decay_t<T>, Style>) {
            cell.format.ansi_style.styles = {e.value};
        }
        os << cell;
        if(delay_ms) {
            time_delay(delay_ms);
            cell.force_clear(os);
        }
    }
    if(delay_ms)
        title.force_clear(os);
    return os;
}

void test_display_unit(os_t& os, sv_t input) {
    os << "[test] input='" << input << "', length=" << input.size() << '\n';
    using namespace logging;
    oss_t buff;
    size_t offset = 0;
    int width = 0;
    DisplayUnit du;
    while (du.parse(input, offset)) {
        du.write(buff, input);
        width += du.width;
        offset += du.length;
    }
    os << "[test] output='" << buff.view() << "', width=" << width << '\n';
}


namespace rt { Config config; }

// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_Cell_and_ansi.cpp -o cell_colors && ./cell_colors
int main(int ac, char** av) {
    if (auto r = rt::config.parse_cmdline(ac, av); r) return r.write_error_if();

//    int delay_ms = 100;

//    if (rt::config.tty_allowed && rt::config.ansi_allowed)
//        terminal_clear_fallback();
//    std::cout << "ABCDEFG";
//    test_ansi_style(std::cout, "123💇🏽‍♀️ Папа у Васи:", enumColors, delay_ms);
//    test_ansi_style(std::cout, "123💇🏽‍♀️ Папа у Васи:", enumColors, 0);
//    std::cout << std::endl;
//
//    std::cout << "ABCDEFG";
//    test_ansi_style(std::cout, "123✅ Папа у Васи Colors:", enumBackgrounds, delay_ms);
//    test_ansi_style(std::cout, "123✅ Папа у Васи Colors:", enumBackgrounds, 0);
//    std::cout << std::endl;
//
//    std::cout << "ABCDEFG";
//    test_ansi_style(std::cout, "123▶️ Styles:", enumStyles, delay_ms);
//    test_ansi_style(std::cout, "123▶️ Styles:", enumStyles, 0);
//    std::cout << std::endl;

    test_display_unit(std::cout, "123 abc");

//    {
//        Format fmt{ .styles = { Style::Bold }, .use_ansi = true };
//        test_write_formats("✅ Style Combinations:", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .foreground = Color::Red, .styles = { Style::Bold }, .use_ansi = true };
//        test_write_formats("Red + bold (error)", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .foreground = Color::BrightRed, .styles = { Style::Bold }, .use_ansi = true };
//        test_write_formats("BrightRed + bold (error)", fmt);
//        std::cout << "\n";
//   }
//
//    {
//        Format fmt{ .foreground = Color::Yellow, .styles = { Style::Underline }, .use_ansi = true };
//        test_write_formats("Yellow + underline (warning)", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .foreground = Color::BrightYellow, .styles = { Style::Underline }, .use_ansi = true };
//        test_write_formats("BrightYellow + underline (warning)", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .foreground = Color::Blue, .styles = { Style::Italic }, .use_ansi = true };
//        test_write_formats("Blue + Italic (Information)", fmt);
//        std::cout << "\n";
//   }
//
//    {
//        Format fmt{ .foreground = Color::BrightBlue, .styles = { Style::Italic }, .use_ansi = true };
//        test_write_formats("BrightBlue + Italic (Information)", fmt);
//        std::cout << "\n";
//   }
//
//    {
//        Format fmt{ .foreground = Color::Green, .styles = { Style::Bold }, .use_ansi = true };
//        test_write_formats("Green + bold (success)", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .foreground = Color::BrightGreen, .styles = { Style::Bold }, .use_ansi = true };
//        test_write_formats("BrightGreen + bold (success)", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .foreground = Color::White, .background = Background::BrightBlack, .use_ansi = true };
//        test_write_formats("Bright black background + white text (contrast mark)", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .foreground = Color::Red, .background = Background::Yellow, .styles = { Style::Bold }, .use_ansi = true };
//        test_write_formats("Yellow background + red text + bold", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .styles = { Style::Inverse, Style::Bold }, .use_ansi = true };
//        test_write_formats("Invert + bold", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .styles = { Style::Bold, Style::Underline, Style::Italic, Style::Strikethrough }, .use_ansi = true };
//        test_write_formats("All styles (as a test)", fmt);
//        std::cout << "\n";
//    }
//
//    {
//        Format fmt{ .styles = { Style::Hidden }, .use_ansi = true };
//        test_write_formats("Hidden text", fmt);
//        std::cout << "\n";
//    }

    return 0;
}
