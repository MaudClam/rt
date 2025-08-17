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
#include "common.hpp"
#include "logging/ansi_enums_naming.hpp"
#include "logging/logging_warns.hpp"
#include "logging/io_channels.hpp"

namespace rt {

struct Config;
extern thread_local      Config config;
extern std::thread::id   main_thread_id;
extern logging::LogWarns log_warns_collected;
extern std::mutex        log_warns_mutex;

using sv_t   = std::string_view;
using os_t   = std::ostream;
using oss_t  = std::ostringstream;
namespace fs = std::filesystem;


fs::path get_exec_path();

inline constexpr sv_t cmdline_help = R"help(
Usage: program [flags] [parameters]

Flags:
  --no-tty      Disable TTY detection
  --no-ansi     Disable ANSI escape codes
  --no-utf8     Disable UTF-8 output, fallback to ASCII
  --no-emoji    Disable emoji in output
  --no-warns    Disable deferred logger warnings

Parameters:
  --log-out=target      Logging output (stdout, stderr, file)
  --log-file=path       Path to log file
  --test-string=value   Test string value
  --test-param=N        Test numeric parameter
)help";

struct Config {
    using Output   = logging::io::Output;
    using LogWarns = logging::LogWarns;
    using flags_t  = logging::flags_t;

	// Logging
    ansi::Color      tty_foreground = ansi::Color::White;
    ansi::Background tty_background = ansi::Background::Black;
	bool tty_allowed    = true;
	bool ansi_allowed   = true;
	bool utf8_inited    = true;
    bool emoji_allowed  = true;
    bool warns_allowed  = true;
    Output    log_out   = Output::Stdout;
    StrBuffer log_file  = "rt.log";
    LogWarns  log_warns = LogWarns::None;

	// Common
    StrBuffer test_string;
    int       test_param = 0;
    // ...

    ~Config() noexcept {
        if (log_warns != LogWarns::None) {
            std::lock_guard lock(log_warns_mutex);
            log_warns_collected |= log_warns;
        }
        if (std::this_thread::get_id() == main_thread_id &&
            log_warns_collected != LogWarns::None)
        {
            try { std::cerr << log_warns_collected; }
            catch (...) { fatal_exit(ExitCode::OutputFailure); }
        }
    }

    os_t& write_config_dump(os_t& os) const noexcept {
        os << "\nCONFIG DUMP ================\n" << std::boolalpha;
        os << "Logging:\n";
        os << "  tty_foreground: " << tty_foreground  << '\n';
        os << "  tty_background: " << tty_background  << '\n';
        os << "  tty_allowed:    " << tty_allowed     << '\n';
        os << "  ansi_allowed:   " << ansi_allowed    << '\n';
        os << "  utf8_inited:    " << utf8_inited     << '\n';
        os << "  emoji_allowed:  " << emoji_allowed   << '\n';
        os << "  warns_allowed:  " << warns_allowed   << '\n';
        os << "  log_out:        " << log_out         << '\n';
        os << "  log_file:       " << log_file.view() << '\n';

        os << "  log_warns:      "
        << log_warns
        << static_cast<std::bitset<8>>(static_cast<logging::flags_t>(log_warns))
        << '\n';

        os << "Common:\n";
        os << "  test_string:    " << test_string.view()    << '\n';
        os << "  test_param:     " << test_param << '\n';
        os << "CONFIG DUMP END ============\n\n";
        return os;
   }
    
    Config& init(int ac, char** av) noexcept {
        try {
            fs::path relative = relative_config_path(ac, av);
            fs::path path = fs::exists(fs::current_path() / relative)
                          ? fs::current_path() / relative
                          : get_exec_path().parent_path() / relative;
            load_config_file(path);
        } catch (const std::exception& e) {
            std::cerr << "Failed to load config file: " << e.what() << "\n";
        } catch (...) {
            std::cerr << "Unknown error while loading config file.\n";
        }
        parse_cmdline(ac, av);
        detect_environment();
        main_thread_id = std::this_thread::get_id();
        if constexpr (!debug_mode)
            if (ac > 1)
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
                errors << path.c_str() << ":" << num << ":\n";
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
            if (sv_arg.starts_with("--")) r = apply_arg(safe_substr(sv_arg, 2));
            else r = { "Unrecognized argument", sv_arg };
            if (!r.ok()) fatal_exit(r.write_error({}, ExitCode::CmdlineFailure));
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
                    logging::set_log_warn(log_warns,
                                          LogWarns::LocaleActivationFailed);
            } else {
                utf8_inited = false;
            }
        }
        return *this;
    }

    [[nodiscard]] sv_t relative_config_path(int ac, char** av) {
        for (int i = 1; i < ac; ++i) {
            sv_t sv_arg{av[i]};
            if (sv_arg.starts_with("--config="))
                return safe_substr(sv_arg, 9);
        }
        return {"config.ini"};
    }

    [[nodiscard]] Return apply_arg(sv_t sv_arg) noexcept {
        if (sv_arg.find('=') != sv_t::npos)
            return apply_param(sv_arg);
        return apply_flag(sv_arg);
    }

    [[nodiscard]] Return apply_flag(sv_t sv_arg) noexcept {
        switch (hash_31(sv_arg)) {
            case hash_31("no-tty"):   tty_allowed   = false; break;
            case hash_31("no-ansi"):  ansi_allowed  = false; break;
            case hash_31("no-utf8"):  utf8_inited   = false; break;
            case hash_31("no-emoji"): emoji_allowed = false; break;
            case hash_31("no-warns"): warns_allowed = false; break;
            default: return error("Unrecognized flag", sv_arg);
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
                if (c == ansi::Color::Default)
                    return error("tty-foreground cannot be 'Default'", val);
                tty_foreground = c;
                break;
            }
            case hash_31("tty-background"): {
                ansi::Background b = find_named_enum(ansi::enumBackgrounds, val);
                if (b == static_cast<ansi::Background>(-1))
                    return error("Unknown tty background color", val);
                if (b == ansi::Background::Default)
                    return error("tty-background cannot be 'Default'", val);
                tty_background = b;
                break;
            }
            case hash_31("log-out"): {
                if      (val == "stdout") log_out = Output::Stdout;
                else if (val == "stderr") log_out = Output::Stderr;
                else if (val == "file")   log_out = Output::File |
                                                    Output::TimeIndex |
                                                    Output::CreateDirs;
                else return error("Invalid value for log-out", val);
                break;
            }
            case hash_31("log-file"):    return log_file.set(val, key);
            case hash_31("test-string"): return test_string.set(val, key);
            case hash_31("test-param"):  test_param = parse_int(val); break;
            case hash_31("config"):      break;
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

} // namespace rt
