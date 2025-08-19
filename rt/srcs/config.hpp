#pragma once
#if NDEBUG
 constexpr bool debug_mode = false;
#else
 constexpr bool debug_mode = true;
#endif

#include <string_view>
#include <ostream>
#include <string>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <memory>
#include "common.hpp"
#include "logging/ansi_enums_naming.hpp"
#include "logging/logging_warns.hpp"
#include "logging/io_channels.hpp"

namespace rt {

using sv_t   = std::string_view;
using os_t   = std::ostream;
using oss_t  = std::ostringstream;
namespace fs = std::filesystem;

struct Config;
extern Config config;
inline constexpr sv_t default_config_name = "config.ini";
inline constexpr sv_t default_log_name    = "rt.log";
inline auto stdout_mutex = std::make_shared<std::mutex>();
inline auto stderr_mutex = std::make_shared<std::mutex>();

fs::path get_exec_path();

inline constexpr sv_t cmdline_help = R"help(
Usage: program [flags] [parameters]

Flags:
  --no-tty      Disable TTY detection
  --no-ansi     Disable ANSI escape codes
  --no-utf8     Disable UTF-8 output, fallback to ASCII
  --no-emoji    Disable emoji in output
  --no-warns    Disable deferred logger warnings
  --config-dump Write config dump

Parameters:
  --config=path         Path to the another user defined config file
  --log-out=target      Logging output (stdout, stderr, file)
  --log-file=path       Path to log file
  --test-string=value   Test string value
  --test-param=N        Test numeric parameter

)help";

struct Config {
    using Output   = logging::io::Output;
    using LogWarns = logging::LogWarns;
    using Warn     = logging::Warn;
    using flags_t  = logging::flags_t;

	// Logging
    ansi::Color      tty_foreground = ansi::Color::White;
    ansi::Background tty_background = ansi::Background::Black;
	bool tty_allowed    = true;
	bool ansi_allowed   = true;
	bool utf8_inited    = true;
    bool emoji_allowed  = true;
    bool warns_allowed  = true;
    Output    log_out   = Output::Stderr;
    StrBuffer log_file  = default_log_name;
    mutable LogWarns log_warns;

	// Common
    bool      config_dump = false;
    StrBuffer test_string;
    int       test_param = 0;
    // ...

    ~Config() noexcept { flush_log_warns(std::cerr); }
    
    os_t& flush_log_warns(os_t& os) noexcept {
        if (warns_allowed && !log_warns.test(Warn::None)) {
            try {
                os << log_warns;
                log_warns.reset();
            }
            catch (...) { fatal_exit(ExitCode::OutputFailure); }
        }
        return os;
    }

    os_t& write_config_dump(os_t& os) const noexcept {
        os << "\nCONFIG DUMP ================\n" << std::boolalpha;
        os << "Logging:\n";
        os << "  tty_foreground: " << tty_foreground << '\n';
        os << "  tty_background: " << tty_background << '\n';
        os << "  tty_allowed:    " << tty_allowed    << '\n';
        os << "  ansi_allowed:   " << ansi_allowed   << '\n';
        os << "  utf8_inited:    " << utf8_inited    << '\n';
        os << "  emoji_allowed:  " << emoji_allowed  << '\n';
        os << "  warns_allowed:  " << warns_allowed  << '\n';
        os << "  log_out:        " << log_out        << '\n';
        os << "  log_file:       '" << log_file.view() << "'\n";
        os << "  log_warns:      " << std::bitset<8>(log_warns.bits()) << '\n';

        os << "Common:\n";
        os << "  config_dump:    " << config_dump        << '\n';
        os << "  test_string:    '" << test_string.view() << "'\n";
        os << "  test_param:     " << test_param << '\n';
        os << "CONFIG DUMP END ============\n\n";
        return os;
   }
    
    Config& init(int ac, char** av) noexcept {
        try {
            load_config_file(config_path(ac, av));
        } catch (const std::exception& e) {
            std::cerr << "Failed to load config file: " << e.what() << "\n";
        } catch (...) {
            std::cerr << "Unknown error while loading config file.\n";
        }
        parse_cmdline(ac, av);
        detect_environment();
        if constexpr (debug_mode)
            if (config_dump)
                write_config_dump(std::cerr);
        return *this;
    }
    
private:
    Config& load_config_file(fs::path path) {
        using namespace logging::io;
        ifs_t  file;
        Return r = open_input_file(file, path);
        if (!r.ok()) {
            r.write_error("Error open config file:");
            return *this;
        }
        bool fatal = false;
        int  num = 0;
        oss_t errors;
        std::string line;
        while (std::getline(file, line)) {
            ++num;
            if (line.empty() || line.starts_with('#')) continue;
            r = apply_arg(line);
            if (!r.ok()) {
                errors << tl_copy(path.c_str()) << ":" << num << ": ";
                errors << "[ERROR]: " << r << '\n';
                fatal |= r.fatal_err;
            }
        }
        if (!errors.view().empty()) {
            std::cerr << errors.view();
            if (fatal) fatal_exit(ExitCode::CfgFileFailure);
        }
        file.close();
        return *this;
    }

    Config& parse_cmdline(int ac, char** av) noexcept {
        Return r{ "Null argument", "argv[i]" };
		for (int i = 1; i < ac; ++i) {
            if (!av[i]) fatal_exit(r.write_error({}, ExitCode::CmdlineFailure));
            sv_t sv_arg{av[i]};
            if (sv_arg == "--help" || sv_arg == "-h" || sv_arg == "/h") {
                std::cout << cmdline_help;
                graceful_exit();
            }
            if (sv_arg.starts_with("--")) {
                r = apply_arg(safe_substr(sv_arg, 2));
                if (!r.ok()) r.prompt = sv_arg;
            } else {
                r = { "Unrecognized argument", sv_arg };
            }
            if (!r.ok())
                fatal_exit(r.write_error({}, ExitCode::CmdlineFailure));
		}
        return *this;
	}

