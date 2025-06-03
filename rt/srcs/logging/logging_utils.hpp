#pragma once
#include <string>   // std::string
#include <cwchar>   // mbrtowc, mbstate_t, wchar_t
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <codecvt>
//#include <locale>
#include <unordered_map>
#include "../config.hpp"
#include "traits.hpp"
#include "ansi_escape_codes.hpp"


namespace logging {

using str_t = std::string;
using strv_t = std::string_view;
using oss_t = std::ostringstream;
using os_t  = std::ostream;
inline auto& log_config = rt::config;

enum class  Level { Info, Warning, Error };
enum class  Align { Left, Right, Centred };
enum class  Label { Error, Warning, Info, Debug, Success };
struct      Range { int from = 0, to = 0; str_t groupe; };

template<typename T>
requires traits::is_ostreamable<T>::value
struct Content {
    strv_t  prefix;
    T       core;
    strv_t  suffix;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Content<T>& c) {
    return os << c.prefix << c.core << c.suffix;
}

struct AlignOpts {
    Align alignment = Align::Left;
    int   width = -1;
    int   precision = -1;
    bool  fixed = false;
    
    struct Offset {
        int left = 0, right = 0;
        int total() const { return left + right; }
    } offset;

    struct Pad {
        char left = ' ';
        char right = ' ';
    } pad;
};

template<typename T>
requires traits::is_ostreamable<T>::value
struct Cell {
    Content<T>   content;
    AlignOpts    align;
    ansi::Format format;
};

struct      LabelFormat {
    struct Entry {
        struct Emoji { bool doublet=false; str_t icon=""; };
        struct Text {
            str_t label;
            Text(const str_t& p, const str_t& w, const str_t& s) {
                label = p + w + s;
            }
        };
        Emoji emoji;
        Text  text;
        ansi::Format fmt;
    };
    std::unordered_map<Label,Entry> entries;
};

constexpr   int utf8_not_initialized = -3;
constexpr   int unsupported_sumbol = -2;
constexpr   int hidden = 0;
constexpr   int without = -1;
//constexpr   Pad DefaultPad{' ', ' '};
//constexpr   AlignOpts DefaultAlign{ without, Align::Left, without, DefaultPad };
//const       LabelFormat DefaultLabels{{
//    { Label::Error,   {{ true, "❌ "}, {"[","Error",  "] "}, {.foreground=ansi::Color::Red,.styles={ansi::Style::Bold}}         }},
//    { Label::Warning, {{false, "⚠️ "}, {"[","Warning","] "}, {.foreground=ansi::Color::BrightYellow}                            }},
//    { Label::Info,    {{false, "ℹ️ "}, {"[","Info",   "] "}, {.foreground=ansi::Color::BrightBlue}                              }},
//    { Label::Debug,   {{false, "🐞 "}, {"[","Debug",  "] "}, {.foreground=ansi::Color::BrightMagenta}                           }},
//    { Label::Success, {{ true, "✅ "}, {"" ,"Success", " "}, {.foreground=ansi::Color::BrightGreen,.styles={ansi::Style::Bold}} }}
//}};
//
//inline os_t& with_label(os_t& os, Label kind, const str_t& prompt = "", const LabelFormat& fmt = DefaultLabels) {
//    auto it = fmt.entries.find(kind);
//    if (it == fmt.entries.end())
//        return os << "[Unknown]";
//    
//    auto entry = it->second;
//    entry.fmt.use_ansi = log_config.ansi_allowed;
//    
//    bool ansi = false;
//    
//    if (log_config.emoji_allowed) {
//        os << entry.emoji.icon;
//        if (entry.emoji.doublet)
//            ansi::with_color(os, entry.text.label, {.use_ansi=log_config.ansi_allowed});
//    } else {
//        ansi::with_color(os, entry.text.label, {.use_ansi=log_config.ansi_allowed});
//        //        ansi::with_color(os, entry.text.label, {entry.fmt});
//    }
//    ansi::with_color(os, prompt, entry.fmt);
//    return os;
//}
//
//inline str_t label(Label kind, const str_t& prompt = "", const LabelFormat& fmt = DefaultLabels) {
//    oss_t oss;
//    with_label(oss, kind, prompt, fmt);
//    return oss.str();
//}

namespace detail {

[[nodiscard]]
inline bool utf8_initialisation() {
	static bool checked_once = false;
	if (!checked_once && !log_config.utf8_inited) {
		log_config.logging_errors += label(Label::Error);
		log_config.logging_errors += "UTF-8 locale is not initialized.\n";
		checked_once = true;
	}
	return log_config.utf8_inited;
}

[[nodiscard]]
inline int utf8_terminal_width(const str_t& str) {
	if (!utf8_initialisation())
		return utf8_not_initialized;

	int         width = 0;
	mbstate_t	state{};
	const char*	ptr = str.c_str();
	wchar_t		wc;
	bool		in_emoji_cluster = false;
	bool		pending_emoji_base = false;	// tracking base emoji before FE0F
	int			pending_flag = 0;			// flags (two letters = 1 flag)

	while (*ptr) {
		size_t len = mbrtowc(&wc, ptr, MB_CUR_MAX, &state);
		if (len == 0 || len == static_cast<size_t>(-1) ||
			len == static_cast<size_t>(-2))
			break;
		//FE0F — if there was a base emoji before this → we consider it as
		// a continuation of the emoji glyph
		if (wc == 0xFE0F) {
			if (pending_emoji_base) {
				width += 1;  // добавляем недостающую ширину
				pending_emoji_base = false;
			}
			ptr += len;
			continue;
		}
		// ZWJ (emoji links, families, professions, etc.)
		if (wc == 0x200D) {
			in_emoji_cluster = true;
			ptr += len;
			continue;
		}
		// Flags of two "Regional Indicator"
		if (wc >= 0x1F1E6 && wc <= 0x1F1FF) {
			if (pending_flag) {
				++width;
				pending_flag = 0;
			} else {
				pending_flag = 1;
			}
			ptr += len;
			continue;
		}
		// Сhecking: was this a base emoji before FE0F?
		if ((wc >= 0x2300 && wc <= 0x23FF) ||  // Misc Technical
			(wc >= 0x2600 && wc <= 0x26FF) ||  // Misc Symbols
			(wc >= 0x1F000 && wc <= 0x1FFFF))  // SMP (emoji and other)
		{
			pending_emoji_base = true;
		} else {
			pending_emoji_base = false;
		}
		// If in a cluster, we do not consider it as a separate width
		if (in_emoji_cluster) {
			in_emoji_cluster = false;
			ptr += len;
			continue;
		}
		int w = wcwidth(wc);
		if (w > 0)
			width += w;
		else
			return unsupported_sumbol;
		ptr += len;
	}
	return width;
}

} // namespace detail

// Converts U+xxxx code to UTF-8 string
inline str_t utf8_from_codepoint(char32_t cp) {
	str_t out;
	if (cp <= 0x7F) {
		out += static_cast<char>(cp);
	} else if (cp <= 0x7FF) {
		out += static_cast<char>(0xC0 | ((cp >> 6) & 0x1F));
		out += static_cast<char>(0x80 | (cp & 0x3F));
	} else if (cp <= 0xFFFF) {
		out += static_cast<char>(0xE0 | ((cp >> 12) & 0x0F));
		out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (cp & 0x3F));
	} else if (cp <= 0x10FFFF) {
		out += static_cast<char>(0xF0 | ((cp >> 18) & 0x07));
		out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
		out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (cp & 0x3F));
	}
	return out;
}

