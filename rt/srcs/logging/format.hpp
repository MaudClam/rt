#pragma once
#include "../config.hpp"
#include <string_view>
#include <ostream>
#include "ansi_escape_codes.hpp"
#include "logging_utils.hpp"
#include "terminal_width.hpp"

namespace logging {

using sv_t = std::string_view;
using os_t = std::ostream;

inline constexpr ansi::Format kNoAnsi{ .use_ansi = false };

struct A {}; // BufferTag 'A'
struct B {}; // BufferTag 'B'

struct FormatBase {
    [[nodiscard]] int width() const noexcept { return terminal_width_; }

    [[nodiscard]] ansi::Color tty_fg() const noexcept {
        return cfg().tty_foreground;
    }

    [[nodiscard]] ansi::Background tty_bg() const noexcept {
        return cfg().tty_background;
    }

protected:
    mutable int terminal_width_ = kUnset;
    mutable io::Output    mode_ = rt::config.log_out;
    rt::Config*            cfg_ = &rt::config;

    [[nodiscard]] const rt::Config& cfg() const noexcept {
        assert(cfg_ && "Internal error: cfg_ should never be null");
        return *cfg_;
    }

    [[nodiscard]] rt::Config& cfg() noexcept {
        assert(cfg_ && "Internal error: cfg_ should never be null");
        return *cfg_;
    }
    
    [[nodiscard]] io::Output& output_mode() const noexcept { return mode_; }

    [[nodiscard]] bool can_use_tty(bool user_defined = true) const noexcept {
        return user_defined &&
               out_supports_tty(mode_) &&
               cfg().tty_allowed;
    }

    [[nodiscard]] bool can_use_utf8(bool user_defined = true) const noexcept {
        return user_defined && cfg().utf8_inited;
    }

    [[nodiscard]] bool can_use_emoji(bool user_defined = true) const noexcept {
        return user_defined && cfg().emoji_allowed;
    }

    [[nodiscard]] bool can_use_ansi(bool user_defined = true) const noexcept {
        return can_use_tty(user_defined) && cfg().ansi_allowed;
    }

    [[nodiscard]] int term_width(const sv_t& sv) const noexcept {
        if (!can_use_utf8()) {
            if (!is_ascii_only(sv))
                cfg().log_warns.set(Warn::Utf8NotInitialized);
            return to_int_clamped(sv.size());
        }
        return utf8_terminal_width(sv);
    }

    os_t& apply_ansi_format(os_t& os, const ansi::Format& style,
                      const ansi::Format& prev_style = kNoAnsi) const noexcept {
        if (!can_use_ansi()) return os;
        ansi::apply_reset(os, prev_style);
        ansi::apply_format(os, style);
        return os;
    }

    os_t& apply_ansi_reset(os_t& os, const ansi::Format& style,
                      const ansi::Format& prev_style = kNoAnsi) const noexcept {
        if (!can_use_ansi()) return os;
        ansi::apply_reset(os, style);
        ansi::apply_format(os, prev_style);
        return os;
    }

    os_t& apply_ansi_clear(os_t& os, int& term_with) const noexcept {
        if (!can_use_ansi()) return os;
        ansi::apply_clear_left(os, std::max(0, term_with));
        terminal_width_ = 0;
        return os;
    }
};

struct IOManip {
    struct [[nodiscard]] StreamGuard {
        std::ostream&           os;
        std::ios_base::fmtflags flags;
        std::streamsize         prec;

        StreamGuard(os_t& os_) :
            os(os_),
            flags(os_.flags()),
            prec(os_.precision()) {}

        ~StreamGuard() {
            os.flags(flags);
            os.precision(prec);
        }
    };

    int  precision  = kUnset;
    bool fixed      = false;
    bool scientific = false;
    bool boolalpha  = false;
    enum class Base : uint8_t { Dec, Hex, Oct }
         base       = Base::Dec;
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
        using enum Base;
        switch (base) {
            case Hex: os << std::hex; break;
            case Oct: os << std::oct; break;
            case Dec: os << std::dec; break;
        }
        if (uppercase)      os << std::uppercase;
        if (precision >= 0) os << std::setprecision(precision);
        return traits::write_sequence(os, args...);
    }
};

