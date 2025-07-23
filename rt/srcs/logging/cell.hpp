#pragma once
#include <sstream>
#include <string_view>
#include <ostream>
#include <iomanip>
#include <algorithm>
#include "traits.hpp"
#include "ansi_escape_codes.hpp"
#include "logging_utils.hpp"
#include "../config.hpp"


namespace logging {

using sv_t = std::string_view;
using os_t = std::ostream;

using Flags             = rt::LoggerStatusFlags;
using LogConfig         = rt::Config;
inline auto& log_config = rt::config;
constexpr int hidden    = 0;  // means "hidden value"
constexpr int unset     = -1; // means "not specified"

struct CellBase {
    [[nodiscard]] int width() const noexcept { return terminal_width_; }

protected:
    mutable int terminal_width_ = unset;
    LogConfig&  cfg_ = log_config;

    [[nodiscard]] bool can_use_tty(bool user_defined = true) const noexcept {
        return user_defined && cfg_.tty_allowed && output_supports_tty(cfg_.output);
    }

    [[nodiscard]] bool can_use_utf8(bool user_defined = true) const noexcept {
        return user_defined && cfg_.utf8_inited;
    }

    [[nodiscard]] bool can_use_ansi(bool user_defined = true) const noexcept {
        return can_use_tty(user_defined) && cfg_.ansi_allowed;
    }

    [[nodiscard]] int term_width(const sv_t& sv) const noexcept {
        if (!can_use_utf8()) {
            if (!is_ascii_only(sv))
                cfg_.set_logger_flag(Flags::Utf8NotInitialized);
            return static_cast<int>(sv.size());
        }
        return utf8_terminal_width(sv);
    }

    os_t& apply_ansi_style(os_t& os, const ansi::Format& style,
               ansi::Format previous_style = {.use_ansi = false}) const noexcept
    {
        if (can_use_ansi()) {
            ansi::write_reset(os, previous_style);
            ansi::write_style(os, style);
        }
        return os;
    }

    os_t& apply_ansi_reset(os_t& os, const ansi::Format& style,
               ansi::Format previous_style = {.use_ansi = false}) const noexcept
    {
        if (can_use_ansi()) {
            ansi::write_reset(os, style);
            ansi::write_style(os, previous_style);
        }
        return os;
    }

    os_t& apply_ansi_clear(os_t& os, int& term_with) const noexcept {
        if (!can_use_ansi())
            return os;
        ansi::write_clear_left(os, std::max(0, term_with));
        terminal_width_ = 0;
        return os;
    }

};

struct IOManip {
    struct [[nodiscard]] StreamGuard {
        os_t& os;
        std::ios_base::fmtflags flags;
        std::streamsize prec;
        StreamGuard(os_t& os_)
            : os(os_), flags(os_.flags()), prec(os_.precision()) {}
        ~StreamGuard() {
            os.flags(flags);
            os.precision(prec);
        }
    };

    int  precision  = unset;
    bool fixed      = false;
    bool scientific = false;
    bool boolalpha  = false;
    enum class Base : uint8_t { Dec, Hex, Oct } base = Base::Dec;
    bool uppercase  = false;
    bool restore_stream_state = true;

    bool modifies_stream() const noexcept {
        return precision >= 0 ||
               fixed || scientific || boolalpha ||
               base != Base::Dec || uppercase;
    }

    template<traits::Ostreamable... Args>
    os_t& apply(os_t& os, const Args&... args) const
                         noexcept(noexcept(traits::write_sequence(os, args...)))
    {
        std::optional<StreamGuard> guard;
        if (restore_stream_state && modifies_stream()) guard.emplace(os);
        if (boolalpha)  os << std::boolalpha;
        if (fixed)      os << std::fixed;
        if (scientific) os << std::scientific;
        switch (base) {
            case Base::Hex: os << std::hex; break;
            case Base::Oct: os << std::oct; break;
            case Base::Dec: os << std::dec; break;
        }
        if (uppercase)      os << std::uppercase;
        if (precision >= 0) os << std::setprecision(precision);
        return traits::write_sequence(os, args...);
    }
};

struct CellFormat {
    struct Align {
        enum class Mode : uint8_t { Left, Right, Centred } mode = Mode::Left;
        char padchar = ' ';
    };

