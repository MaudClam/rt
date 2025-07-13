#include <iostream>
#include <thread>
#include <vector>
#include "../srcs/logging/cell.hpp"
#include "../srcs/logging/ansi_enums_naming.hpp"
#include "../srcs/logging/timing.hpp"

using namespace logging;
using namespace ansi;

struct formats_t { sv_t sv; Format fmt{}; };

void test_write_formats(os_t& os, formats_t f) {
    using Controls = CellFormat::Control::EndPolicy;
    Cell<sv_t> cell;
    cell.value = f.sv;
    cell.format.ansi_style = f.fmt;
    if (f.sv != "Hidden text") {
        cell.format.control.end_policy = Controls::Newline;
        os << cell;
    } else {
        cell.format.control.end_policy = Controls::PadThenFlush;
        os << cell << "[" << f.sv << "]\n";
    }
}

void time_delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

template <typename T, size_t N>
os_t& test_ansi_style(os_t& os, sv_t prompt, const NamedEnum<T> (&en)[N],
                      int delay_ms = 1000) noexcept
{
    Cell<sv_t> title;
    title.value = prompt;
    title.format.ansi_style.styles = {Style::Bold};
    title.format.control.normalize = CellFormat::Control::Normalize::Required;

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


namespace rt { Config config; }

// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_Cell_and_ansi.cpp -o cell_colors && ./cell_colors
int main(int ac, char** av) {
    if (auto r = rt::config.parse_cmdline(ac, av); r) return r.write_error_if();
    
    ScopedTimer timer(std::cout);

    int delay_ms = 200;

    if (rt::config.tty_allowed && rt::config.ansi_allowed)
        terminal_clear_fallback();

    test_ansi_style(std::cout, "▶️ Foreground Colors:", enumColors, delay_ms);
    test_ansi_style(std::cout, "✅ Foreground Colors:", enumColors, 0);
    std::cout << std::endl;

    test_ansi_style(std::cout, "▶️ Background Colors:", enumBackgrounds, delay_ms);
    test_ansi_style(std::cout, "✅ Background Colors:", enumBackgrounds, 0);
    std::cout << std::endl;

    test_ansi_style(std::cout, "▶️ Styles:           ", enumStyles, delay_ms);
    test_ansi_style(std::cout, "✅ Styles:           ", enumStyles, 0);
    std::cout << std::endl;
    
    std::vector<formats_t> fmts;

    fmts.emplace_back("✅ Style Combinations:",
                      Format{ .styles = { Style::Bold }, .use_ansi = true });
    fmts.emplace_back("Red + bold (error)",
                      Format{ .foreground = Color::Red, .styles = { Style::Bold }, .use_ansi = true });
    fmts.emplace_back("BrightRed + bold (error)",
                      Format{ .foreground = Color::BrightRed, .styles = { Style::Bold }, .use_ansi = true });
    fmts.emplace_back("Yellow + underline (warning)",
                      Format{ .foreground = Color::Yellow, .styles = { Style::Underline }, .use_ansi = true });
    fmts.emplace_back("BrightYellow + underline (warning)",
                      Format{ .foreground = Color::BrightYellow, .styles = { Style::Underline }, .use_ansi = true });
    fmts.emplace_back("Blue + Italic (Information)",
                      Format{ .foreground = Color::Blue, .styles = { Style::Italic }, .use_ansi = true });
    fmts.emplace_back("BrightBlue + Italic (Information)",
                      Format{ .foreground = Color::BrightBlue, .styles = { Style::Italic }, .use_ansi = true });
    fmts.emplace_back("Green + bold (success)",
                      Format{ .foreground = Color::Green, .styles = { Style::Bold }, .use_ansi = true });
    fmts.emplace_back("BrightGreen + bold (success)",
                      Format{ .foreground = Color::BrightGreen, .styles = { Style::Bold }, .use_ansi = true });
    fmts.emplace_back("Bright black background + white text (contrast mark)",
                      Format{ .foreground = Color::White, .background = Background::BrightBlack, .use_ansi = true });
    fmts.emplace_back("Yellow background + red text + bold",
                      Format{ .foreground = Color::Red, .background = Background::Yellow, .styles = { Style::Bold }, .use_ansi = true });
    fmts.emplace_back("Invert + bold",
                      Format{ .styles = { Style::Inverse, Style::Bold }, .use_ansi = true });
    fmts.emplace_back("All styles (as a test)",
                      Format{ .styles = { Style::Bold, Style::Underline, Style::Italic, Style::Strikethrough }, .use_ansi = true });
    fmts.emplace_back("Hidden text",
                      Format{ .styles = { Style::Hidden }, .use_ansi = true });

    for (formats_t f : fmts) {
        test_write_formats(std::cout, f);
        time_delay(delay_ms);
    }

    return 0;
}