    Config& detect_environment() noexcept {
        tty_allowed  = tty_allowed && detect_is_terminal();
        ansi_allowed = ansi_allowed && tty_allowed;
        if (utf8_inited) {
            if (environment_declares_utf8()) {
                utf8_inited = try_activate_utf8_locale();
                if (!utf8_inited)
                    log_warns.set(Warn::LocaleActivationFailed);
            } else {
                utf8_inited = false;
                log_warns.set(Warn::Utf8NotInitialized);
            }
        }
        return *this;
    }

    [[nodiscard]] fs::path config_path(int ac, char** av) {
        sv_t raw_cmdline_path = default_config_name;
        for (int i = 1; i < ac; ++i) {
            sv_t sv_arg{av[i]};
            if (sv_arg.starts_with("--config="))
                raw_cmdline_path = safe_substr(sv_arg, 9);
            if (sv_arg.starts_with("--cfg-dump"))
                config_dump = true;
        }
        fs::path cmdline_path{raw_cmdline_path};
        std::error_code ec;
        if (cmdline_path.is_absolute() && fs::exists(cmdline_path, ec) && !ec)
            return cmdline_path;
        fs::path possible_paths[] = {
            cmdline_path,
            get_exec_path().parent_path() / cmdline_path,
            fs::current_path() / cmdline_path,
            default_config_name,
            fs::path{".."} / default_config_name,
            fs::path{"../.."} / default_config_name,
        };
        for (const auto& raw : possible_paths) {
            fs::path p = fs::weakly_canonical(raw, ec);
            if constexpr (debug_mode)
                if (config_dump)
                    std::cerr << p << "\n";//FIXME: 1
            if (fs::exists(p, ec))
                return p;
        }
        return cmdline_path;
    }

    [[nodiscard]] Return apply_arg(sv_t sv_arg) noexcept {
        if (sv_arg.find('=') != sv_t::npos)
            return apply_param(sv_arg);
        return apply_flag(sv_arg);
    }

    [[nodiscard]] Return apply_flag(sv_t sv_arg) noexcept {
        switch (hash_31(sv_arg)) {
            case hash_31("no-tty"):      tty_allowed   = false; break;
            case hash_31("no-ansi"):     ansi_allowed  = false; break;
            case hash_31("no-utf8"):     utf8_inited   = false; break;
            case hash_31("no-emoji"):    emoji_allowed = false; break;
            case hash_31("no-warns"):    warns_allowed = false; break;
            case hash_31("config-dump"): config_dump   = true;  break;
            default: return error("Unrecognized flag", sv_arg, true);
        }
        return ok();
    }

    [[nodiscard]] Return apply_param(sv_t arg) noexcept {
        size_t pos = arg.find('=');
        if (pos == sv_t::npos) return error("Expected '=' in parameter", arg);
        sv_t key = trim(safe_substr(arg, 0, pos));
        sv_t val = trim(safe_substr(arg, pos + 1));
        switch (hash_31(key)) {
            case hash_31("tty-foreground"): {
                ansi::Color c = find_named_enum(ansi::enumColors, val);
                if (c == static_cast<ansi::Color>(-1))
                    return error("Unknown tty foreground color", val);
                tty_foreground = c;
                break;
            }
            case hash_31("tty-background"): {
                ansi::Background b = find_named_enum(ansi::enumBackgrounds, val);
                if (b == static_cast<ansi::Background>(-1))
                    return error("Unknown tty background color", val);
                tty_background = b;
                break;
            }
            case hash_31("log-out"): {
                if      (val == "stdout") log_out = Output::Stdout;
                else if (val == "stderr") log_out = Output::Stderr;
                else if (val == "file")   log_out = Output::File |
                                                    Output::CreateDirs |
                                                    Output::Indexing |
                                                    Output::TimeIndex;
                else return error("Invalid value for log-out", val);
                break;
            }
            case hash_31("log-file"):    return log_file.set(val, key);
            case hash_31("test-string"): return test_string.set(val, key);
            case hash_31("test-param"):  test_param = parse_int(val); break;
            case hash_31("config"):      break;
            default: return error("Unrecognized parameter", key, true);
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

    [[nodiscard]] static bool contains_utf8_ci(sv_t v) noexcept {
        const auto tolow = [](unsigned char c) noexcept -> char {
            if (c >= 'A' && c <= 'Z') return char(c - 'A' + 'a');
            return char(c);
        };
        const size_t n = v.size();
        for (size_t i = 0; i < n; ++i) {
            if (tolow(v[i]) == 'u' && i + 3 < n &&
                tolow(v[i + 1]) == 't' && tolow(v[i + 2]) == 'f')
            {
                size_t j = i + 3;
                if (j < n && (v[j] == '-' || v[j] == '_')) ++j;
                if (j < n && v[j] == '8') return true;
            }
        }
        return false;
    }

    [[nodiscard]] static bool environment_declares_utf8() noexcept {
        if (const char* lc_all = std::getenv("LC_ALL")) {
            return contains_utf8_ci(sv_t{lc_all});
        }
        if (const char* lc_ctype = std::getenv("LC_CTYPE")) {
            return contains_utf8_ci(sv_t{lc_ctype});
        }
        if (const char* lang = std::getenv("LANG")) {
            return contains_utf8_ci(sv_t{lang});
        }
        return false;
    }

    [[nodiscard]] static bool try_activate_utf8_locale() noexcept {
        if (const char* r = std::setlocale(LC_CTYPE, "")) {
            return contains_utf8_ci(sv_t{r});
        }
        return false;
    }
};

} // namespace rt