inline void terminal_clear_fallback() {
#if defined(_WIN32)
	std::system("cls");
#else
	std::system("clear");
#endif
}

[[nodiscard]]
inline str_t get_hex_codepoints(const str_t& str) {
    oss_t oss;
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.data());
    size_t len = str.size();
    for (size_t i = 0; i < len;) {
        char32_t cp = 0;
        size_t extra = 0;
        if (bytes[i] <= 0x7F) {
            cp = bytes[i];
            extra = 0;
        } else if ((bytes[i] & 0xE0) == 0xC0) {
            cp = bytes[i] & 0x1F;
            extra = 1;
        } else if ((bytes[i] & 0xF0) == 0xE0) {
            cp = bytes[i] & 0x0F;
            extra = 2;
        } else if ((bytes[i] & 0xF8) == 0xF0) {
            cp = bytes[i] & 0x07;
            extra = 3;
        }
        for (size_t j = 1; j <= extra; ++j)
            cp = (cp << 6) | (bytes[i + j] & 0x3F);
        oss << "U+" << std::hex << std::uppercase << static_cast<uint32_t>(cp);
        i += 1 + extra;
    }
    return oss.str();
}

template<typename... Args>
requires (traits::is_ostreamable<Args>::value && ...)
os_t& compose_into(os_t& os, const Args&... args) {
    (os << ... << args); // Fold expression: os << arg1 << arg2 << ...
    return os;
}

