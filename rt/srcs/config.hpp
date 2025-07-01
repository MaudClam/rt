#pragma once
#include <iostream>
#include <string>
#include <cassert>
#include <unistd.h>      // isatty
#include <cstdlib>       // std::exit, std::getenv
#include <charconv>      // std::from_chars
#include <array>

namespace rt {

using sv_t    = std::string_view;
using os_t    = std::ostream;
using flags_t = uint8_t;

enum class ExitCode {
    Success       = EXIT_SUCCESS,
    UnknownError  = EXIT_FAILURE,
    OutputFailure = 2,
};

[[noreturn]] inline void fatal_exit(ExitCode code) noexcept {
    std::exit(static_cast<int>(code));
}

[[noreturn]] inline void graceful_exit() noexcept {
    std::exit(static_cast<int>(ExitCode::Success));
}

struct Return {
    sv_t status = "";
    sv_t prompt = "";

    [[nodiscard]] inline bool ok() const noexcept { return status.empty(); }

    inline os_t& write(os_t& os = std::cerr) const noexcept {
        try { os << status << " '" << prompt << "'\n"; }
        catch (...) { fatal_exit(ExitCode::OutputFailure); }
        return os;
    }

    [[nodiscard]]
    inline bool write_error_if(os_t& os = std::cerr) const noexcept {
        if (ok()) return false;
        write(os);
        return true;
    }

    inline explicit operator bool() const noexcept { return !ok(); }
};

inline os_t& operator<<(os_t& os, const Return& ret) noexcept {
    return ret.write(os);
}

inline constexpr
uint32_t hash_31_impl(const char* data, size_t size) noexcept {
    uint32_t h = 0;
    for (size_t i = 0; i < size; ++i)
        h = h * 31 + static_cast<unsigned char>(data[i]);
    return h;
}

template <size_t N>
constexpr uint32_t hash_31(const char (&literal)[N]) noexcept {
    return hash_31_impl(literal, N - 1);
}

inline constexpr
uint32_t hash_31(sv_t sv) noexcept {
    return hash_31_impl(sv.data(), sv.size());
}

inline constexpr
int parse_int(sv_t sv, int fallback = 0) noexcept {
    int value = fallback;
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
    return (ec == std::errc{}) ? value : fallback;
}

[[nodiscard]] inline constexpr
sv_t safe_substr(sv_t sv, size_t pos, size_t count = sv_t::npos) noexcept {
    return sv.substr(std::min(pos, sv.size()), count);
}

enum class LoggerStatusFlags : flags_t {
    None                   = 0,
    LocaleActivationFailed = 1 << 0,
    Utf8NotInitialized     = 1 << 1,
    LoggingBufferFailed    = 1 << 2,
};

template<typename Enum>
struct EnumDescriptor {
    Enum        value;
    const char* message;
};

inline void print_logger_warns(flags_t flags, os_t& os = std::cerr) noexcept {
    using Flags = LoggerStatusFlags;
    constexpr EnumDescriptor<LoggerStatusFlags> logger_flag_descriptions[] = {
        {
            Flags::LocaleActivationFailed,
            "Failed to activate UTF-8 locale from environment.\n "
            "      Unicode alignment may be incorrect."
        },
        {
            Flags::Utf8NotInitialized,
            "UTF-8 locale not initialized or unsupported.\n "
            "      Unicode alignment may be incorrect."
        },
        {
            Flags::LoggingBufferFailed,
            "Failed to create logger buffer.\n "
            "      Data alignment may be incorrect."
        },
    };
    try {
        for (const auto& entry : logger_flag_descriptions)
            if ((flags & static_cast<flags_t>(entry.value)) != 0)
                os << "\n[WARN] " << entry.message << '\n';
    } catch (...) { fatal_exit(ExitCode::OutputFailure); }
}

struct Config {
    struct StringBuffer {
        static constexpr size_t capacity = 512;
        using Buffer = std::array<char, capacity>;
        
        StringBuffer() = default;

        template <size_t N>
        constexpr StringBuffer(const char (&str)[N]) noexcept {
            static_assert(N > 1, "String literal cannot be empty");
            const Return r = set(sv_t{str, N - 1}, "StringBuffer literal");
            assert(r.ok() && "Invalid default literal for StringBuffer");
        }

        [[nodiscard]] sv_t view() const noexcept { return view_; }

        [[nodiscard]] Return set(sv_t sv, sv_t name) noexcept {
            if (sv.empty())
                return { "Empty string", name };
            if (sv.size() >= capacity)
                return { "String too long", name };
            if (sv.find('\0') != sv_t::npos)
                return { "String contains null character", name };
            std::memcpy(buffer_.data(), sv.data(), sv.size());
            buffer_[sv.size()] = '\0';
            view_ = { buffer_.data(), sv.size() };
            return {};
        }

    private:
        Buffer buffer_{};
        sv_t   view_{};
    };
    using sb_t = StringBuffer;

	// Logging
	bool    tty_allowed   = true;
	bool    ansi_allowed  = true;
	bool    utf8_inited   = true;
    bool    emoji_allowed = true;
    bool    warns_allowed = true;
    flags_t logger_flags  = static_cast<flags_t>(LoggerStatusFlags::None);
    sb_t    log_file      = "rt.log";
	// Common
    sb_t    test_string;
    int     test_param    = 9;
    // ...

    ~Config() {
        if (warns_allowed && logger_flags != 0)
            print_logger_warns(logger_flags);
    }
    