    struct Truncate {
        bool enabled = true;
        int  cutlen  = 3;
        char cutchar = '.';
        ansi::Format style {
            .foreground = ansi::Color::Red,
            .styles     = {ansi::Style::Underline}
        };

        [[nodiscard]] std::pair<int,int> limits(int width) const noexcept {
            width = std::max(0, width);
            const int tail = std::clamp(cutlen, 0, width);
            return { width - tail, tail };
        }
    };

    struct Control {
        enum class Normalize : uint8_t {
            Forbidden, Allowed, Required
        } normalize  = Normalize::Allowed;

        enum class EndPolicy : uint8_t {
            None, Flush, Newline, Pad, PadThenFlush
        } end_policy = EndPolicy::PadThenFlush;

        char padchar  = ' ';
        char normchar = '?';
    };

    int          width = unset;
    Align        align;
    Truncate     truncate;
    IOManip      manip;
    ansi::Format ansi_style;
    Control      control;
    bool         use_utf8 = true;

    [[nodiscard]] bool has_width() const noexcept { return width > 0; }

    [[nodiscard]] bool is_alignable() const noexcept { return has_width(); }

    [[nodiscard]] bool is_truncatable() const noexcept {
        return truncate.enabled && is_alignable();
    }

    [[nodiscard]] bool should_normalize(bool in_case_allowed) const noexcept {
        switch (control.normalize) {
            case Control::Normalize::Required:  return true;
            case Control::Normalize::Allowed:   return in_case_allowed;
            case Control::Normalize::Forbidden: return false;
        }
        return false;
    }

    [[nodiscard]] bool should_be_buffered() const noexcept {
        return is_alignable() ||
               is_truncatable() ||
               should_normalize(false);
    }
};

template<traits::Ostreamable T>
struct Cell : CellBase {
    sv_t prefix = "";
    T    value;
    sv_t suffix = "";
    CellFormat format;

    os_t& write(os_t& os) const noexcept { return write(os, format); }

    os_t& write(os_t& os, const CellFormat& fmt) const noexcept {
        terminal_width_ = 0;
        if (fmt.width == hidden)
            return apply_end(os, fmt, terminal_width_);
        apply_ansi_style(os, fmt.ansi_style);
        std::optional<sv_t> sv_out;
        if (fmt.should_be_buffered()) {
            if (auto sv_raw = buff_raw<A>(fmt.manip, prefix, value, suffix)) {
                if (auto sv_adj = buff_adj<B>(fmt, *sv_raw, terminal_width_))
                    sv_out = *sv_adj;
                else cfg_.set_logger_flag(Flags::LoggingBufferFailed);
            } else cfg_.set_logger_flag(Flags::LoggingBufferFailed);
        }
        if (sv_out) {
            write_aligned(os, fmt, *sv_out, terminal_width_);
        } else {
            terminal_width_ = unset;
            write_with_iomanip(os, fmt.manip, prefix, value, suffix);
        }
        apply_end(os, fmt, terminal_width_);
        return apply_ansi_reset(os, fmt.ansi_style);
    }

    os_t& force_clear(os_t& os) const noexcept {
        if (!format.should_be_buffered()) {
            CellFormat fmt = format;
            fmt.control.normalize = CellFormat::Control::Normalize::Required;
            terminal_width_ = measure_width(fmt);
        }
        return (clear(os));
    }

    os_t& clear(os_t& os) const noexcept { return clear(os, format); }

    os_t& clear(os_t& os, const CellFormat& fmt) const noexcept {
        if (fmt.control.end_policy != CellFormat::Control::EndPolicy::Newline) {
            apply_ansi_clear(os, terminal_width_);
            terminal_width_ = unset;
        }
        return os << std::flush;
    }

    [[nodiscard]] int measure_width() const noexcept { return measure_width(format); }