template<typename T>
requires traits::is_ostreamable<T>::value
inline os_t& with_precision(os_t& os, const T& value, int precision) {
    if (precision < 0)
        return os << value;
    std::ios_base::fmtflags original_flags = os.flags();
    std::streamsize original_precision = os.precision();
    os << std::fixed << std::setprecision(precision) << value;
    os.flags(original_flags);
    os.precision(original_precision);
    return os;
}

template<typename T>
requires traits::is_ostreamable<T>::value
[[nodiscard]]
inline str_t precision(const T& value, int precision_) {
    oss_t oss;
    with_precision(oss, value, precision_);
    return oss.str();
}

namespace detail {

[[nodiscard]]
inline Offset calculate_offsets(int width, int len, Align alignment) {
	int left = 0, right = 0;
	switch (alignment) {
		case Align::Left:
			right = std::max(0, width - len);
			break;
		case Align::Right:
			left = std::max(0, width - len);
			break;
		case Align::Centred: {
			int space = std::max(0, width - len);
			left = space / 2;
			right = space - left;
			break;
		}
		default: break;
	}
	return {left, right};
}

template<typename T>
requires traits::is_ostreamable<T>::value
[[nodiscard]]
inline int estimate_terminal_width(const T& value, str_t& value_str, int precision_ = without) {
	int width = 0;
	if constexpr (traits::is_string_like_v<T>)
		value_str = str_t(value);
	else
		value_str = precision(value, precision_);
	width = utf8_terminal_width(value_str);
	return width > 0 ? width : static_cast<int>(value_str.size());
}

} // namespace detail

template<typename T>
requires traits::is_ostreamable<T>::value
inline os_t& with_pad(os_t& os, const T& value, const Padding& padding = {.pad=DefaultPad}) {
    ansi::fill(os, padding.offset.left, padding.pad.left);
    os << value;
    ansi::fill(os, padding.offset.right, padding.pad.right);
    return os;
}

template<typename T>
requires traits::is_ostreamable<T>::value
[[nodiscard]]
inline str_t pad(const T& value, const Padding& padding = {.pad=DefaultPad}) {
    return ansi::fill(padding.offset.left, padding.pad.left)
           + value
           + ansi::fill(padding.offset.right, padding.pad.right);
}

template<typename T>
requires traits::is_ostreamable<T>::value
inline os_t& with_align(os_t& os, const T& value, const AlignOpts& opt = DefaultAlign) {

    constexpr bool is_value_string = traits::is_string_like_v<T>;

    // String without alignment (alias 'without') — the most frequent case.
    if (opt.width < 0 && is_value_string)
        return os << value << opt.pad.right;

    // Number or user-defined type without alignment — almost as frequent case.
    if (opt.width < 0)
        return with_precision(os, value, opt.precision) << opt.pad.right;

    // Used to hide the value (alias 'hidden').
    if (opt.width == 0) return os;

    // Alignment, allow allocation.
    str_t value_str;
    int len = detail::estimate_terminal_width(value, value_str, opt.precision);
	Offset offset = detail::calculate_offsets(opt.width, len, opt.alignment);
    return with_pad(os, value_str, { offset, opt.pad });
}

template<typename T>
requires traits::is_ostreamable<T>::value
[[nodiscard]]
inline str_t align(const T& value, AlignOpts opt = DefaultAlign) {
    oss_t oss;
    with_align(oss, value, opt);
    return oss.str();
}

template<typename Stream>
struct Output {
    Stream& os;

    template<typename T>
    requires traits::is_ostreamable<T>::value
    inline os_t& with_precision(const T& value, int precision) {
        return logging::with_precision(os, value, precision);
    }

