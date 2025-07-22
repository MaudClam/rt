#pragma once
#include <iostream>
#include <sstream>
#include <string_view>
#include <cwchar>       // for mbrtowc, mbstate_t, wchar_t
#include <cstdlib>      // for std::system
#include "traits.hpp"


namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;
using oss_t = std::ostringstream;

struct A {}; // Tag 'A'
struct B {}; // Tag 'B'

template<typename Tag> [[nodiscard]]
oss_t& get_buffer(bool restore = true) {
    static thread_local oss_t oss;
    oss.str(""); // clears the string contents, preserves capacity
    oss.clear(); // clears error flags (failbit, badbit, eofbit)
    if (restore) {
        // clears all formatting flags (fixed, boolalpha, etc.)
        oss.flags(std::ios_base::fmtflags{});
        // resets precision to default (like std::cout)
        oss.precision(6);
    }
    return oss;
}

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
    return is_ascii_control(c) ? '?' : c;
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
    {0x231A,  0x231B,  "watch/timer"},
    {0x23E9,  0x23F3,  "media clocks"},
    {0x25A0,  0x25FF,  "geometric shapes"},
    {0x2600,  0x26FF,  "weather and symbols"},
    {0x2640,  0x2642,  "gender signs"},
    {0x2700,  0x27BF,  "dingbats"},
    {0x1F000, 0x1F02F, "mahjong tiles"},
    {0x1F0A0, 0x1F0FF, "playing cards"},
    {0x1F100, 0x1F1FF, "enclosed alphanumerics + regional"},
    {0x1F200, 0x1F2FF, "CJK symbols"},
    {0x1F300, 0x1F5FF, "misc symbols and pictographs"},
    {0x1F600, 0x1F64F, "emoticons"},
    {0x1F680, 0x1F6FF, "transport and map"},
    {0x1F700, 0x1F77F, "alchemical"},
    {0x1F780, 0x1F7FF, "geometric extended"},
    {0x1F800, 0x1F8FF, "supplemental arrows"},
    {0x1F900, 0x1F9FF, "supplemental symbols and pictographs"},
    {0x1FA00, 0x1FA6F, "chess, axes, etc."},
    {0x1FA70, 0x1FAFF, "extended symbols and pictographs"},
    {0x1FB00, 0x1FBFF, "additional emoji"},
};

struct Codepoint {
    enum class Type : uint8_t {
        Invalid,
        VariationSelector,
        ZeroWidthJoiner,
        RegionalIndicator,
        EmojiModifier,
        EmojiBase,
        Other
    };

    int    width  = 1;
    size_t length = 1;
    Type   type   = Type::Other;

    [[nodiscard]] bool next(sv_t sv, size_t offset) noexcept {
        if (offset >= sv.size()) return false;
        *this = {};
        mbstate_t state{};
        length = mbrtowc(&wchar_, sv.data() + offset, sv.size() - offset, &state);
        if (length != 0 &&
            length != static_cast<size_t>(-1) &&
            length != static_cast<size_t>(-2))
        {
            width = wcwidth(wchar_);
            if (width < 0)
                set(1, length, Type::Invalid);
            else
                classify();
        } else {
            set(1, 1, Type::Invalid);
        }
        return true;
    }

    os_t& write_debug(os_t& os = std::cerr) const noexcept {
        os << "[cp]   U+"
        << std::hex << std::uppercase << std::setw(5) << std::setfill('0')
        << static_cast<uint32_t>(wchar_) << "  "
        << std::dec
        << " l=" << length
        << " w="  << width
        << " " << type_string() << "\n";
        return os;
    }

private:
    const char* type_string() const noexcept {
        switch (type) {
            case Type::Invalid:           return "Invalid";
            case Type::VariationSelector: return "VariationSelector";
            case Type::ZeroWidthJoiner:   return "ZeroWidthJoiner";
            case Type::RegionalIndicator: return "RegionalIndicator";
            case Type::EmojiModifier:     return "EmojiModifier";
            case Type::EmojiBase:         return "EmojiBase";
            case Type::Other:             return "Other";
        }
        return "Unknown";
    }