    [[nodiscard]] int measure_width(const CellFormat& fmt) const noexcept {
        if (auto buff = buffering<A>(fmt))
            return width();
        cfg_.set_logger_flag(Flags::LoggingBufferFailed);
        return unset;
    }

private:
    [[nodiscard]] std::pair<int,int>
    safe_truncat_limits(sv_t sv, const CellFormat& fmt) const noexcept {
        const std::pair<int,int> untruncate{std::numeric_limits<int>::max(), 0};
        if (!fmt.is_truncatable())
           return untruncate;
        const auto [safe_width, tail] = fmt.truncate.limits(fmt.width);
        if (can_use_utf8(fmt.use_utf8)) {
            if (utf8_terminal_width(sv) <= fmt.width)
                return untruncate;
        } else {
            if (is_ascii_only(sv) && static_cast<int>(sv.size()) <= fmt.width)
                return untruncate;
            if (ascii_prefix_length(sv) < safe_width) {
                cfg_.set_logger_flag(Flags::Utf8NotInitialized);
                return untruncate;
            }
      }
       return { safe_width, tail };
    }

    os_t& apply_end(os_t& os, const CellFormat& fmt, int& term_width) const noexcept {
        using EP = CellFormat::Control::EndPolicy;
        switch (fmt.control.end_policy) {
            case EP::None:    break;
            case EP::Flush:   os << std::flush; break;
            case EP::Newline: os << std::endl;  break;
            case EP::Pad: {
                if (!fmt.has_width()) {
                    os << fmt.control.padchar;
                    ++term_width;
                }
                break;
            }
            case EP::PadThenFlush: {
                if (!fmt.has_width()) {
                    os << fmt.control.padchar;
                    ++term_width;
                }
                os << std::flush;
                break;
            }
            default: break;
        }
        return os;
    }

    template<typename Tag, traits::Ostreamable... Args>
    [[nodiscard]] std::optional<sv_t>
    buff_raw(const IOManip& manip, const Args&... args) const noexcept {
        try {
            auto& oss = get_buffer<Tag>(false);
            write_with_iomanip(oss, manip, args...);
            return oss.view();
        } catch (...) {
            return std::nullopt;
        }
    }

    template<typename Tag>
    [[nodiscard]] std::optional<sv_t>
    buff_adj(const CellFormat& fmt, sv_t sv, int& cut_width) const noexcept {
        try {
            auto& oss = get_buffer<Tag>(false);
            write_truncated_and_normalized(oss, fmt, sv, cut_width);
            return oss.view();
        } catch (...) {
            return std::nullopt;
        }
    }

    template<typename Tag>
    [[nodiscard]] std::optional<sv_t>
    buffering(const CellFormat& fmt) const noexcept {
        try {
            auto& oss = get_buffer<Tag>(false);
            write(oss, fmt);
            return oss.view();
        } catch (...) {
            return std::nullopt;
        }
    }

    template<traits::Ostreamable... Args> static
    os_t& write_with_iomanip(os_t& os, const IOManip& manip,const Args&... args)
                                    noexcept(noexcept(manip.apply(os, args...)))
    {
        return manip.apply(os, args...);
    }

    os_t& write_truncated_and_normalized(os_t& os, const CellFormat& fmt,
                                        sv_t sv, int& term_width) const noexcept
    {
        const auto normalize = fmt.should_normalize(true);
        auto [safe_width, tail] = safe_truncat_limits(sv, fmt);
        if (can_use_utf8(fmt.use_utf8)) {
            DisplayUnit unit{};
            size_t offset = 0;
            while (unit.parse(sv, offset) && term_width < safe_width) {
                const bool it_goes_in = (term_width + unit.width <= safe_width);
                //“So it does!” said Pooh.   “It goes in!”
                //“So it does!” said Piglet. “And it comes out!”
                //“Doesn’t it?” said Eeyore. “It goes in and out like anything.”
                if (it_goes_in) {
                    unit.write(os, sv, fmt.control.normchar);
                    term_width += unit.width;
                    offset += unit.length;
                } else {
                    tail += safe_width - term_width;
                    break;
                }
            }
        } else {
            int i = 0, sv_size = static_cast<int>(sv.size());
            for (; i < sv_size && term_width < safe_width; ++i, ++term_width)
                os << (normalize ? normalize_char(sv[i]) : sv[i]);
        }
        if (tail > 0) {
            apply_ansi_style(os, fmt.truncate.style, fmt.ansi_style);
            for (int i = 0; i < tail; ++i, ++term_width)
                os << fmt.truncate.cutchar;
            apply_ansi_reset(os, fmt.truncate.style, fmt.ansi_style);
        }
        return os;
    }