struct Format : FormatBase {
    struct Align {
        enum class Mode : uint8_t {
            Left,
            Right,
            Centred
        };

        Mode mode    = Mode::Left;
        char padchar = ' ';

        [[nodiscard]] constexpr
        std::pair<int,int> padding(int width, int text_width) const noexcept {
            return padding(std::max(0, width) - std::max(0, text_width));
        }

        [[nodiscard]] constexpr
        std::pair<int,int> padding(int pad) const noexcept {
            if (pad <= 0) return {0,0};
            int left = 0, right = 0;
            using enum Mode;
            switch (mode) {
                case Left:    left = 0;       right = pad;        break;
                case Right:   left = pad;     right = 0;          break;
                case Centred: left = pad / 2; right = pad - left; break;
            }
            return { left, right };
        }
    };

    struct Truncate {
        bool         enabled = true;
        int          cutlen  = 3;
        char         cutchar = '.';
        ansi::Format ansi_format{
                         .foreground = ansi::Color::BrightRed,
                         .styles{
                             ansi::Style::Underline,
                             ansi::Style::Blink,
                     }
    };

        [[nodiscard]] std::pair<int,int> limits(int width) const noexcept {
            width = std::max(0, width);
            const int tail = std::clamp(cutlen, 0, width);
            return { width - tail, tail };
        }
    };

    struct Control {
        enum class Normalize : uint8_t {
            Forbidden,
            Allowed,
            Required
        };
        enum class EndPolicy : uint8_t {None,
            Flush,
            Newline,
            Pad,
            PadThenFlush
        };

        EndPolicy end_policy = EndPolicy::PadThenFlush;
        Normalize normalize  = Normalize::Allowed;
        char      padchar    = ' ';
        char      normchar   = '?';
    };

    int          width = kUnset;
    Align        align;
    Truncate     truncate;
    IOManip      manip;
    ansi::Format ansi_format;
    Control      control;
    bool         use_utf8  = true;
    bool         use_emoji = true;
    bool         preserve_background = true;

    [[nodiscard]]
    bool should_emoji() const noexcept { return can_use_emoji(use_emoji); }

    [[nodiscard]]
    bool is_unlimited_width() const noexcept { return width == kUnset; }

    [[nodiscard]]
    bool is_hidden_width() const noexcept { return width == kHidden; }

    [[nodiscard]]
    bool has_width() const noexcept { return width > 0; }

    [[nodiscard]]
    bool is_alignable() const noexcept { return has_width(); }

    [[nodiscard]]
    bool is_truncatable() const noexcept {
        return truncate.enabled && is_alignable();
    }

    [[nodiscard]]
    bool should_normalize(bool in_case_allowed) const noexcept {
        using enum Control::Normalize;
        switch (control.normalize) {
            case Required:  return true;
            case Allowed:   return in_case_allowed;
            case Forbidden: return false;
        }
        return false;
    }

    [[nodiscard]]
    bool should_be_buffered() const noexcept {
        return is_alignable() ||
               is_truncatable() ||
               should_normalize(false);
    }

    [[nodiscard]]
    bool should_sanitize_pad_style() const noexcept {
        return can_use_ansi(ansi_format.use_ansi) &&
               ansi_format.has_pad_unsafe_styles();
    }

    [[nodiscard]] constexpr
    bool should_use_trunc_ansi_format(bool user_defined = true) const noexcept {
        return user_defined && can_use_ansi(truncate.ansi_format.use_ansi);
    }

    template<traits::Ostreamable... Args>
    os_t& apply(io::Output mode, os_t& os, const Args&... args) const noexcept {
        common::ScopedOverride scoped(output_mode(), mode);
        return apply(os, args...);
    }

