#pragma once
#include <string_view>
#include <iostream>
#include <ostream>
#include <cwchar>
#include <cstdint>
#include <iomanip>
#include <filesystem>
#include "../common/common.hpp"
#include "logging_utils.hpp"


namespace logging {

using sv_t    = std::string_view;
using os_t    = std::ostream;
namespace fs  = std::filesystem;


//inline constexpr int kHidden = 0;  // means "hidden value"
//inline constexpr int kUnset  = -1; // means "not specified"
//inline constexpr int    kUnlimited     = std::numeric_limits<int>::max();
//inline constexpr size_t Unlimited_size = static_cast<size_t>(kUnlimited);

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
    bool parse_debug(sv_t sv, size_t offset_) {
        return parse<true>(sv, offset_);
    }

    os_t& write(os_t& os, sv_t sv,
                bool normalize = true, char pad = '?') const noexcept {
        if (offset >= sv.size() || offset + length > sv.size())
            return os;
        if (normalize && !valid)
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
    [[nodiscard]] bool parse(sv_t sv, size_t offset_) noexcept(Debug == false) {
        using namespace common;
        if (offset_ >= sv.size()) return false;
        *this = {.offset = offset_};
        Codepoint cp{};
        while (offset + length < sv.size()) {
            if (!cp.next(sv, offset + length)) break;
            if (!accept_codepoint(cp)) break;
            length += std::max<size_t>(1, cp.length);
            if constexpr (Debug)
                stream_locker(std::cerr, stderr_mutex,
                           [this](os_t& os) {this->write_debug(os);});
            if (!valid) { width = 1; break; }
        }
        if constexpr (Debug)
            stream_locker(std::cerr, stderr_mutex,
            [](std::ostream& os, const Codepoint& cp) {cp.write_debug(os);}, cp);
        return true;
    }
};

[[nodiscard]]
inline int utf8_terminal_width(sv_t sv, bool normalize = true) noexcept {
    int width = 0;
    size_t offset = 0;
    DisplayUnit unit{};
    while (unit.parse(sv, offset)) {
        if (!normalize && !unit.valid)
            return kUnset;
        width += unit.width;
        offset += unit.length;
    }
    return width;
}

struct Trimmer {
    enum class Mode : uint8_t { Left, Right };

    Mode mode      = Mode::Right;
    bool use_utf8  = false;
    bool normalize = false;
    int  cutlen    = 3;
    char cutchar   = '.';
    char normchar  = '?';
    mutable bool trimmed = false;
    mutable int  terminal_width = kUnset;

    os_t& apply(os_t& os, sv_t sv, int width = kUnset) const {
        if (width == kHidden || sv.empty()) return os;
        if (width < 0) return apply_visible(os, sv, width);
        if (mode == Mode::Right) {
            apply_visible(os, sv, width);
            return apply_cutchars(os, width);
        }
        auto& oss = common::tl_buffer<Trimmer>(false);
        apply_visible(oss, sv, width);
        apply_cutchars(os, width);
        return os << oss.view();
    }

    os_t& apply_visible(os_t& os, sv_t sv, int width = kUnset) const noexcept {
        if (use_utf8) apply_visible_utf8(os, sv, width);
        else apply_visible_ascii(os, sv, width);
        return os;
    }

    os_t& apply_visible_ascii(os_t& os, sv_t sv,
                              int width = kUnset) const noexcept {
        terminal_width   = 0;
        trimmed          = false;
        const int limit  = calc_limit(width);
        const int size   = to_int_clamped(sv.size());
        bool do_trimming = (!sv.empty() && width > 0 && limit < size);
        if (limit == 0 || sv.empty()) return os;
        bool width_known = true;
        int i = 0;
        if (mode == Mode::Left && do_trimming) {
            const int tail = std::max(0, size - limit);
            sv_t visible   = common::safe_substr(sv, static_cast<size_t>(tail));
            width_known    = normalize
                           ? is_ascii_only(visible)
                           : is_printable_ascii_only(visible);
            if (width_known) {
                i = tail;
                trimmed = true;
            }
            do_trimming = false;
        }
        for (; i < size; ++i) {
            if (do_trimming && terminal_width >= limit) {
                trimmed = true;
                break;
            }
            const char ch = normalize
                          ? normalize_ascii_char(sv[i], normchar)
                          : sv[i];
            os << ch;
            ++terminal_width;
            if (!normalize && width_known && !is_printable_ascii(ch)) {
                width_known = false;
                do_trimming = false;
                trimmed     = false;
            }
        }
        if (!width_known) terminal_width = kUnset;
        return os;
    }