    os_t& write_aligned(os_t& os, const CellFormat& fmt,
                        sv_t sv, int& term_width) const noexcept
    {
        using Mode = CellFormat::Align::Mode;
        int left = 0, right = 0;
        int padd = std::max(0, std::max(0,fmt.width) - std::max(0,term_width));
        switch (fmt.align.mode) {
            case Mode::Left:    left = 0;        right = padd;        break;
            case Mode::Right:   left = padd;     right = 0;           break;
            case Mode::Centred: left = padd / 2; right = padd - left; break;
        }
        ansi::write_pad(os, left, fmt.align.padchar);
        os << sv;
        ansi::write_pad(os, right, fmt.align.padchar);
        term_width += padd;
        return os;
    }
};

template<typename T>
inline os_t& operator<<(os_t& os, const Cell<T>& cell) noexcept {
    return cell.write(os);
}

struct ProgressBarState {
    struct Bar {
    protected:
        mutable int prev_slider  = 0;
        mutable int affix_width  = 0;
        mutable int mark_width   = 0;
        mutable int unmark_width = 0;
    };

    struct Alternate {
    protected:
        mutable int prev_slider  = 0;
        mutable int prefix_width = 0;
        mutable int mark_width   = 0;
        mutable int suffix_width = 0;
    };

    struct Percent {
    protected:
        mutable int prev_percnt = 0;
        mutable int affix_width = 0;
   };

protected:
    mutable int count = 0;
};

struct ProgressBar : CellBase, ProgressBarState {
    enum class Type : uint8_t { Bar, Alternate, Percent };

    struct Bar: ProgressBarState::Bar {
        sv_t prefix = "[";
        sv_t mark   = "#";
        sv_t unmark = " ";
        sv_t suffix = "] ";

        os_t& write(os_t& os, ProgressBar& ctx) noexcept {
            const bool start = ctx.count == 0;
            if (start)
                init(ctx);
            const bool finish   = ctx.count == ctx.cycles;
            const int  slider   = !finish ? ctx.slider_pos() : ctx.width;
            const bool activiry = std::max(0, slider - prev_slider);
            if (!start && !activiry && !finish)
                return os;
            const int unmarks = std::max(0, ctx.width - slider);
            prev_slider = slider;
            ctx.apply_ansi_clear(os, ctx.terminal_width_);
            ctx.apply_ansi_style(os, ctx.style);
            write_content(os, slider, unmarks);
            ctx.apply_ansi_reset(os, ctx.style);
            ctx.terminal_width_ = affix_width
                                + mark_width * slider
                                + mark_width * unmarks;
            if (finish) {
                if (ctx.hide)
                    ctx.apply_ansi_clear(os, ctx.terminal_width_);
                ctx.terminal_width_ = unset;
            }
            return os << std::flush;
        }

    protected:
        Bar& init(const ProgressBar& ctx) noexcept {
            prev_slider  = 0;
            affix_width  = ctx.term_width(prefix) + ctx.term_width(suffix);
            mark_width   = ctx.term_width(mark);
            unmark_width = ctx.term_width(mark);
            return *this;
        }

        os_t& write_content(os_t& os, int marks, int unmarks) const noexcept {
            os << prefix;
            write_repeats(os, marks, mark);
            return write_repeats(os, unmarks, unmark) << suffix;
        }
    };

    struct Alternate: ProgressBarState::Alternate {
        sv_t prefix = "Progress ";
        sv_t mark   = ".";
        sv_t suffix = " ";

