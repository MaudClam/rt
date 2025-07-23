#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <filesystem>
#include <chrono>
#include <unistd.h>      // isatty
#include <cstdlib>       // std::exit, std::getenv
#include <charconv>      // std::from_chars
#include <array>
#include <chrono>

#define LOG_LABEL_ERROR   "[ ERROR ]"
#define LOG_LABEL_WARNING "[WARNING]"

namespace rt {

using sv_t    = std::string_view;
using os_t    = std::ostream;
using flags_t = uint8_t;
namespace fs  = std::filesystem;

enum class ExitCode : uint8_t {
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

template<typename T>
class ScopedOverride {
public:
    ScopedOverride(T& target, T value) noexcept
        : ref_(target), saved_(target) {
        ref_ = std::move(value);
    }

    ScopedOverride(const ScopedOverride&) = delete;
    ScopedOverride& operator=(const ScopedOverride&) = delete;

    ~ScopedOverride() noexcept { ref_ = std::move(saved_); }

private:
    T& ref_;
    T  saved_;
};

struct Return {
    sv_t status = "";
    sv_t prompt = "";

    [[nodiscard]] bool ok() const noexcept { return status.empty(); }

    os_t& write(os_t& os = std::cerr, sv_t context = "") const noexcept {
        try {
            if (!context.empty())
                os << LOG_LABEL_ERROR << ' ' << context << ": ";
            os << status;
            if (!prompt.empty())
                os << " '" << prompt << '\'';
            os << std::endl;
        }
        catch (...) { fatal_exit(ExitCode::OutputFailure); }
        return os;
    }

    bool write_error_if(os_t& os = std::cerr, sv_t context = "") const noexcept {
        if (ok()) return false;
        write(os, context);
        return true;
    }

    explicit operator bool() const noexcept { return !ok(); }
};

[[nodiscard]] inline Return ok() noexcept { return {}; }

[[nodiscard]] inline Return error(sv_t status, sv_t prompt = {}) noexcept {
#ifdef DEBUG
    if (status.empty()) status = "CODING ERROR (missing error status)";
#else
    if (status.empty()) status = "Unknown error";
#endif
    return {status, prompt};
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
    LoggerWriteFailed      = 1 << 3,
};

template<typename Enum>
struct EnumDescriptor {
    Enum        value;
    const char* message;
};

inline void write_logger_warns(os_t& os, flags_t flags) noexcept {
    using Flags = LoggerStatusFlags;
    constexpr EnumDescriptor<LoggerStatusFlags> logger_flag_descriptions[] = {
        {
            Flags::LocaleActivationFailed,
            "Failed to activate UTF-8 locale from environment. "
            "Unicode alignment may be incorrect."
        },
        {
            Flags::Utf8NotInitialized,
            "UTF-8 locale not initialized or unsupported. "
            "Unicode alignment may be incorrect."
        },
        {
            Flags::LoggingBufferFailed,
            "Failed to create logger buffer. "
            "Data alignment may be incorrect."
        },
        {
            Flags::LoggerWriteFailed,
            "LoggerSink write() failed. "
            "Output stream is null or unreachable."
        },
    };
    try {
        for (const auto& entry : logger_flag_descriptions)
            if ((flags & static_cast<flags_t>(entry.value)) != 0)
                os << LOG_LABEL_WARNING << ' ' << entry.message << '\n';
    } catch (...) { fatal_exit(ExitCode::OutputFailure); }
}

enum class Output : uint8_t {
    Stdout     = 0b0000'0000,
    Stderr     = 0b0010'0000,
    File       = 0b0100'0000,
    Buffer     = 0b0110'0000,

