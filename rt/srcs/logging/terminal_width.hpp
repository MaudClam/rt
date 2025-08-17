#pragma once
#include <iostream>
#include <cwchar>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <ostream>
#include <iomanip>


namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;

struct EmojiRange {
    const wchar_t from;
    const wchar_t to;
    const sv_t    name;
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

    os_t& write_debug(os_t& os) const noexcept {
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
    const sv_t type_string() const noexcept {
        using enum Type;
        switch (type) {
            case Invalid:           return "Invalid";
            case VariationSelector: return "VariationSelector";
            case ZeroWidthJoiner:   return "ZeroWidthJoiner";
            case RegionalIndicator: return "RegionalIndicator";
            case EmojiModifier:     return "EmojiModifier";
            case EmojiBase:         return "EmojiBase";
            case Other:             return "Other";
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
    bool parse_debug(sv_t sv, size_t offset_, os_t& os) {
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
        using enum Codepoint::Type;
        switch (cp.type) {
            case Invalid:
                position_valid = first_place;
                valid          = position_valid ? false : valid;
                break;
            case Other:
                position_valid = first_place;
                width          = position_valid ? cp.width : width;
                break;
            case EmojiBase:
                position_valid = first_place || pending_emoji;
                width          = first_place    ? cp.width : width;
                in_emoji_unit  = position_valid ? true     : in_emoji_unit;
                pending_emoji  = position_valid ? false    : pending_emoji;
                break;
            case VariationSelector:
                position_valid =  in_emoji_unit;
                width          =  position_valid ? 2     : width;
                valid          = !position_valid ? false : valid;
                break;
            case ZeroWidthJoiner:
                position_valid =  in_emoji_unit;
                valid          = !position_valid ? false : valid;
                pending_emoji  =  position_valid ? true  : pending_emoji;
                break;
            case EmojiModifier:
                position_valid = in_emoji_unit;
                width = position_valid ? 2 : (first_place ? cp.width : width);
                break;
            case RegionalIndicator:
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
    [[nodiscard]] bool
    parse(sv_t sv, size_t offset_, os_t& os = std::cerr) noexcept(Debug == false)
    {
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

} // namespace logging