    os_t& apply_visible_utf8(os_t& os, sv_t sv,
                             int width = kUnset) const noexcept {
        terminal_width   = 0;
        trimmed          = false;
        const int limit  = calc_limit(width);
        bool do_trimming = (!sv.empty() && width > 0);
        if (limit == 0 || sv.empty()) return os;
        size_t offset = 0;
        DisplayUnit du{};
        if (mode == Mode::Left && do_trimming) {
            int sv_width = utf8_terminal_width(sv, true);
            int skipped = 0;
            while (limit < sv_width - skipped && du.parse(sv, offset)) {
                offset  += du.length;
                skipped += du.width;
            }
            if (skipped) trimmed = true;
            if (!normalize) {
                sv_t visible = common::safe_substr(sv, offset);
                if (utf8_terminal_width(visible, false) == kUnset) {
                    terminal_width = kUnset;
                    trimmed = false;
                    return os << sv;
                }
            }
            do_trimming = false;
        }
        bool width_known = true;
        while (du.parse(sv, offset)) {
            if (do_trimming && du.width > limit - terminal_width) {
                trimmed = true;
                break;
            }
            du.write(os, sv, normalize, normchar);
            terminal_width += du.width;
            offset += du.length;
            if (!normalize && width_known && !du.valid) {
                width_known = false;
                do_trimming = false;
                trimmed     = false;
            }
        }
        if (!width_known) terminal_width = kUnset;
        return os;
    }

    os_t& apply_cutchars(os_t& os, int width) const noexcept {
        if (width > 0 && terminal_width >= 0 && terminal_width + cutlen <= width)
            if (trimmed || width <= cutlen)
                return write_repeats(os, width - terminal_width, cutchar);
        return os;
    }

    [[nodiscard]] int calc_limit(int width) const noexcept {
        if (width < 0) return kIntMax;
        return width - std::clamp(cutlen, 0, width);
    }
};

template <class Tag = struct tl_default_tag>
[[nodiscard]] inline
sv_t tl_copy(sv_t sv, int width, const Trimmer& trim) noexcept {
    using namespace common;
    try {
        auto& oss = common::tl_buffer<Tag>(false);
        trim.apply(oss, sv, width);
        return oss.view();
    } catch (...) {
        print_error("Error logging::tl_copy():",
                    error("Unable to create temporary copy", sv));
        return {};
    }
}

template <class Tag = struct tl_default_tag>
[[nodiscard]] inline
sv_t tl_copy(sv_t sv, int width = kUnset, bool right = true) noexcept {
    using enum Trimmer::Mode;
    return tl_copy<Tag>(sv, width, { .mode = right ? Right : Left});
}

template <class Tag = struct tl_default_tag>
[[nodiscard]] inline
sv_t tl_copy(const fs::path& path, int width = 32, bool right = false) noexcept {
    using namespace common;
    try {
#if defined(_WIN32)
        auto u8 = path.u8string();
        return
            tl_copy(sv_t{reinterpret_cast<const char*>(u8.c_str()), u8.size()},
                    width, right);
#else
        auto s = path.string();
        return tl_copy<Tag>(sv_t{s.data(), s.size()}, width, right);
#endif
    } catch (...) {
        print_error("Error logging::tl_copy():",
                    error("Unable to convert path", path.native()));
        return {};
    }
}


} // namespace logging