    template<typename T>
    requires traits::is_ostreamable<T>::value
    os_t& with_align(const T& value, AlignOpts opt = DefaultAlign) {
        return logging::with_align(os, value, opt);
    }

    template<typename T>
    requires traits::is_ostreamable<T>::value
    inline os_t& with_pad(const T& value, const Padding& padding = {.pad=DefaultPad}) {
        return logging::with_pad(os, value, padding);
    }
    
    template<typename T>
    requires traits::is_ostreamable<T>::value
    inline os_t& with_label(Label kind, const str_t& prompt = "",
                            const LabelFormat& fmt = DefaultLabels) {
        return logging::with_label(os, kind, prompt, fmt);
    }
    
    // The str() method is only available if Stream is an ostringstream
    str_t str() requires std::is_same_v<Stream, std::ostringstream> {
        return os.str();
    }

    template<typename T>
    requires traits::is_ostreamable<T>::value
    Output& operator<<(const T& val) {
        os << val;
        return *this;
    }

    Output& operator<<(os_t& (*manip)(os_t&)) {
        os << manip;
        return *this;
    }

    Output& operator<<(std::ios& (*manip)(std::ios&)) {
        os << manip;
        return *this;
    }

    Output& operator<<(std::ios_base& (*manip)(std::ios_base&)) {
        os << manip;
        return *this;
    }
};

template<typename Stream>
Output<Stream> extend(Stream& os) { return Output<Stream>{os}; }

[[nodiscard]]
constexpr int get_percent(int idx, int cycles) {
    idx = std::abs(idx);
    cycles = std::abs(cycles);
    if (cycles == 1)
        return 100;
    else if (idx == cycles - 1)
        return 100;
	return idx * 100 / cycles;
}

//// FIXME: unfinished utility
//inline void progress(os_t& os, int idx, int cycles,
//					 const str_t& prefix = "started") {
//	static int counter;
//	static int displayed;
//	static bool started;
//	static const bool is_tty	= [] { detect_is_terminal(); return true; }();
//	static const bool is_ansi	= rt::config.ansi_allowed &&
//								[] { detect_ansi_support(); return true; }();
//	if (!is_tty) return;
//	int percent = get_percent(idx, cycles);
//	if (!is_ansi) {
//		if (percent == -100) {
//			percent = -percent;
//			os << prefix << std::flush;
//			started = true;
//		} else if (percent == 0) {
//			counter = 0;
//			if (!started) {
//				os << prefix << std::flush;
//				started = true;
//			}
//		}
//		if (percent >= 100) started = false;
//		int expected_dots = std::min(10, percent * 10 / 100);
//		while (percent && counter < expected_dots) {
//			os << "." << std::flush;
//			++counter;
//		}
//	} else {
////		if (displayed && (percent >= 100 || percent == 0) ) {
////			os << ansi_clear_left(displayed) << std::flush;
////			displayed = 0;
////			return;
////		}
//		oss_t oss;
//		oss << prefix << std::setw(3) << std::right << percent << "%";
//		os << ansi::clear_left(displayed) << std::flush;
//		os << oss.str() << std::flush;
//		if (displayed && (percent >= 100 || percent == 0) ) {
//			os << ansi::clear_left(displayed) << std::flush;
//			displayed = 0;
//			return;
//		}
////		displayed = utf8_terminal_width(oss.str());
//	}
//}

std::vector<Range> emoji_ranges = {
    {0x1F600, 0x1F64F, "faces"},
    {0x1F300, 0x1F5FF, "symbols"},
    {0x1F680, 0x1F6FF, "transport"},
    {0x1F900, 0x1F9FF, "extension"},
    {0x2600,  0x26FF, "weather and stuff"},
    {0x2700,  0x27BF, "miscellaneous"}
};

[[nodiscard]]
inline str_t get_random_emoji() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    // random range
    std::uniform_int_distribution<> dist_range(0, int(emoji_ranges.size()) - 1);
    Range r = emoji_ranges[dist_range(gen)];
    // random code in range
    std::uniform_int_distribution<> dist_cp(r.from, r.to);
    return utf8_from_codepoint(dist_cp(gen));
}

} // namespace logging