    void debug_print(os_t& os = std::cerr) const noexcept {
        try {
            os << "\nCONFIG DUMP ================\n" << std::boolalpha;
            os << "Logging:\n";
            os << "  tty_allowed:   " << tty_allowed << '\n';
            os << "  ansi_allowed:  " << ansi_allowed << '\n';
            os << "  utf8_inited:   " << utf8_inited << '\n';
            os << "  emoji_allowed: " << emoji_allowed << '\n';
            os << "  logger_flags:  "
            << static_cast<std::bitset<8>>(logger_flags) << '\n';
            os << "  log_file:      " << log_file.view() << '\n';
            os << "Common:\n";
            os << "  test_string:   " << test_string.view() << '\n';
            os << "  test_param:    " << test_param << '\n';
            os << "CONFIG DUMP END ============\n\n";
        } catch (...) { fatal_exit(ExitCode::OutputFailure); }
   }

    bool has_logger_flag(LoggerStatusFlags flag) const noexcept {
        return (logger_flags & static_cast<flags_t>(flag)) != 0;
    }

    Config& set_logger_flag(LoggerStatusFlags flag) noexcept {
        if (!has_logger_flag(flag))
            logger_flags |= static_cast<flags_t>(flag);
        return *this;
    }
    
	inline Return parse_cmdline(int ac, char** av) noexcept {
		for (int i = 1; i < ac; ++i) {
            const char* arg = av[i];
            if (!arg)
                return { "Null argument", "argv[i]" };
            sv_t sv_arg{arg};
            if (sv_arg == "--help" || sv_arg == "-h" || sv_arg == "/h") {
                print_help();
                graceful_exit();
            }
            if (sv_arg.find('=') != sv_t::npos) {
                if (Return r = apply_param(sv_arg); !r.ok())
                    return r;
            } else {
                if (Return r = apply_flag(arg); !r.ok())
                    return r;
            }
		}
        detect_environment();
        if (ac > 1) debug_print();// FIXME: while testing
        return {};
	}

private:
    inline void print_help(os_t& os = std::cout) const noexcept {
        try {
            os << "Usage: program [flags] [parameters]\n\n";
            os << "Flags:\n";
            os << "  --no-tty      Disable TTY detection\n";
            os << "  --no-ansi     Disable ANSI escape codes\n";
            os << "  --no-utf8     Disable UTF-8 output, fallback to ASCII\n";
            os << "  --no-emoji    Disable emoji in output\n";
            os << "  --no-warns    Disable deferred logger warnings\n\n";
            os << "Parameters:\n";
            os << "  --log-file=path       Path to log file\n";
            os << "  --test-string=value   Test string value\n";
            os << "  --test-param=N        Test numeric parameter\n\n";
        } catch (...) { fatal_exit(ExitCode::OutputFailure); }
    }

    Config& detect_environment() noexcept {
        tty_allowed   = tty_allowed && detect_is_terminal();
        ansi_allowed  = ansi_allowed && tty_allowed;
        if (utf8_inited) {
            if (environment_declares_utf8()) {
                utf8_inited = try_activate_utf8_locale();
                if (!utf8_inited)
                    set_logger_flag(LoggerStatusFlags::LocaleActivationFailed);
            } else {
                utf8_inited = false;
            }
        }
//        emoji_allowed = emoji_allowed && utf8_inited;
        return *this;
    }

    [[nodiscard]] Return apply_flag(const char* arg) noexcept {
        switch (hash_31(arg)) {
            case hash_31("--no-tty"):   tty_allowed   = false; break;
            case hash_31("--no-ansi"):  ansi_allowed  = false; break;
            case hash_31("--no-utf8"):  utf8_inited   = false; break;
            case hash_31("--no-emoji"): emoji_allowed = false; break;
            case hash_31("--no-warns"): warns_allowed = false; break;
            default: return { "Unrecognized flag", arg };
        }
        return {};
    }

    [[nodiscard]] Return apply_param(sv_t arg) noexcept {
        size_t pos = arg.find('=');
        if (pos == sv_t::npos) return { "Expected '=' in parameter", arg };
        sv_t key = arg.substr(0, pos);
        sv_t val = safe_substr(arg, pos + 1);
        switch (hash_31(key)) {
            case hash_31("--log-file"):    return log_file.set(val, key);
            case hash_31("--test-string"): return test_string.set(val, key);
            case hash_31("--test-param"):  test_param = parse_int(val); break;
            default: return { "Unrecognized parameter", key };
        }
        return {};
    }

    [[nodiscard]] static bool detect_is_terminal() noexcept {
        #if defined(_WIN32)
                return false;
        #else
                return isatty(fileno(stdout));
        #endif
    }

    [[nodiscard]] static bool environment_declares_utf8() noexcept {
        auto is_utf8_locale = [](const char* val) noexcept -> bool {
            if (!val) return false;
            sv_t v{val};
            return v.find("UTF-8") != sv_t::npos ||
                   v.find("utf8")  != sv_t::npos;
        };
        return is_utf8_locale(std::getenv("LANG")) ||
               is_utf8_locale(std::getenv("LC_ALL"));
    }

    [[nodiscard]] static bool try_activate_utf8_locale() noexcept {
        const char* r = std::setlocale(LC_CTYPE, "");
        if (!r) return false;
        sv_t v{r};
        return v.find("UTF-8") != sv_t::npos || v.find("utf8") != sv_t::npos;
    }

    [[nodiscard]] static bool detect_utf8_locale() noexcept {
        const char* current = std::setlocale(LC_CTYPE, nullptr);
        if (!current) return false;
        sv_t v{current};
        return v.find("UTF-8") != sv_t::npos || v.find("utf8")  != sv_t::npos;
    }
};

extern Config config;

} // namespace rt