    template<traits::Ostreamable... Args>
    os_t& apply(os_t& os, const Args&... args) const noexcept {
        terminal_width_ = 0;
        if (is_hidden_width())
            return apply_end(os);
        std::optional<sv_t> sv_out;
        if (should_be_buffered()) {
            if (auto sv_raw = buff_raw<A>(manip, args...)) {
                if (auto sv_adj = buff_adj<B>(*sv_raw))
                    sv_out = *sv_adj;
                else cfg().log_warns.set(Warn::LoggingBufferFailed);
            } else cfg().log_warns.set(Warn::LoggingBufferFailed);
        }
        if (sv_out) {
            apply_align(os, *sv_out);
        } else {
            terminal_width_ = kUnset;
            apply_iomanip(os, manip, args...);
        }
        return apply_end(os);
    }

    // Buffering is active when any of the following conditions is true:
    // A fixed width is specified;
    // Alignment is enabled;
    // Truncation is enabled and applicable;
    // Normalization is explicitly requested or allowed in this context.
    os_t& apply_clear(os_t& os) const noexcept {
        assert(
            control.end_policy != Control::EndPolicy::Newline &&
            should_be_buffered() &&
            "Format::clear() is valid only with buffering and without newline policy"
        );
        apply_ansi_clear(os, terminal_width_);
        terminal_width_ = kUnset;
        return os << std::flush;
    }

private:
    [[nodiscard]] constexpr
    ansi::Format safe_contrast_ansi_format() const noexcept {
        return ansi::Format{ansi_format}.apply_safe_contrast(tty_fg(), tty_bg(),
                                                           preserve_background);
    }

    [[nodiscard]] constexpr
    ansi::Format safe_pad_ansi_format(ansi::Format safe) const noexcept {
        safe.styles.erase_if(ansi::is_pad_unsafe_style);
        return safe;
    }

    [[nodiscard]] constexpr
    ansi::Format safe_pad_ansi_format() const noexcept {
        auto safe = safe_contrast_ansi_format();
        safe.styles.erase_if(ansi::is_pad_unsafe_style);
        return safe;
    }

    [[nodiscard]] constexpr
    ansi::Format safe_truncate_ansi_format() const noexcept {
        auto safe = truncate.ansi_format;
        if (ansi_format.use_ansi) {
            if (preserve_background) safe.background = ansi_format.background;
            else safe.background = safe_contrast_ansi_format().background;
        }
        safe.styles.erase_if(ansi::is_trunc_unsafe_style);
        safe.apply_safe_contrast(tty_fg(), tty_bg(), preserve_background);
        return safe;
    }

    [[nodiscard]]
    std::pair<int,int> safe_truncate_limits(sv_t sv) const noexcept {
    const std::pair<int,int> untruncate{std::numeric_limits<int>::max(), 0};
    if (!is_truncatable())
        return untruncate;
    const auto [safe_width, tail] = truncate.limits(width);
    if (can_use_utf8(use_utf8)) {
        if (utf8_terminal_width(sv) <= width)
            return untruncate;
    } else {
        if (is_ascii_only(sv) && to_int_clamped(sv.size()) <= width)
            return untruncate;
        if (ascii_prefix_length(sv) < safe_width) {
            cfg().log_warns.set(Warn::Utf8NotInitialized);
            return untruncate;
        }
    }
    return { safe_width, tail };
}

    os_t& apply_padding(os_t& os, int pad, char padder = ' ') const noexcept {
        terminal_width_ += pad;
        return ansi::apply_pad(os, pad, padder);
    }

    os_t& apply_end(os_t& os) const noexcept {
        bool pad = false;
        bool flush = false;
        bool newline = false;
        using enum Control::EndPolicy;
        switch (control.end_policy) {
            case None:         break;
            case Flush:        flush = true; break;
            case Newline:      newline = true; break;
            case Pad:          pad = is_unlimited_width(); break;
            case PadThenFlush: pad = is_unlimited_width(); flush = true; break;
        }
        if (pad) {
            bool use_ansi = can_use_ansi(ansi_format.use_ansi);
            if (use_ansi) ansi::apply_format(os, safe_pad_ansi_format());
            apply_padding(os, 1, align.padchar);
            if (use_ansi) ansi::apply_reset(os, ansi_format);
        }
        if (newline) return os << std::endl;
        if (flush) os << std::flush;
        return os;
    }