        os_t& write(os_t& os, ProgressBar& ctx)  noexcept {
            const bool start = ctx.count == 0;
            if (start)
                init(ctx);
            const bool finish = ctx.count == ctx.cycles;
            const int  slider = !finish ? ctx.slider_pos() : ctx.width;
            const int  step   = std::max(0, slider - prev_slider);
            if (!start && !step && !finish)
                return os;
            prev_slider = slider;
            ctx.apply_ansi_style(os, ctx.style);
            if (start) {
                os << prefix;
                ctx.terminal_width_ = prefix_width;
            }
            write_repeats(os, step, mark);
            ctx.terminal_width_ += (mark_width * step);
            if (finish) {
                os << suffix;
                ctx.terminal_width_ += suffix_width;
                if (ctx.hide)
                    ctx.apply_ansi_clear(os, ctx.terminal_width_);
                ctx.terminal_width_ = unset;
            }
            ctx.apply_ansi_reset(os, ctx.style);
            return os << std::flush;
        }

    protected:
        Alternate& init(const ProgressBar& ctx) noexcept {
            prev_slider  = 0;
            prefix_width = ctx.term_width(prefix);
            mark_width   = ctx.term_width(mark);
            suffix_width = ctx.term_width(suffix);
            return *this;
        }
    };

    struct Percent: ProgressBarState::Percent {
        sv_t prefix = "Progress: ";
        int  width  = 3;
        sv_t suffix = "% ";

        os_t& write(os_t& os, ProgressBar& ctx) noexcept {
            const bool start = ctx.count == 0;
            if (start)
                init(ctx);
            const int  percnt   = ctx.percentage();
            const bool activiry = std::max(0, percnt - prev_percnt);
            const bool finish   = ctx.count == ctx.cycles;
            if (!start && !activiry && !finish)
                return os;
            prev_percnt = percnt;
            ctx.apply_ansi_clear(os, ctx.terminal_width_);
            ctx.apply_ansi_style(os, ctx.style);
            write_content(os, percnt);
            ctx.apply_ansi_reset(os, ctx.style);
            ctx.terminal_width_ = affix_width + width;
            if (finish) {
                if (ctx.hide)
                    ctx.apply_ansi_clear(os, ctx.terminal_width_);
                ctx.terminal_width_ = unset;
            }
            return os << std::flush;
        }

    protected:
        Percent& init(const ProgressBar& ctx) noexcept {
            prev_percnt = 0;
            affix_width = ctx.term_width(prefix) + ctx.term_width(suffix);
            return *this;
        }

        os_t& write_content(os_t& os, int val) const noexcept {
            const int spaces = std::max(3, width)
                             - (val < 10 ? 1 : (val < 100 ? 2 : 3));
            os << prefix;
            return write_repeats(os, std::max(0, spaces), " ") << val << suffix;
        }
    };

    mutable int  cycles = 10;
    Type         type   = Type::Bar;
    bool         hide   = true;
    int          width  = 10;
    ansi::Format style  = { .styles = {ansi::Style::Bold} };
    Bar          bar;
    Alternate    alt;
    Percent      percent;

    os_t& write(os_t& os) noexcept {
        if (cycles <= 0 || width <= 0) return os;
        if (terminal_width_ == unset) {
            terminal_width_ = 0;
            count = 0;
        } else {
            ++count;
        }
        if (!can_use_ansi()) {
            if (can_use_tty())
                alt.write(os, *this);
            return os;
        }
        switch (type) {
            case Type::Bar:       bar.write(os, *this);     break;
            case Type::Alternate: alt.write(os, *this);     break;
            case Type::Percent:   percent.write(os, *this); break;
        }
        return os;
    }

private:
    [[nodiscard]] int slider_pos() const noexcept {
        return bresenham_y(count, cycles, width);
    }

    [[nodiscard]] int percentage() const noexcept {
        return std::clamp(count * 100 / std::max(1, cycles), 0, 100);
    }
};

inline os_t& operator<<(os_t& os, ProgressBar& pbar) {
    pbar.write(os);
    return os;
}

} // namespace logging
