#pragma once
#include <string>
#include <string_view>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <algorithm>
#include "traits.hpp"
#include "ansi_escape_codes.hpp"
#include "logging_utils.hpp"

namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;
using oss_t = std::ostringstream;

enum class Align { Left, Right, Centred };

struct CellFormat {
    struct Alignment {
        Align type = Align::Left;
        int   width = meta::unset;
        char  pad   = ' ';
        bool  truncate_content = false;
    };

    struct Precision {
        int  digits = meta::unset;
        bool fixed  = true;
    };

    Alignment    align;
    Precision    precision;
    ansi::Format ansi_style;
};

template<traits::Ostreamable T>
struct Cell {
    sv_t  prefix = "";
    T     value;
    sv_t  suffix = "";
    CellFormat  fmt;
    mutable int term_width = meta::unset;
    
    os_t& write(os_t& os) const noexcept {
        using namespace ansi;
        using namespace traits;
        
        if (fmt.align.width == meta::hidden) {
            term_width = 0;
            return os;
        }
        
        ansi_style(os, fmt.ansi_style);
        
        // String without alignment — the most frequent case.
        if constexpr (is_any_string_like_v<T>) {
            if (fmt.align.width < 0) {
                term_width = meta::unset;
                sequence(os, prefix, value, suffix);
                return ansi_reset(os, fmt.ansi_style);
            }
        }
        
        // Number or user-defined type without alignment — frequent case.
        if (fmt.align.width < 0) {
            term_width = meta::unset;
            write_content_with_precision(os);
            return ansi_reset(os, fmt.ansi_style);
        }
        
        // Compute terminal width (UTF-8) of the full rendered content.
        try {
            static thread_local oss_t oss;
            oss.str(""); oss.clear();
            write_content_with_precision(oss, false);
            term_width = terminal_width(oss.view());
            write_aligned(os, oss.view(), term_width);//FIXME: ⚠️ do string truncation
        } catch (...) {
            term_width = meta::unset;
            write_content_with_precision(os, false);
        }
        return ansi_reset(os, fmt.ansi_style);
    }

private:

    os_t& write_value_with_precision(os_t& os, bool save_flags = true) const
        noexcept(noexcept(traits::sequence(os, value)))
    {
        using namespace traits;

        if constexpr (is_any_string_like_v<T>)
            return sequence(os, value);

        if (fmt.precision.digits < 0)
            return sequence(os, value);

        if (save_flags) {
            auto flags = os.flags();
            auto prec  = os.precision();

            os << std::setprecision(fmt.precision.digits);
            if (fmt.precision.fixed) os << std::fixed;
            sequence(os, value);

            os.flags(flags);
            os.precision(prec);
            return os;
        }

        os << std::setprecision(fmt.precision.digits);
        if (fmt.precision.fixed) os << std::fixed;
        return sequence(os, value);
    }

    os_t& write_content_with_precision(os_t& os, bool save_flags = true) const noexcept {
        os << prefix;
        write_value_with_precision(os, save_flags);
        return os << suffix;
    }

    os_t& write_aligned(os_t& os, sv_t sv, int term_width) const noexcept {
        int left = 0, right = 0;
        int padd = std::max(fmt.align.width - term_width, 0);
        switch (fmt.align.type) {
            case Align::Left:    left = 0;        right = padd;        break;
            case Align::Right:   left = padd;     right = 0;           break;
            case Align::Centred: left = padd / 2; right = padd - left; break;
        }
        ansi::pad(os, left, fmt.align.pad);
        os << sv;
        ansi::pad(os, right, fmt.align.pad);
        return os;
    }

};

template<typename T>
inline os_t& operator<<(os_t& os, const Cell<T>& cell) noexcept {
    return cell.write(os);
}

struct ProgressBar {
    mutable int slider  = 0;
    int len = 10;
    sv_t prefix = "[";
    sv_t suffix = "]";
    sv_t mark   = "#";
    sv_t unmark = " ";
    bool tty = true;
    mutable int width  = 0;
    ProgressBar() {
        width = terminal_width(prefix) + len * terminal_width(mark)
              + terminal_width(suffix);
    }
    os_t& write(os_t& os, int value) const noexcept {
        if (!tty) return os;
        static bool on = false;
        slider = value;
        if (on)
            ansi::clear_left(os, width);
        else
            on = true;
        if (slider <= len)
            write_bar(os);
        return os;
    }
private:
    void write_bar(os_t& os) const noexcept {
        os << prefix;
        for(int i = 0; i < std::min(slider, len); ++i)
            os << mark;
        for(int i = slider; i < len; ++i)
            os << unmark;
        os << suffix;
    }
};

inline os_t& operator<<(os_t& os, ProgressBar& bar) {
    bar.write(os, bar.slider);
    return os;
}

} // namespace logging
