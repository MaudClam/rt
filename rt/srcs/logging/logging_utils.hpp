#pragma once
#include <string>
#include <string_view>
#include <cwchar>       // for mbrtowc, mbstate_t, wchar_t
#include <cstdlib>      // for std::system
#include <algorithm>    // for std::max
#include "traits.hpp"
#include "ansi_escape_codes.hpp"
#include "../config.hpp"

namespace logging {

using sv_t  = std::string_view;
using oss_t = std::ostringstream;
using os_t  = std::ostream;
using Flags = rt::LoggerStatusFlags;
inline auto& log_config = rt::config;

// logging-specific meta constants for unset/default/special markers
namespace meta {
constexpr int hidden = 0;                   // means "hidden value"
constexpr int unset = -1;                   // means "not specified"
constexpr int utf8_not_initialized = -2;    // for utf8_terminal_width() only
constexpr int utf8_unsupported_sumbol = -3; // for utf8_terminal_width() only
} // namespace meta

/// Estimates terminal column width of a UTF-8 string.
/// Handles emoji clusters, ZWJ, variation selectors, wide characters (CJK).
///
/// Control ASCII characters (`U+0000..001F`, `U+007F`) are ignored (width 0).
/// Other unsupported characters return meta::utf8_unsupported_sumbol.
///
/// Returns:
///   - total width (≥ 0),
///   - meta::utf8_not_initialized if UTF-8 disabled,
///   - meta::utf8_unsupported_sumbol on invalid/unsupported symbols.
///
/// Suitable for in-place terminal formatting. noexcept.
[[nodiscard]] inline
int utf8_terminal_width(sv_t sv) noexcept {
    if (!log_config.utf8_inited)
        return meta::utf8_not_initialized;

    int         width = 0;
    mbstate_t   state{};
    const char* ptr = sv.data();
    size_t      remaining = sv.size();
    wchar_t     wc;
    bool        in_emoji_cluster = false;
    bool        pending_emoji_base = false;
    int         pending_flag = 0;

    while (remaining > 0) {
        size_t len = mbrtowc(&wc, ptr, remaining, &state);
        if (len == 0 || len == static_cast<size_t>(-1) || len == static_cast<size_t>(-2))
            break;

        // FE0F: emoji variation selector
        if (wc == 0xFE0F) {
            if (pending_emoji_base) {
                width += 1;
                pending_emoji_base = false;
            }
            ptr += len; remaining -= len;
            continue;
        }
        // ZWJ: emoji cluster continuation
        if (wc == 0x200D) {
            in_emoji_cluster = true;
            ptr += len; remaining -= len;
            continue;
        }
        // Flags (regional indicator pairs)
        if (wc >= 0x1F1E6 && wc <= 0x1F1FF) {
            if (pending_flag) {
                ++width;
                pending_flag = 0;
            } else {
                pending_flag = 1;
            }
            ptr += len; remaining -= len;
            continue;
        }
        // Check if it's a base emoji
        if ((wc >= 0x2300 && wc <= 0x23FF) ||
            (wc >= 0x2600 && wc <= 0x26FF) ||
            (wc >= 0x1F000 && wc <= 0x1FFFF)) {
            pending_emoji_base = true;
        } else {
            pending_emoji_base = false;
        }
        // Skip if part of emoji cluster
        if (in_emoji_cluster) {
            in_emoji_cluster = false;
            ptr += len; remaining -= len;
            continue;
        }
        int w = wcwidth(wc);
        if (w > 0)
            width += w;
        else if (wc < 0x20 || wc == 0x7F)
            ; // ignore: control ASC
        else
            return meta::utf8_unsupported_sumbol;

        ptr += len; remaining -= len;
    }
    return width;
}

[[nodiscard]] inline
int terminal_width(const sv_t sv) noexcept {
    int ret = utf8_terminal_width(sv);
    if (ret == meta::utf8_not_initialized)
        log_config.set_logger_flag(Flags::Utf8NotInitialized);
    if (ret == meta::utf8_unsupported_sumbol)
        log_config.set_logger_flag(Flags::Utf8UnsupportedSymbol);
    return ret > 0 ? ret : static_cast<int>(sv.size());
}

[[nodiscard]] inline constexpr
sv_t safe_substr(sv_t sv, size_t pos, size_t count = sv_t::npos) noexcept {
    return sv.substr(std::min(pos, sv.size()), count);
}

inline void terminal_clear_fallback() noexcept {
#if defined(_WIN32)
    std::system("cls");
#else
    std::system("clear");
#endif
}

} // namespace logging
