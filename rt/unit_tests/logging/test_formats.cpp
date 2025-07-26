#include <iostream>
#include <thread>
#include <vector>
#include "../../srcs/logging/format.hpp"
#include "../../srcs/logging/ansi_enums_naming.hpp"
#include "../../srcs/logging/timing.hpp"

using namespace logging;

struct formats_t { sv_t sv; ansi::Format ansi_fmt{}; };

void test_apply_formats(os_t& os, formats_t f) {
    using EndPolicy = Format::Control::EndPolicy;
    Format fmt;
    fmt.ansi_style = f.ansi_fmt;
    if (f.sv != "Hidden text") {
        fmt.control.end_policy = EndPolicy::Newline;
        fmt.apply(os, f.sv);
    } else {
        fmt.control.end_policy = EndPolicy::PadThenFlush;
        fmt.apply(os, f.sv) << "[" << f.sv << "]\n";
    }
}

void time_delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

template <typename T, size_t N>
os_t& test_ansi_style(os_t& os, sv_t prompt, const ansi::NamedEnum<T> (&en)[N],
                      int delay_ms = 1000) noexcept
{
    Format title_fmt;
    title_fmt.ansi_style.styles = { ansi::Style::Bold };
    title_fmt.control.normalize = Format::Control::Normalize::Required;

    Format cell_fmt;
    cell_fmt.control.normalize = Format::Control::Normalize::Required;

    title_fmt.apply(os, prompt);
    if(delay_ms)
        time_delay(delay_ms);

    for (const auto& e : en) {
        if constexpr (std::is_same_v<std::decay_t<T>, ansi::Color>) {
            cell_fmt.ansi_style.foreground = e.value;
            if (cell_fmt.ansi_style.foreground == ansi::Color::Black)
                cell_fmt.ansi_style.background = ansi::Background::White;
            else
                cell_fmt.ansi_style.background = ansi::Background::Default;
        }
        if constexpr (std::is_same_v<std::decay_t<T>, ansi::Background>) {
            cell_fmt.ansi_style.background = e.value;
            if (cell_fmt.ansi_style.background == ansi::Background::White)
                cell_fmt.ansi_style.foreground = ansi::Color::Black;
            else if (cell_fmt.ansi_style.background == ansi::Background::Black)
                cell_fmt.ansi_style.foreground = ansi::Color::Default;
            else
                cell_fmt.ansi_style.foreground = ansi::Color::Black;
        }
        if constexpr (std::is_same_v<std::decay_t<T>, ansi::Style>) {
            cell_fmt.ansi_style.styles = {e.value};
        }

        cell_fmt.apply(os, e.name);
        if(delay_ms) {
            time_delay(delay_ms);
            cell_fmt.apply_clear(os);
        }
    }
    if(delay_ms)
        title_fmt.apply_clear(os);
    return os;
}


namespace rt { thread_local Config config; }

// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_formats.cpp -o formats && ./formats
int main(int ac, char** av) {
    if (auto r = rt::config.parse_cmdline(ac, av); r) return r.write_error_if();
    
    ScopedTimer timer(std::cout);

    int delay_ms = 200;

    if (rt::config.tty_allowed && rt::config.ansi_allowed)
        terminal_clear_fallback();

    test_ansi_style(std::cout, "▶️ Foreground Colors:", ansi::enumColors, delay_ms);
    test_ansi_style(std::cout, "✅ Foreground Colors:", ansi::enumColors, 0);
    std::cout << std::endl;

    test_ansi_style(std::cout, "▶️ Background Colors:", ansi::enumBackgrounds, delay_ms);
    test_ansi_style(std::cout, "✅ Background Colors:", ansi::enumBackgrounds, 0);
    std::cout << std::endl;

    test_ansi_style(std::cout, "▶️ Styles:           ", ansi::enumStyles, delay_ms);
    test_ansi_style(std::cout, "✅ Styles:           ", ansi::enumStyles, 0);
    std::cout << std::endl;
    
    std::vector<formats_t> fmts;

    fmts.emplace_back(
                      "✅ Style Combinations:",
                      ansi::Format{
                          .styles = { ansi::Style::Bold },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "Red + bold (error)",
                      ansi::Format{
                          .foreground = ansi::Color::Red,
                          .styles = { ansi::Style::Bold },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "BrightRed + bold (error)",
                      ansi::Format{
                          .foreground = ansi::Color::BrightRed,
                          .styles = { ansi::Style::Bold },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "Yellow + underline (warning)",
                      ansi::Format{
                          .foreground = ansi::Color::Yellow,
                          .styles = { ansi::Style::Underline },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "BrightYellow + underline (warning)",
                      ansi::Format{
                          .foreground = ansi::Color::BrightYellow,
                          .styles = { ansi::Style::Underline },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "Blue + Italic (Information)",
                      ansi::Format{
                          .foreground = ansi::Color::Blue,
                          .styles = { ansi::Style::Italic },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "BrightBlue + Italic (Information)",
                      ansi::Format{
                          .foreground = ansi::Color::BrightBlue,
                          .styles = { ansi::Style::Italic },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "Green + bold (success)",
                      ansi::Format{
                          .foreground = ansi::Color::Green,
                          .styles = { ansi::Style::Bold },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "BrightGreen + bold (success)",
                      ansi::Format{
                          .foreground = ansi::Color::BrightGreen,
                          .styles = { ansi::Style::Bold },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "Bright black background + white text (contrast mark)",
                      ansi::Format{
                          .foreground = ansi::Color::White,
                          .background = ansi::Background::BrightBlack,
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "Yellow background + red text + bold",
                      ansi::Format{
                          .foreground = ansi::Color::Red,
                          .background = ansi::Background::Yellow,
                          .styles = { ansi::Style::Bold },
                          .use_ansi = true
                      });
    fmts.emplace_back(
                      "Invert + bold",
                      ansi::Format{
                          .styles = {
                              ansi::Style::Inverse,
                              ansi::Style::Bold
                          },
                              .use_ansi = true
                      });
    fmts.emplace_back(
                      "All styles (as a test)",
                      ansi::Format{
                          .styles = {
                              ansi::Style::Bold,
                              ansi::Style::Underline,
                              ansi::Style::Italic,
                              ansi::Style::Strikethrough
                          },
                              .use_ansi = true
                      });
    fmts.emplace_back(
                      "Hidden text",
                      ansi::Format{
                          .styles = { ansi::Style::Hidden },
                          .use_ansi = true
                      });

    for (formats_t f : fmts) {
        test_apply_formats(std::cout, f);
        time_delay(delay_ms);
    }

    return 0;
}
