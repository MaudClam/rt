#pragma once
#include <string_view>
#include <cwchar>       // for mbrtowc, mbstate_t, wchar_t
#include <cstdlib>      // for std::system
#include <optional>

namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;

[[nodiscard]] constexpr
bool is_ascii(char c) noexcept {
    return static_cast<unsigned char>(c) < 0x80;
}

[[nodiscard]] inline
int ascii_prefix_length(sv_t sv) noexcept {
    for (int i = 0, sz = static_cast<int>(sv.size()); i < sz; ++i)
        if (!is_ascii(sv[i]))
            return i;
    return static_cast<int>(sv.size());
}

[[nodiscard]] inline
bool is_ascii_only(sv_t sv) noexcept {
    return ascii_prefix_length(sv) == static_cast<int>(sv.size());
}

[[nodiscard]] constexpr
bool is_ascii_control(char c) noexcept {
    return static_cast<unsigned char>(c) < 0x20 || c == 0x7F;
}

[[nodiscard]] constexpr
char normalize_char(char c) noexcept {
    return is_ascii_control(c) ? ' ' : c;
}

template<traits::Ostreamable Os>
os_t& write_repeats(os_t& os, int num, const Os& val) noexcept {
    for(int i = 0; i < num; ++i)
        os << val;
    return os;
}

struct EmojiRange {
    wchar_t from;
    wchar_t to;
    const char* name;
};

constexpr EmojiRange emoji_ranges[] = {
    {0x1F600, 0x1F64F, "faces"},
    {0x1F300, 0x1F5FF, "symbols"},
    {0x1F680, 0x1F6FF, "transport"},
    {0x1F900, 0x1F9FF, "extensions"},
    {0x2600,  0x26FF,  "weather"},
    {0x2700,  0x27BF,  "miscellaneous"},
    {0x1F000, 0x1FFFF, "supplemental symbols"}
};

struct Codepoint {
    wchar_t value  = static_cast<wchar_t>('?');
    int     width  = 1;
    size_t  length = 1;
    
    enum class Type : uint8_t {
        Invalid,
        Ascii,
        VariationSelector,
        ZeroWidthJoiner,
        RegionalIndicator,
        EmojiBase,
        Other
    } type = Type::Other;

    [[nodiscard]] bool next(sv_t sv, size_t offset) noexcept {
        if (offset >= sv.size()) return false;
        *this = {};
        mbstate_t state{};
        length = mbrtowc(&value, sv.data() + offset, sv.size() - offset, &state);
        if (!sanitize()) {
            width = wcwidth(value);
            if (!sanitize())
                classify();
        }
        return true;
    }

private:
    void classify() noexcept {
        if (value <= 0x7F) {
            type = Type::Ascii;
        } else if (value == 0xFE0F) {
            type = Type::VariationSelector;
        } else if (value == 0x200D) {
            type = Type::ZeroWidthJoiner;
        } else if (value >= 0x1F1E6 && value <= 0x1F1FF) {
            type = Type::RegionalIndicator;
        } else if (is_in_emoji_range()) {
            type = Type::EmojiBase;
        } else {
            type = Type::Other;
        }
    }

    [[nodiscard]] constexpr bool is_invalid() const noexcept {
        return width < 0 ||
               length ==  0 ||
               length == static_cast<size_t>(-1) ||
               length == static_cast<size_t>(-2);
    }

    [[nodiscard]] constexpr bool sanitize() noexcept {
        if (!is_invalid()) return false;
        *this = {.type = Type::Invalid};
        return true;
    }

    [[nodiscard]] constexpr bool is_in_emoji_range() const noexcept {
        for (const auto& r : emoji_ranges)
            if (value >= r.from && value <= r.to)
                return true;
        return false;
    }
};

struct DisplayUnit {
    Codepoint codepoint{};
    size_t    offset = 0;
    size_t    length = 0;
    int       width  = 0;
    