    Append     = 0b0000'0001,
    Indexing   = 0b0000'0010,
    TimeIndex  = 0b0000'0100,
    CreateDirs = 0b0000'1000
};

constexpr Output OutputChannelMask = static_cast<Output>(0b1110'0000);

constexpr Output OutputPolicyMask  = static_cast<Output>(0b0001'1111);

inline constexpr Output operator|(Output a, Output b) noexcept {
    return static_cast<Output>(
        static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline constexpr Output operator&(Output a, Output b) noexcept {
    return static_cast<Output>(
        static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline constexpr bool has_flag(Output value, Output flag) noexcept {
    return static_cast<uint8_t>(value & flag) == static_cast<uint8_t>(flag);
}

inline constexpr bool output_supports_tty(Output output) noexcept {
    return (output & OutputChannelMask) == Output::Stdout ||
           (output & OutputChannelMask) == Output::Stderr;
}

inline os_t& operator<<(os_t& os, Output value) {
    if (has_flag(value, Output::File)) {
                                                  os << "File";
        if (has_flag(value, Output::Append))      os << "|Append";
        if (has_flag(value, Output::Indexing))    os << "|Indexing";
        if (has_flag(value, Output::TimeIndex))   os << "|TimeIndex";
        if (has_flag(value, Output::CreateDirs))  os << "|CreateDirs";
    } else {
        if (has_flag(value, Output::Stderr))      os << "Stderr";
        else if (has_flag(value, Output::Buffer)) os << "Buffer";
        else                                      os << "Stdout";
    }
    return os;
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
                return error("Empty string", name);
            if (sv.size() >= capacity)
                return error("String too long", name);
            if (sv.find('\0') != sv_t::npos)
                return error("String contains null character", name);
            std::memcpy(buffer_.data(), sv.data(), sv.size());
            buffer_[sv.size()] = '\0';
            view_ = { buffer_.data(), sv.size() };
            return ok();
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
    Output  output        = Output::Stdout;
    sb_t    log_file      = "rt.log";
	// Common
    sb_t    test_string;
    int     test_param    = 0;
    // ...

    ~Config() {
        if (warns_allowed && logger_flags != 0) {
            write_logger_warns(std::cerr, logger_flags);
            logger_flags = static_cast<flags_t>(LoggerStatusFlags::None);
        }
    }

    void write_config_dump(os_t& os) const noexcept {
        try {
            os << "\nCONFIG DUMP ================\n" << std::boolalpha;
            os << "Logging:\n";
            os << "  tty_allowed:   " << tty_allowed << '\n';
            os << "  ansi_allowed:  " << ansi_allowed << '\n';
            os << "  utf8_inited:   " << utf8_inited << '\n';
            os << "  emoji_allowed: " << emoji_allowed << '\n';
            os << "  warns_allowed: " << warns_allowed << '\n';
            os << "  logger_flags:  "
            << static_cast<std::bitset<8>>(logger_flags) << '\n';
            os << "  output:        " << output << '\n';
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
                return error("Null argument", "argv[i]");
            sv_t sv_arg{arg};
            if (sv_arg == "--help" || sv_arg == "-h" || sv_arg == "/h") {
                write_help(std::cout);
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
        if (ac > 1) write_config_dump(std::cerr);// FIXME: while testing
        return ok();
	}

private:
    inline void write_help(os_t& os) const noexcept {
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
        return *this;
    }

    [[nodiscard]] Return apply_flag(const char* arg) noexcept {
        switch (hash_31(arg)) {
            case hash_31("--no-tty"):   tty_allowed   = false; break;
            case hash_31("--no-ansi"):  ansi_allowed  = false; break;
            case hash_31("--no-utf8"):  utf8_inited   = false; break;
            case hash_31("--no-emoji"): emoji_allowed = false; break;
            case hash_31("--no-warns"): warns_allowed = false; break;
            default: return error("Unrecognized flag", arg);
        }
        return ok();
    }

    [[nodiscard]] Return apply_param(sv_t arg) noexcept {
        size_t pos = arg.find('=');
        if (pos == sv_t::npos) return error("Expected '=' in parameter", arg);
        sv_t key = arg.substr(0, pos);
        sv_t val = safe_substr(arg, pos + 1);
        switch (hash_31(key)) {
            case hash_31("--log-file"):    return log_file.set(val, key);
            case hash_31("--test-string"): return test_string.set(val, key);
            case hash_31("--test-param"):  test_param = parse_int(val); break;
            default: return error("Unrecognized parameter", key);
        }
        return ok();
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


// === Filepath and file output/input open utilities ===

[[nodiscard]] inline
sv_t format_timestamp_suffix() noexcept {
    static thread_local char buffer[32];
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_MSC_VER)
    localtime_s(&tm, &tt);
#elif defined(__unix__) || defined(__APPLE__)
    localtime_r(&tt, &tm);
#else
    std::tm* tmp = std::localtime(&tt);
    if (tmp) tm = *tmp;
#endif
    std::strftime(buffer, sizeof(buffer), "_%Y%m%d_%H%M%S", &tm);
    return {buffer, std::char_traits<char>::length(buffer)};
}

[[nodiscard]] inline
fs::path indexed_candidate(const fs::path& base, const fs::path& ext, int index) {
    static thread_local fs::path candidate;
    candidate = base;
    candidate += "_";
    candidate += std::to_string(index);
    candidate += ext;
    return candidate;
}

[[nodiscard]] inline
fs::path base_with_timestamp(const fs::path& base) {
    static thread_local fs::path result;
    result = base;
    result += format_timestamp_suffix();
    return result;
}

[[nodiscard]] inline std::optional<fs::path>
resolve_collision(const fs::path& base_with_ts, const fs::path& ext)  {
    std::error_code ec;
    static thread_local fs::path candidate;
    for (int i = 0; i < std::numeric_limits<int>::max(); ++i) {
        if (i == 0) {
            candidate = base_with_ts;
            candidate += ext;
        } else {
            candidate = indexed_candidate(base_with_ts, ext, i);
        }
        if (candidate.empty()) return std::nullopt;
        if (!fs::exists(candidate, ec)) return candidate;
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<fs::path>
prepare_file_path(const fs::path& path, Output mode = Output::File) noexcept {
    if (!has_flag(mode, Output::File)) return std::nullopt;
    try {
        fs::path dir  = path.parent_path();
        fs::path base = path.stem();
        fs::path ext  = path.extension();
        std::error_code ec;
        if (!dir.empty() && !fs::exists(dir, ec)) {
            if (has_flag(mode, Output::CreateDirs)) {
                fs::create_directories(dir, ec);
                if (ec || !fs::exists(dir, ec)) return std::nullopt;
            } else {
                return std::nullopt;
            }
        }
        fs::path stamped = has_flag(mode, Output::TimeIndex)
                         ? base_with_timestamp(dir / base)
                         : dir / base;
        if (stamped.empty()) return std::nullopt;
        if (has_flag(mode, Output::Indexing))
            return resolve_collision(stamped, ext);
        static thread_local fs::path candidate;
        candidate  = stamped;
        candidate += ext;
        if (!fs::exists(candidate, ec) || !has_flag(mode, Output::Append))
            return candidate;
        return std::nullopt;
    } catch (...) { return std::nullopt; }
}

[[nodiscard]] inline std::optional<fs::path>
prepare_file_path(sv_t raw_path, Output mode = Output::File) noexcept {
    try {
        fs::path full_path{raw_path.data(), raw_path.data() + raw_path.size()};
        return prepare_file_path(full_path, mode);
    } catch (...) {
        return std::nullopt;
    }
}

[[nodiscard]] inline
Return open_output_file(std::ofstream& out,
                             sv_t raw_path, Output mode = Output::File) noexcept
{
    if (!has_flag(mode, Output::File))
        return error("Output mode does not target a file");
    std::optional<fs::path> full_path = prepare_file_path(raw_path, mode);
    if (!full_path)
        return error("Invalid or inaccessible file path", raw_path);
    std::ios_base::openmode flags = std::ios::out;
    if (has_flag(mode, Output::Append))
        flags |= std::ios::app;
    else
        flags |= std::ios::trunc;
    out.open(*full_path, flags);
    if (!out.is_open()) {
        if (!fs::exists(full_path->parent_path()))
            return error("Parent directory does not exist",
                         full_path->parent_path().c_str());
        if (!fs::is_regular_file(*full_path) && fs::exists(*full_path))
            return error("Not a regular file", full_path->c_str());
        return error("Failed to open file (permission denied or I/O error)",
                     full_path->c_str());
    }
    return ok();
}

[[nodiscard]] inline
Return open_input_file(std::ifstream& in, sv_t raw_path) noexcept {
    if (raw_path.empty()) return error("empty file path");
    std::error_code ec;
    fs::path path;
    try {
        path = fs::path{raw_path.data(), raw_path.data() + raw_path.size()};
    } catch (...) {
        return error("invalid path encoding or allocation failure", raw_path);
    }
    if (!fs::exists(path, ec))
        return error("file does not exist", path.c_str());
    if (!fs::is_regular_file(path, ec))
        return error("not a regular file", path.c_str());
    in.open(path);
    if (!in.is_open())
        return error("failed to open file (permission denied or I/O error)",
                     path.c_str());
    return ok();
}

// === End of filepath and file output/input open utilities ===


} // namespace rt

