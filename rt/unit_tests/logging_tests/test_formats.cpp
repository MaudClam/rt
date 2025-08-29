#include <iostream>
#include <thread>
#include "../../srcs/logging/format.hpp"
#include "../../srcs/logging/ansi_enums_naming.hpp"

using namespace logging;

void time_delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

template <typename T>
os_t& test_ansi_format(os_t& os, sv_t name, T value, int delay_ms,
                                                     int width = kUnset) noexcept
{
    logging::Format cell{
        .control.normalize = logging::Format::Control::Normalize::Required,
        .width = width
    };
    if constexpr (std::is_same_v<std::decay_t<T>, ansi::Color>) {
        cell.ansi_format.foreground = value;
        cell.preserve_background   = false;
    }
    if constexpr (std::is_same_v<std::decay_t<T>, ansi::Background>) {
        cell.ansi_format.background = value;
        cell.ansi_format.foreground = same(value);
    }
    if constexpr (std::is_same_v<std::decay_t<T>, ansi::Style>) {
        cell.ansi_format.styles = {value};
    }
    if constexpr (std::is_same_v<std::decay_t<T>, ansi::styles_t>) {
        cell.ansi_format.styles = value;
    }
    cell.apply(os, name);
    if(delay_ms) {
        time_delay(delay_ms);
        cell.apply_clear(os);
    }
    return os;
}

template <typename T, size_t N>
os_t& test_ansi_formats(os_t& os, sv_t prompt, const ansi::NamedEnum<T> (&en)[N],
                                      int delay_ms, int width = kUnset ) noexcept
{
    Format title{};
    title.ansi_format.styles = { ansi::Style::Bold };
    title.control.normalize = Format::Control::Normalize::Required;

    title.apply(os, prompt);
    if(delay_ms) time_delay(delay_ms);
    for (const auto& e : en)
        test_ansi_format(os, e.name, e.value, delay_ms, width);
    if(delay_ms) title.apply_clear(os);
    return os;
}