    template<typename BufferTag, traits::Ostreamable... Args>
    [[nodiscard]] std::optional<sv_t>
    buff_raw(const IOManip& manip, const Args&... args) const noexcept {
        try {
            auto& oss = common::tl_buffer<BufferTag>(false);
            manip.apply(oss, args...);
            return oss.view();
        } catch (...) {
            return std::nullopt;
        }
    }

    template<typename BufferTag>
    [[nodiscard]] std::optional<sv_t>
    buff_adj(sv_t sv) const noexcept {
        try {
            auto& oss = common::tl_buffer<BufferTag>(false);
            write_truncated_and_normalized(oss, sv);
            return oss.view();
        } catch (...) {
            return std::nullopt;
        }
    }

    template<traits::Ostreamable... Args>
    os_t& apply_iomanip(os_t& os, const IOManip& manip, const Args&... args)
                              const noexcept(noexcept(manip.apply(os, args...)))
    {
        const bool use_ansi = can_use_ansi(ansi_format.use_ansi);
        if (use_ansi) ansi::apply_format(os, safe_contrast_ansi_format());
        manip.apply(os, args...);
        if (use_ansi) ansi::apply_reset(os, ansi_format);
        return os;
    }

    os_t& write_truncated_and_normalized(os_t& os, sv_t sv) const noexcept {
        auto [safe_width, tail] = safe_truncate_limits(sv);
        if (can_use_utf8(use_utf8)) {
            DisplayUnit unit{};
            size_t offset = 0;
            while (unit.parse(sv, offset) && terminal_width_ < safe_width) {
                const bool it_goes_in =
                    (terminal_width_ + unit.width <= safe_width);
                //“So it does!” said Pooh.   “It goes in!”
                //“So it does!” said Piglet. “And it comes out!”
                //“Doesn’t it?” said Eeyore. “It goes in and out like anything.”
                if (it_goes_in) {
                    unit.write(os, sv, control.normchar);
                    terminal_width_ += unit.width;
                    offset += unit.length;
                } else {
                    tail += safe_width - terminal_width_;
                    break;
                }
            }
        } else {
            const auto normalize = should_normalize(true);
            int i = 0, sv_size = to_int_clamped(sv.size());
            for (; i < sv_size && terminal_width_ < safe_width; ++i) {
                os << (normalize ? normalize_ascii_char(sv[i]) : sv[i]);
                ++terminal_width_;
            }
        }
        const bool use_ansi = should_use_trunc_ansi_format(tail > 0);
        if (use_ansi) ansi::apply_format(os, safe_truncate_ansi_format());
        apply_padding(os, tail, truncate.cutchar);
        if (use_ansi && !ansi_format.use_ansi)
            ansi::apply_reset(os, truncate.ansi_format);
        return os;
    }

    os_t& apply_align(os_t& os, sv_t sv) const noexcept {
        const auto [left, right] = align.padding(width, terminal_width_);
        if (!can_use_ansi(ansi_format.use_ansi)) {
            apply_padding(os, left, align.padchar);
            os << sv;
            return apply_padding(os, right, align.padchar);
        }
        if ((!left && !right) || !ansi_format.has_pad_unsafe_styles()) {
            ansi::apply_format(os, safe_contrast_ansi_format());
            apply_padding(os, left, align.padchar);
            os << sv;
            apply_padding(os, right, align.padchar);
            return ansi::apply_reset(os, ansi_format);
        }
        const auto safe_content = safe_contrast_ansi_format();
        const auto safe_pad     = safe_pad_ansi_format(safe_content);
        if (left) {
            ansi::apply_format(os, safe_pad);
            apply_padding(os, left, align.padchar);
            ansi::apply_reset(os, safe_pad);
        }
        ansi::apply_format(os, safe_content);
        os << sv;
        ansi::apply_reset(os, safe_content);
        if (right) {
            ansi::apply_format(os, safe_pad);
            apply_padding(os, right, align.padchar);
            ansi::apply_reset(os, safe_pad);
        }
        return os;
    }
};

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

struct ProgressBar : Format, ProgressBarState {
    enum class Type : uint8_t { Bar, Alternate, Percent };