    enum class Type : uint8_t {
        Invalid,
        Ascii,
        EmojiCluster,
        RegionalPair,
        PlainText,
        Unknown
    } type = Type::Unknown;
    
    [[nodiscard]] bool parse(sv_t sv, size_t offset_) noexcept {
        if (offset_ >= sv.size()) return false;
        *this = {.offset = offset_};
        bool in_emoji_cluster   = false;
        bool pending_flag       = false;
        bool pending_emoji_base = false;
        while (offset + length < sv.size()) {
            if (!codepoint.next(sv, offset + length)) break;
            switch (codepoint.type) {
                case Codepoint::Type::Invalid:
                    type = Type::Invalid;
                    width = 1;
                    length += codepoint.length;
                    return true;
                case Codepoint::Type::Ascii:
                    type = Type::Ascii;
                    width = 1;
                    length += codepoint.length;
                    return true;
                case Codepoint::Type::VariationSelector:
                    if (pending_emoji_base) {
                        pending_emoji_base = false;
                        width = 1;
                    }
                    length += codepoint.length;
                    continue;
                case Codepoint::Type::ZeroWidthJoiner:
                    in_emoji_cluster = true;
                    length += codepoint.length;
                    continue;
                case Codepoint::Type::RegionalIndicator:
                    if (pending_flag) {
                        pending_flag = false;
                        type = Type::RegionalPair;
                        width = 2;
                        length += codepoint.length;
                        return true;
                    } else {
                        pending_flag = true;
                        width = 0;
                        length += codepoint.length;
                        continue;
                    }
                case Codepoint::Type::EmojiBase:
                    pending_emoji_base = true;
                    if (in_emoji_cluster) {
                        in_emoji_cluster = false;
                        type = Type::EmojiCluster;
                        width = 0;
                    } else {
                        width += (codepoint.width > 0 ? codepoint.width : 1);
                    }
                    length += codepoint.length;
                    return true;
                case Codepoint::Type::Other:
                    type = Type::PlainText;
                    width += (codepoint.width > 0 ? codepoint.width : 1);
                    length += codepoint.length;
                    return true;
            }
        }
        if (pending_flag) {
            type = Type::RegionalPair;
            ++width;
        } else if (pending_emoji_base) {
            type = Type::EmojiCluster;
            width = 1;
        } else if (width == 0) {
            type = Type::Invalid;
            width = 1;
        }
        return true;
    }
    
    os_t& write(os_t& os, sv_t sv, bool normalize = false) const noexcept {
        if (offset >= sv.size() || offset + length > sv.size())
            return os;
        if (type == Type::Invalid)
            return os << static_cast<unsigned char>(codepoint.value);
        if (normalize && type == Type::Ascii)
            return os << normalize_char(static_cast<char>(codepoint.value));
        return os.write(sv.data() + offset, length);
    }
};

[[nodiscard]] inline int utf8_terminal_width(sv_t sv) noexcept {
    int width = 0;
    size_t offset = 0;
    DisplayUnit unit{};
    while (unit.parse(sv, offset)) {
        width += unit.width;
        offset += unit.length;
    }
    return width;
}

[[nodiscard]] inline int bresenham_y(int x, int dx, int dy) noexcept {
    if (x < 0 || dx <= 0 || dy <= 0) return 0;
    bool steep = dx < dy;
    if (steep) std::swap(dx, dy);
    int derror2 = dy * 2;
    int error2 = 0;
    for (int x0 = 0, y0 = 0; x0 < dx; ++x0) {
        if (!steep && x0 == x) return y0;
        if ( steep && y0 == x) return x0;
        error2 += derror2;
        if (error2 > dx) {
            ++y0;
            error2 -= dx * 2;
        }
    }
    return dy;
}

inline void terminal_clear_fallback() noexcept {
#if defined(_WIN32)
    std::system("cls");
#else
    std::system("clear");
#endif
}

} // namespace logging
