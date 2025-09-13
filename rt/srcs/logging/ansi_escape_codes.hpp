#pragma once
#include "../common/types.hpp"

namespace ansi {

template<typename T, std::size_t MaxN> struct style_list;

inline constexpr char PARAM_SEP        = ';';
inline constexpr char SGR_SUFFIX       = 'm';
inline constexpr sv_t CSI_SEQ          = "\033[";
inline constexpr sv_t RESET_SEQ        = "\033[0m";
inline constexpr sv_t CLEAR_SCREEN_SEQ = "\033[H\033[2J";
inline constexpr sv_t CLEAR_LINE_SEQ   = "\033[K";

template<typename T, std::size_t MaxN>
struct style_list {
    T data[MaxN]{};
    std::size_t count = 0;
    constexpr style_list() = default;
    constexpr style_list(const style_list&) = default;
    constexpr style_list& operator=(const style_list&) = default;
    constexpr style_list(std::initializer_list<T> init) {
        count = std::min(init.size(), MaxN);
        std::copy_n(init.begin(), count, data);
        // Extra styles are silently discarded.
    }
    [[nodiscard]] constexpr std::size_t size() const noexcept { return count; }
    [[nodiscard]] constexpr bool empty() const noexcept { return count == 0; }
    [[nodiscard]] constexpr T* begin() noexcept { return data; }
    [[nodiscard]] constexpr T* end() noexcept { return data + count; }
    [[nodiscard]] constexpr const T* begin() const noexcept { return data; }
    [[nodiscard]] constexpr const T* end() const noexcept { return data + count; }
    constexpr void push_back(const T& value) {
        assert(count < MaxN && "style_list overflow");
        data[count++] = value;
    }
    constexpr void clear() noexcept { count = 0; }
    constexpr T& operator[](std::size_t i) noexcept { return data[i]; }
    constexpr const T& operator[](std::size_t i) const noexcept { return data[i]; }

    template<class Pred>
    constexpr std::size_t erase_if(Pred pred) noexcept {
        std::size_t w = 0;
        for (std::size_t r = 0; r < count; ++r) {
            const T& x = data[r];
            if (!pred(x)) data[w++] = x;
        }
        const std::size_t removed = count - w;
        count = w;
        return removed;
    }
    
    template<class Pred>
    [[nodiscard]] constexpr bool any_of(Pred pred) const noexcept {
        for (std::size_t i = 0; i < count; ++i)
            if (pred(data[i])) return true;
        return false;
    }
};

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

inline constexpr bool is_normal(const Color& c) noexcept {
    const int code = static_cast<int>(c);
    return code >= 30 && code <= 37;
}

inline constexpr bool is_normal(const Background& b) noexcept {
    const int code = static_cast<int>(b);
    return code >= 40  && code <= 47;
}

inline constexpr bool is_bright(const Color& c) noexcept {
    const int code = static_cast<int>(c);
    return code >= 90 && code <= 97;
}

inline constexpr bool is_bright(const Background& b) noexcept {
    const int code = static_cast<int>(b);
    return code >= 100 && code <= 107;
}

inline constexpr bool is_foreground(const Color& c) noexcept {
    return is_normal(c) || is_bright(c);
}

inline constexpr bool is_background(const Background& b) noexcept {
    return is_normal(b) || is_bright(b);
}

inline constexpr bool operator==(const Color& c, const Background& b) noexcept {
    if (is_foreground(c) && is_background(b))
        return static_cast<int>(b) - static_cast<int>(c) == 10;
    return false;
}

inline constexpr bool operator!=(const Color& c, const Background& b) noexcept {
    return !(c == b);
}

inline constexpr bool operator==(const Background& b, const Color& c) noexcept {
    return c == b;
}

inline constexpr bool operator!=(const Background& b, const Color& c) noexcept {
    return c != b;
}

inline constexpr Color same(Background b) noexcept {
    return is_background(b) ? static_cast<Color>(static_cast<int>(b) - 10)
                            : Color::Default;
}

inline constexpr Background same(Color c) noexcept {
    return is_foreground(c) ? static_cast<Background>(static_cast<int>(c) + 10)
                            : Background::Default;
}

inline constexpr Color toggle_brightness(Color c) noexcept {
    if (is_bright(c)) return static_cast<Color>(static_cast<int>(c) - 60);
    if (is_normal(c)) return static_cast<Color>(static_cast<int>(c) + 60);
    return c;
}

inline constexpr Background toggle_brightness(Background b) noexcept {
    if (is_bright(b)) return static_cast<Background>(static_cast<int>(b) - 60);
    if (is_normal(b)) return static_cast<Background>(static_cast<int>(b) + 60);
    return b;
}

enum class Style : uint8_t {
    Reset, Bold, Faint, Italic, Underline, Blink,
    BlinkRapid, Inverse, Hidden, Strikethrough
};

inline constexpr bool is_pad_unsafe_style(Style s) noexcept {
    using enum Style;
    switch (s) {
        case Reset:
        case Underline:
        case Blink:
        case BlinkRapid:
        case Hidden:
        case Strikethrough:
            return true;
        default:
            return false;
    }
}

inline constexpr bool is_trunc_unsafe_style(Style s) noexcept {
    using enum Style;
    switch (s) {
        case Reset:
        case Hidden:
        case Strikethrough:
            return true;
        default:
            return false;
    }
}

/// List of styles (bold, italic, etc.). Maximum 8. Extra styles are ignored.
using styles_t = style_list<Style, 8>;

struct Format {
    Color foreground = Color::Default;
    Background background = Background::Default;
    styles_t styles = {};
    bool use_ansi = true;
    