    struct Bar: ProgressBarState::Bar {
        sv_t prefix = "[";
        sv_t mark   = "#";
        sv_t unmark = " ";
        sv_t suffix = "] ";

        os_t& write(os_t& os, const ProgressBar& ctx) const noexcept {
            const bool start = ctx.count == 0;
            if (start)
                init(ctx);
            const bool finish   = ctx.count == ctx.cycles;
            const int  slider   = !finish ? ctx.slider_pos() : ctx.width;
            const bool activity = std::max(0, slider - prev_slider);
            if (!start && !activity && !finish)
                return os;
            const int unmarks = std::max(0, ctx.width - slider);
            prev_slider = slider;
            ctx.apply_ansi_clear(os, ctx.terminal_width_);
            ctx.apply_ansi_format(os, ctx.style);
            write_content(os, slider, unmarks);
            ctx.apply_ansi_reset(os, ctx.style);
            ctx.terminal_width_ = affix_width
                                + mark_width * slider
                                + mark_width * unmarks;
            if (finish) {
                if (ctx.hide)
                    ctx.apply_ansi_clear(os, ctx.terminal_width_);
                ctx.terminal_width_ = kUnset;
            }
            return os << std::flush;
        }

    protected:
        void init(const ProgressBar& ctx) const noexcept {
            prev_slider  = 0;
            affix_width  = ctx.term_width(prefix) + ctx.term_width(suffix);
            mark_width   = ctx.term_width(mark);
            unmark_width = ctx.term_width(unmark);
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

        os_t& write(os_t& os, const ProgressBar& ctx) const noexcept {
            const bool start = ctx.count == 0;
            if (start)
                init(ctx);
            const bool finish = ctx.count == ctx.cycles;
            const int  slider = !finish ? ctx.slider_pos() : ctx.width;
            const int  step   = std::max(0, slider - prev_slider);
            if (!start && !step && !finish)
                return os;
            prev_slider = slider;
            ctx.apply_ansi_format(os, ctx.style);
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
                ctx.terminal_width_ = kUnset;
            }
            ctx.apply_ansi_reset(os, ctx.style);
            return os << std::flush;
        }

    protected:
        void init(const ProgressBar& ctx) const noexcept {
            prev_slider  = 0;
            prefix_width = ctx.term_width(prefix);
            mark_width   = ctx.term_width(mark);
            suffix_width = ctx.term_width(suffix);
        }
    };

    struct Percent: ProgressBarState::Percent {
        sv_t prefix = "Progress: ";
        int  width  = 3;
        sv_t suffix = "% ";

        os_t& write(os_t& os, const ProgressBar& ctx) const noexcept {
            const bool start = ctx.count == 0;
            if (start)
                init(ctx);
            const int  percnt   = ctx.percentage();
            const bool activity = std::max(0, percnt - prev_percnt);
            const bool finish   = ctx.count == ctx.cycles;
            if (!start && !activity && !finish)
                return os;
            prev_percnt = percnt;
            ctx.apply_ansi_clear(os, ctx.terminal_width_);
            ctx.apply_ansi_format(os, ctx.style);
            write_content(os, percnt);
            ctx.apply_ansi_reset(os, ctx.style);
            ctx.terminal_width_ = affix_width + width;
            if (finish) {
                if (ctx.hide)
                    ctx.apply_ansi_clear(os, ctx.terminal_width_);
                ctx.terminal_width_ = kUnset;
            }
            return os << std::flush;
        }

    protected:
        void init(const ProgressBar& ctx) const noexcept {
            prev_percnt = 0;
            affix_width = ctx.term_width(prefix) + ctx.term_width(suffix);
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

    os_t& write(io::Output mode, os_t& os) const noexcept {
        common::ScopedOverride scoped(output_mode(), mode);
        return write(os);
    }

    os_t& write(os_t& os) const noexcept {
        if (cycles <= 0 || width <= 0) return os;
        if (terminal_width_ == kUnset) {
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
        using enum Type;
        switch (type) {
            case Bar:       bar.write(os, *this);     break;
            case Alternate: alt.write(os, *this);     break;
            case Percent:   percent.write(os, *this); break;
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