os_t& test_combinations(os_t& os, sv_t prompt_, int delay_ms, int width = kUnset) {
    Format cell{};
    cell.ansi_format = {.styles = { ansi::Style::Bold }};
    cell.apply(os, prompt_, " width=", width) << '\n';

    time_delay(delay_ms);
    cell.width = width;

    cell.ansi_format = {
        .foreground = ansi::Color::Red,
        .background = ansi::Background::Default,
        .styles = { ansi::Style::Bold },};
    cell.apply(os, "[error]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::BrightRed,
        .background = ansi::Background::Default,};
    cell.apply(os, "[error]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Yellow,
        .background = ansi::Background::Default,
        .styles = { ansi::Style::Underline },};
    cell.apply(os, "[warning]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Blue,
        .background = ansi::Background::Default,
        .styles = { ansi::Style::Italic },};
    cell.apply(os, "[information]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::BrightBlue,
        .background = ansi::Background::Default,
        .styles = { ansi::Style::Italic },};
    cell.apply(os, "[information]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Green,
        .background = ansi::Background::Default,
        .styles = { ansi::Style::Bold },};
    cell.apply(os, "[success]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::BrightGreen,
        .background = ansi::Background::Default,};
    cell.apply(os, "[success]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::White,
        .background = ansi::Background::BrightBlack,};
    cell.apply(os, "[contrast mark]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Red,
        .background = ansi::Background::Yellow,
        .styles = {ansi::Style::Bold},};
    cell.apply(os, "[contrast mark]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Default,
        .background = ansi::Background::Default,
        .styles = {
            ansi::Style::Inverse,
            ansi::Style::Bold},};
    cell.apply(os, "[invert + bold]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Default,
        .background = ansi::Background::Default,
        .styles = {
            ansi::Style::Bold,
            ansi::Style::Italic,
            ansi::Style::Underline,
            ansi::Style::Strikethrough,},};
    cell.apply(os, "[abrakaddabbra]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .use_ansi = false,
        .foreground = ansi::Color::Default,
        .background = ansi::Background::Default,
        .styles = {
            ansi::Style::Bold,
            ansi::Style::Italic,
            ansi::Style::Underline,
            ansi::Style::Strikethrough,},};
    cell.apply(os, "[abrakaddabbra]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);
    
    cell.ansi_format = {.use_ansi = true};

    cell.ansi_format = {
        .use_ansi = true,
        .foreground = ansi::Color::Default,
        .background = ansi::Background::Default,
        .styles = {ansi::Style::Hidden},};
    cell.apply(os, "[hidden]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .use_ansi = false,
        .foreground = ansi::Color::Default,
        .background = ansi::Background::Default,
        .styles = {ansi::Style::Hidden},};
    cell.apply(os, "[hidden]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {.use_ansi = true};

    cell.ansi_format = {
        .foreground = ansi::Color::White,
        .background = ansi::Background::White,};
    cell.preserve_background = true;
    cell.apply(os, "[safe contrast]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::White,
        .background = ansi::Background::White,};
    cell.preserve_background = false;
    cell.apply(os, "[safe contrast]") << ' ' << cell.ansi_format
    << ", preserve_background=" << std::boolalpha << cell.preserve_background << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Cyan,
        .background = ansi::Background::Cyan,};
    cell.preserve_background = true;
    cell.apply(os, "[safe contrast]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Cyan,
        .background = ansi::Background::Cyan,};
    cell.preserve_background = false;
    cell.apply(os, "[safe contrast]") << ' ' << cell.ansi_format
    << ", preserve_background=" << std::boolalpha << cell.preserve_background << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Green,
        .background = ansi::Background::Green,};
    cell.preserve_background = true;
    cell.apply(os, "[safe contrast]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Green,
        .background = ansi::Background::Green,};
    cell.preserve_background = false;
    cell.apply(os, "[safe contrast]") << ' ' << cell.ansi_format
    << ", preserve_background=" << std::boolalpha << cell.preserve_background << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::White,
        .background = ansi::Background::White,
        .styles = {ansi::Style::Strikethrough},};
    cell.preserve_background = true;
    cell.align.mode = logging::Format::Align::Mode::Left;
    cell.apply(os, "[align]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::White,
        .background = ansi::Background::White,
        .styles = {ansi::Style::Strikethrough},};
    cell.preserve_background = false;
    cell.align.mode = logging::Format::Align::Mode::Centred;
    cell.apply(os, "[align]") << ' ' << cell.ansi_format << ", align.mode=Centred"
    << ", preserve_background=" << std::boolalpha << cell.preserve_background << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::White,
        .background = ansi::Background::White,
        .styles = {ansi::Style::Strikethrough},};
    cell.preserve_background = true;
    cell.align.mode = logging::Format::Align::Mode::Right;
    cell.apply(os, "[align]") << ' ' << cell.ansi_format << ", align.mode=Right" << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::White,
        .background = ansi::Background::White,
        .styles = {ansi::Style::Strikethrough},};
    cell.preserve_background = false;
    cell.width = kUnset;
    sv_t prompt = "[width]";
    int indent = width - static_cast<int>(prompt.size()) - 1;
    cell.apply(os, prompt);
    ansi::apply_pad(os, indent) << ' ' << cell.ansi_format << ", width=kUnset(-1)"
    << ", preserve_background=" << std::boolalpha << cell.preserve_background << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::White,
        .background = ansi::Background::White,
        .styles = {ansi::Style::Strikethrough},};
    cell.preserve_background = true;
    cell.width = kHidden;
    prompt = "[width]";
    cell.apply(os, prompt);
    ansi::apply_pad(os, width) << ' ' << cell.ansi_format << ", width=kHidden(0)" << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::BrightRed,
        .background = ansi::Background::BrightRed,};
    cell.preserve_background = true;
    cell.align.mode = logging::Format::Align::Mode::Left;
    cell.width = width;
    cell.apply(os, "[trancatetrancatetrancatetrancate]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Black,
        .background = ansi::Background::Black,};
    cell.preserve_background = true;
    cell.width = width;
    cell.apply(os, "[trancatetrancatetrancatetrancate]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .foreground = ansi::Color::Black,
        .background = ansi::Background::Black,};
    cell.preserve_background = false;
    cell.width = width;
    cell.apply(os, "[trancatetrancatetrancatetrancate]") << ' ' << cell.ansi_format
    << ", preserve_background=" << std::boolalpha << cell.preserve_background << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .use_ansi = false,
        .foreground = ansi::Color::Black,
        .background = ansi::Background::Black,};
    cell.preserve_background = true;
    cell.width = width;
    cell.apply(os, "[trancatetrancatetrancatetrancate]") << ' ' << cell.ansi_format << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .use_ansi = true,
        .foreground = ansi::Color::Black,
        .background = ansi::Background::Black,};
    cell.preserve_background = true;
    cell.truncate.ansi_format.use_ansi = false;
    cell.width = width;
    cell.apply(os, "[trancatetrancatetrancatetrancate]") << ' ' << cell.ansi_format
    << ", truncate.ansi_format.use_ansi=" << std::boolalpha << cell.truncate.ansi_format.use_ansi << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .use_ansi = true,
        .foreground = ansi::Color::Black,
        .background = ansi::Background::Black,};
    cell.preserve_background = true;
    cell.truncate.ansi_format.use_ansi = true;
    cell.width = width;
    cell.truncate.cutlen = 1;
    cell.apply(os, "[trancatetrancatetrancatetrancate]") << ' ' << cell.ansi_format
    << ", truncate.cutlen=" << cell.truncate.cutlen << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .use_ansi = true,
        .foreground = ansi::Color::BrightWhite,
        .background = ansi::Background::BrightWhite,};
    cell.preserve_background = true;
    cell.truncate.ansi_format.use_ansi = true;
    cell.width = 2;
    cell.truncate.cutlen = 3;
    cell.apply(os, "[trancatetrancatetrancatetrancate]");
    ansi::apply_pad(os, width - cell.width) << ' ' << cell.ansi_format
    << ", width=" << cell.width << '\n';
    time_delay(delay_ms);

    cell.ansi_format = {
        .use_ansi = true,
        .foreground = ansi::Color::White,
        .background = ansi::Background::White,};
    cell.preserve_background = true;
    cell.truncate.ansi_format.use_ansi = true;
    cell.width = width;
    cell.truncate.cutlen = 3;
    cell.truncate.enabled = false;
    cell.apply(os, "[trancatetrancatetrancatetrancate]") << ' ' << cell.ansi_format
    << ", width=" << cell.width << ", truncate.enabled=" << cell.truncate.enabled << '\n';
    time_delay(delay_ms);

    return os;
}

// g++ -std=c++2a -O2 -Wall -Wextra -Werror ../../srcs/globals.cpp test_formats.cpp -o formats && ./formats
// ./formats --no-ansi --no-utf8
int main(int ac, char** av) {
    rt::config.init(ac, av);
    os_t& os = std::cout;

    int delay_ms = 300;
    int width    = kUnset;

    test_ansi_formats(os, "▶️ Foregrounds:", ansi::enumColors, delay_ms, width);
    test_ansi_formats(os, "✅ Foregrounds:", ansi::enumColors, 0, width);
    os << std::endl;
    test_ansi_formats(os, "▶️ Backgrounds:", ansi::enumBackgrounds, delay_ms, width);
    test_ansi_formats(os, "✅ Backgrounds:", ansi::enumBackgrounds, 0, width);
    os << std::endl;
    test_ansi_formats(os, "▶️ Styles:     ", ansi::enumStyles, delay_ms, width);
    test_ansi_formats(os, "✅ Styles:     ", ansi::enumStyles, 0, width);
    os << std::endl;
    width = 15;
    test_combinations(os, "✅ Style Combinations:", delay_ms, width);
    
    return 0;
}
