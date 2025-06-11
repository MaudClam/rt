#pragma once
#include <iostream>
#include <string>
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
    
    [[nodiscard]]
    inline bool  ok() const noexcept { return status.empty(); }
        
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

struct ShortPath {
    static constexpr size_t capacity = 512;
    using Buffer = std::array<char, capacity>;

    Buffer buffer{};
    sv_t   view = "rt.log";

    [[nodiscard]]
    Return set(sv_t sv, sv_t name) noexcept {
        if (sv.empty())
            return { "Empty path", name };

        if (sv.size() >= capacity)
            return { "Path too long", name };

        if (sv.find('\0') != sv_t::npos)
            return { "Path contains null character", name };

        std::memcpy(buffer.data(), sv.data(), sv.size());
        buffer[sv.size()] = '\0';
        view = { buffer.data(), sv.size() };
        return {};
    }
};

enum class LoggerStatusFlags : uint32_t {
    None                  = 0,
    Utf8NotInitialized    = 1 << 0,
    Utf8UnsupportedSymbol = 1 << 1,
};

template<typename Enum>
struct EnumDescriptor {
    Enum        value;
    const char* message;
};

inline void print_logger_warns(flags_t flags, os_t& os = std::cerr) noexcept {
    using Flag = LoggerStatusFlags;
    constexpr EnumDescriptor<LoggerStatusFlags> logger_flag_descriptions[] = {
        {
            Flag::Utf8NotInitialized,
            "UTF-8 locale not initialized or unsupported.\n "
            "      Unicode alignment may be incorrect."
        },
        {
            Flag::Utf8UnsupportedSymbol,
            "Unsupported symbol in UTF-8 string.\n "
            "      Unicode alignment may be incorrect."
       },
    };
    try {
        for (const auto& entry : logger_flag_descriptions)
            if ((flags & static_cast<flags_t>(entry.value)) != 0)
                os << "\n[WARN] " << entry.message << '\n';
    } catch (...) { fatal_exit(ExitCode::OutputFailure); }
}

struct Config {
	// logging
	bool tty_allowed      = false;
	bool ansi_allowed     = false;
	bool utf8_inited      = false;
	bool emoji_allowed    = false;
    ShortPath log_file;
	int indent_spaces     = 4;
	int test_param        = 0;
    flags_t logger_flags = static_cast<flags_t>(LoggerStatusFlags::None);

	// common parameters
    // ...

    ~Config() {
        if (logger_flags != 0)
            print_logger_warns(logger_flags);
    }
    
    void debug_print(os_t& os = std::cerr) const noexcept {
        try {
            os << "Config dump:\n" << std::boolalpha;
            os << "  tty_allowed:   " << tty_allowed << '\n';
            os << "  ansi_allowed:  " << ansi_allowed << '\n';
            os << "  utf8_inited:   " << utf8_inited << '\n';
            os << "  emoji_allowed: " << emoji_allowed << '\n';
            os << "  log_file:      " << log_file.view << '\n';
            os << "  indent_spaces: " << indent_spaces << '\n';
            os << "  test_param:    " << test_param << '\n';
            os << "  logger_flags:  "
            << static_cast<std::bitset<8>>(test_param) << '\n';
        } catch (...) { fatal_exit(ExitCode::OutputFailure); }
        print_logger_warns(logger_flags);
   }

    Config& set_logger_flag(LoggerStatusFlags flag) noexcept {
        logger_flags |= static_cast<flags_t>(flag);
        return *this;
    }

	inline Return parse_cmdline(int ac, char** av) noexcept {
        detect_environment();
		for (int i = 1; i < ac; ++i) {
            const char* arg = av[i];
            if (!arg)
                return { "Null argument", "argv[i]" };
            sv_t sv_arg{arg};
            if (sv_arg.find('=') != sv_t::npos) {
                if (Return r = apply_param(sv_arg); !r.ok())
                    return r;
            } else {
                if (Return r = apply_flag(arg); !r.ok())
                    return r;
            }
		}
        return {};
	}

private:
    Config& detect_environment() noexcept {
        tty_allowed   = detect_is_terminal();
        ansi_allowed  = tty_allowed;
        utf8_inited   = detect_utf8_locale();
        emoji_allowed = tty_allowed && utf8_inited;
        return *this;
    }

    [[nodiscard]] inline
    Return apply_flag(const char* arg) noexcept {
        switch (hash_31(arg)) {
            case hash_31("--no-tty"):   tty_allowed   = false; break;
            case hash_31("--no-ansi"):  ansi_allowed  = false; break;
            case hash_31("--no-utf8"):  utf8_inited   = false; break;
            case hash_31("--no-emoji"): emoji_allowed = false; break;
            case hash_31("--emoji"):    emoji_allowed = true;  break;
            default: return { "Unrecognized flag", arg };
        }
        return {};
    }

    [[nodiscard]] inline
    Return apply_param(sv_t arg) noexcept {
        size_t pos = arg.find('=');
        if (pos == sv_t::npos) return { "Expected '=' in parameter", arg };
        sv_t key = arg.substr(0, pos);
        sv_t val = safe_substr(arg, pos + 1);
        switch (hash_31(key)) {
            case hash_31("--log-file"):   return log_file.set(val, key);
            case hash_31("--indent"):     indent_spaces = parse_int(val); break;
            case hash_31("--test-param"): test_param    = parse_int(val); break;
            default: return { "Unrecognized parameter", key };
        }
        return {};
    }

    [[nodiscard]] static inline
    bool detect_is_terminal() noexcept {
        #if defined(_WIN32)
                return false;
        #else
                return isatty(fileno(stdout));
        #endif
    }

    [[nodiscard]] static inline
    bool detect_utf8_locale() noexcept {
        auto is_utf8_locale = [](const char* val) noexcept -> bool {
            if (!val) return false;
            sv_t v{val};
            return v.find("UTF-8") != sv_t::npos ||
                   v.find("utf8")  != sv_t::npos;
        };
        return is_utf8_locale(std::getenv("LANG")) ||
               is_utf8_locale(std::getenv("LC_ALL"));
    }
};

extern Config config;

} // namespace rt