    void classify() noexcept {
        if (wchar_ >= 0x1F1E6 && wchar_ <= 0x1F1FF) {
            type = Type::RegionalIndicator;
        } else if (wchar_ >= 0x1F3FB && wchar_ <= 0x1F3FF) {
            type = Type::EmojiModifier;
        } else if (is_in_emoji_range()) {
            type = Type::EmojiBase;
        } else if (wchar_ == 0xFE0F) {
            type = Type::VariationSelector;
        } else if (wchar_ == 0x200D) {
            type = Type::ZeroWidthJoiner;
        } else {
            type = Type::Other;
        }
    }

    Codepoint& set(int w, size_t l, Codepoint::Type t) noexcept {
        width = w;
        length = l;
        type = t;
        return *this;
    }

    [[nodiscard]] constexpr bool is_in_emoji_range() const noexcept {
        for (const auto& r : emoji_ranges)
            if (wchar_ >= r.from && wchar_ <= r.to)
                return true;
        return false;
    }

    wchar_t wchar_{};
};

struct DisplayUnit {
    size_t offset = 0;
    size_t length = 0;
    int    width  = 0;
    bool   valid  = true;

    bool in_emoji_unit = false;
    bool pending_emoji = false;
    bool pending_flag  = false;

    [[nodiscard]]
    bool parse(sv_t sv, size_t offset_) noexcept {
        return parse<false>(sv, offset_);
    }

    [[nodiscard]]
    bool parse_debug(sv_t sv, size_t offset_, os_t& os = std::cerr) {
        return parse<true>(sv, offset_, os);
    }

    os_t& write(os_t& os, sv_t sv, char pad = '?') const noexcept {
        if (offset >= sv.size() || offset + length > sv.size())
            return os;
        if (!valid)
            return os << pad;
        return os.write(sv.data() + offset, length);
    }

private:
    [[nodiscard]] bool accept_codepoint(const Codepoint& cp) noexcept {
        const bool first_place    = !length;
        bool       position_valid = false;
        switch (cp.type) {
            case Codepoint::Type::Invalid:
                position_valid = first_place;
                valid          = position_valid ? false : valid;
                break;
            case Codepoint::Type::Other:
                position_valid = first_place;
                width          = position_valid ? cp.width : width;
                break;
            case Codepoint::Type::EmojiBase:
                position_valid = first_place || pending_emoji;
                width          = first_place    ? cp.width : width;
                in_emoji_unit  = position_valid ? true     : in_emoji_unit;
                pending_emoji  = position_valid ? false    : pending_emoji;
                break;
            case Codepoint::Type::VariationSelector:
                position_valid =  in_emoji_unit;
                width          =  position_valid ? 2     : width;
                valid          = !position_valid ? false : valid;
                break;
            case Codepoint::Type::ZeroWidthJoiner:
                position_valid =  in_emoji_unit;
                valid          = !position_valid ? false : valid;
                pending_emoji  =  position_valid ? true  : pending_emoji;
                break;
            case Codepoint::Type::EmojiModifier:
                position_valid = in_emoji_unit;
                width = position_valid ? 2 : (first_place ? cp.width : width);
                break;
            case Codepoint::Type::RegionalIndicator:
                position_valid = first_place || pending_flag;
                width          = position_valid ? 2             : width;
                pending_flag   = position_valid ? !pending_flag : false;
                break;
        }
        return position_valid || first_place;
    }

    os_t& write_debug(os_t& os = std::cerr) const noexcept {
        os << "[unit] offset="  << std::setw(2) << offset
           << " l=" << length
           << " w=" << width
           << " valid=" << std::boolalpha << valid << "\n";
        return os;
    }

    template <bool Debug = false>
    [[nodiscard]] bool parse(sv_t sv, size_t offset_, os_t& os = std::cerr)
                                                      noexcept(Debug == false) {
        if (offset_ >= sv.size()) return false;
        *this = {.offset = offset_};
        Codepoint cp{};
        while (offset + length < sv.size()) {
            if (!cp.next(sv, offset + length)) break;
            if (!accept_codepoint(cp)) break;
            length += std::max<size_t>(1, cp.length);
            if constexpr (Debug) cp.write_debug(os);
            if (!valid) { width = 1; break; }
        }
        if constexpr (Debug) write_debug(os);
        return true;
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