    [[nodiscard]] constexpr bool has_pad_unsafe_styles() const noexcept {
        return styles.any_of(is_pad_unsafe_style);
    }

    [[nodiscard]] constexpr bool has_trunc_unsafe_styles() const noexcept {
        return styles.any_of(is_trunc_unsafe_style);
    }

    constexpr Format& apply_safe_format_truncate(const Format& other) noexcept {
        background = other.background;
        styles.erase_if(is_trunc_unsafe_style);
        return *this;
    }

    constexpr Format& apply_safe_contrast(Color tty_fg, Background tty_bg,
                                              bool preserve_background) noexcept
    {
        Color fg = (foreground == Color::Default ? tty_fg : foreground);
        Background bg = (background == Background::Default ? tty_bg : background);
        if (fg == bg) {
            if (preserve_background)
                foreground = toggle_brightness(fg);
            else
                background = toggle_brightness(bg);
        }
        return *this;
    }
};

inline os_t& apply_format(os_t& os, const Format& fmt) {
    if (!fmt.use_ansi) return os;
    os << CSI_SEQ << static_cast<int>(fmt.foreground);
    os << PARAM_SEP << static_cast<int>(fmt.background);
    for (Style s : fmt.styles)
        os << PARAM_SEP << static_cast<int>(s);
    return os << SGR_SUFFIX;
}

inline os_t& apply_reset(os_t& os, const Format& fmt) {
    return os << (fmt.use_ansi ? RESET_SEQ : "");
}

template<typename... Args>
inline os_t& apply_sequence(os_t& os, const Args&... args) {
    return (os << ... << args);
}

inline os_t& apply_pad(os_t& os, int width, char padder = ' ') {
	for (int i = 0; i < width; ++i)
		os << padder;
	return os;
}

inline os_t& apply_move_left(os_t& os, int n)  {
    if (n < 1) return os;
    return apply_sequence(os, CSI_SEQ, n, 'D');
}

inline os_t& apply_move_right(os_t& os, int n) {
    if (n < 1) return os;
    return apply_sequence(os, CSI_SEQ, n, 'C');
}

inline os_t& apply_move_up(os_t& os, int n) {
    if (n < 1) return os;
    return apply_sequence(os, CSI_SEQ, n, 'A');
}

inline os_t& apply_move_down(os_t& os, int n) {
    if (n < 1) return os;
    return apply_sequence(os, CSI_SEQ, n, 'B');
}

inline os_t& apply_clear_left(os_t& os, int n) {
    if (n < 1) return os;
    apply_move_left(os, n);
    apply_pad(os, n);
    apply_move_left(os, n);
	return os;
}

} // namespace ansi
